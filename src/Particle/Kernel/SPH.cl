#include "NTL.hl"

#define FLAG_FOCUS (1 << 0)
#define FLAG_NEIG  (1 << 1)

void __kernel SPH_Init(__global ParticleDataSPH *dataSPH,
                       __global ModuleParamSPH *moduleParam,
                       __global unsigned int *dataCellIndex,
                       __global unsigned int *dataParticleIndex) {
    uint g_id = get_global_id(0);
    __global ParticleDataSPH *sph = &dataSPH[g_id];

    sph->density = 0.f;
    sph->pressure = 0.f;
    sph->mass = 1.f;
    sph->force1 = 0.0f;
    sph->force2 = 0.0f;
    sph->velocity = 0.0f;

    dataCellIndex[g_id] = 0xFFFFFFFF;
    dataParticleIndex[g_id] = g_id;
}

bool sdSphere(float3 centerOfSphere, float radius, float3 checkPosition, float *t) {
    float t0 = length(centerOfSphere - checkPosition);
    *t = t0 - radius;
    return (t0 < radius);
}

uint GetFlatCellIndex(int3 cellIndex) {
    const uint p1 = 73856093; // some large primes
    const uint p2 = 19349663;
    const uint p3 = 83492791;
    int n = p1 * cellIndex.x ^ p2 * cellIndex.y ^ p3 * cellIndex.z;
    const uint TOTAL_GRID_CELL_COUNT = 128 * 128 * 64;
    n %= TOTAL_GRID_CELL_COUNT;
    return n;
}

void __kernel SPH_UpdateCellIndex(__global ParticleData *dataParticle,
                                  __global ParticleDataSPH *dataSPH,
                                  __global ModuleParamSPH *moduleParam,
                                  __global unsigned int *dataCellIndex,
                                  __global unsigned int *dataParticleIndex,
                                  __global unsigned int *cellOffsetBuffer) {
    uint g_id = (uint)get_global_id(0);

    uint particleIndex = dataParticleIndex[g_id];
    __global ParticleData *particleA = &dataParticle[particleIndex];
    __global ParticleDataSPH *sphA = &dataSPH[particleA->index];

    float3 cellIndexf = floor(particleA->position / moduleParam->smoothingRadius);
    int3 cellIndex;
    cellIndex.x = cellIndexf.x;
    cellIndex.y = cellIndexf.y;
    cellIndex.z = cellIndexf.z;
    uint flatCellIndex = GetFlatCellIndex(cellIndex);

    dataCellIndex[particleIndex] = flatCellIndex;

    int size = (128 * 128 * 64) / get_global_size(0);
    int start = g_id * size;

    for (int i = 0; i < size; i++) {
        cellOffsetBuffer[start + i] = 0xFFFFFFFF;
    }
}

void __kernel SPH_UpdateCellOffset(__global ParticleData *dataParticle,
                                   __global ParticleDataSPH *dataSPH,
                                   __global ModuleParamSPH *moduleParam,
                                   __global unsigned int *dataCellIndex,
                                   __global unsigned int *dataParticleIndex,
                                   __global unsigned int *cellOffsetBuffer) {

    uint g_id = (uint)get_global_id(0);

    uint particleIndex = dataParticleIndex[g_id];
    uint cellIndex = dataCellIndex[particleIndex];
    atomic_min(&cellOffsetBuffer[cellIndex], g_id);

    //printf("g_id[%u] cellIndex[%u] = %u\n", g_id, cellIndex, cellOffsetBuffer[cellIndex]);
}

void __kernel SPH_UpdateDensity(__global ParticleData *dataParticle,
                                __global ParticleDataSPH *dataSPH,
                                __global ModuleParamSPH *moduleParam,
                                __local ParticleDataSPH *sharedSPH,
                                __global unsigned int *dataCellIndex,
                                __global unsigned int *dataParticleIndex,
                                __global unsigned int *cellOffsetBuffer,
                                int focus) {
    uint work_dim = get_work_dim();

    uint g_id = (uint)get_global_id(0);
    uint l_id = (uint)get_local_id(0);
    uint l_size = (uint)get_local_size(0);

    uint group_id = (uint)get_group_id(0);
    uint n_groups = (uint)get_num_groups(0);

    // PARAMS
    const float K = moduleParam->pressure;    //pressure constant parameter
    const float p0 = moduleParam->densityRef; //reference density param

    const float h = moduleParam->smoothingRadius; // smoothing radius parameter (float)
    const float h2 = h * h;
    const float h9 = pow(h, 9);

    const float Poly6 = (315.0f / (64.0f * M_PI * h9));

    __global ParticleData *particleA = &dataParticle[g_id];
    /*

    __global ParticleDataSPH *sphA = &dataSPH[particleA->index];

    sphA->position = particleA->position;
    sphA->density = 0;

    barrier(CLK_GLOBAL_MEM_FENCE);
    
    for (uint tile = 0; tile < n_groups; ++tile) {
        uint offset = tile * l_size;
        sharedSPH[l_id] = dataSPH[offset + l_id];
        barrier(CLK_LOCAL_MEM_FENCE);

        for (uint i = 0; i < l_size; ++i) {
            ParticleDataSPH sphB = sharedSPH[i];

            const float3 diff = sphA->position - sphB.position;
            const float r2 = dot(diff, diff);
            if (r2 < h2) // h2 = h*h
            {
                const float W = Poly6 * pow(h2 - r2, 3);
                sphA->density += sphB.mass * W;
            }
        }
        // avoid negative pressure by clamping density to reference value:
        sphA->density = max(p0, sphA->density);

        sphA->pressure = K * (sphA->density - p0);
        barrier(CLK_LOCAL_MEM_FENCE);
    }
    */

    dataSPH[particleA->index].flag = 0;
    if (g_id == focus) {
        dataSPH[particleA->index].flag = 1;
    }
    ParticleDataSPH sphA = dataSPH[particleA->index];
    sphA.position = particleA->position;
    sphA.density = 0;
    barrier(CLK_GLOBAL_MEM_FENCE);

    float3 cellIndexf = floor(particleA->position / moduleParam->smoothingRadius);
    int3 cellIndex;
    cellIndex.x = cellIndexf.x;
    cellIndex.y = cellIndexf.y;
    cellIndex.z = cellIndexf.z;

    for (int i = -1; i <= 1; ++i) {
        for (int j = -1; j <= 1; ++j) {
            for (int k = -1; k <= 1; ++k) {
                int3 neighborIndex = cellIndex + (int3)(i, j, k);
                uint flatNeighborIndex = GetFlatCellIndex(neighborIndex);

                // look up the offset to the cell:
                uint neighborIterator = cellOffsetBuffer[flatNeighborIndex];
                //                if (!g_id)
                //                    printf("at[%i %i %i] cellOffset[%i]\n", cellIndex.x, cellIndex.y, cellIndex.z, cellOffsetBuffer[flatNeighborIndex]);

                // iterate through particles in the neighbour cell (if iterator offset is valid)
                while (neighborIterator != 0xFFFFFFFF && neighborIterator < get_global_size(0)) {
                    uint particleIndexB = dataParticleIndex[neighborIterator];
                    if (dataCellIndex[particleIndexB] != flatNeighborIndex) {
                        break; // it means we stepped out of the neighbour cell list!
                    }

                    // Here you can load particleB and do the SPH evaluation logic
                    __global ParticleDataSPH *sphB = &dataSPH[particleIndexB];
                    if (!sphB->flag)
                        sphB->flag = 2;

                    const float3 diff = sphA.position - sphB->position;
                    const float r2 = dot(diff, diff);
                    if (r2 < h2) // h2 = h*h
                    {
                        const float W = Poly6 * pow(h2 - r2, 3);
                        sphA.density += sphB->mass * W;
                    }

                    neighborIterator++; // iterate...
                }
            }
        }
    }
    sphA.density = max(p0, sphA.density);
    sphA.pressure = K * (sphA.density - p0);

    dataSPH[particleA->index] = sphA;

    if (g_id == focus) {
        printf("FLAG : %i\n", dataSPH[particleA->index].flag);
    }
}

void __kernel SPH_UpdatePressure(__global ParticleData *dataParticle,
                                 __global ParticleDataSPH *dataSPH,
                                 __global ModuleParamSPH *moduleParam,
                                 __local ParticleDataSPH *sharedSPH) {
    uint work_dim = get_work_dim();

    uint g_id = (uint)get_global_id(0);
    uint l_id = (uint)get_local_id(0);
    uint l_size = (uint)get_local_size(0);

    uint group_id = (uint)get_group_id(0);
    uint n_groups = (uint)get_num_groups(0);

    // PARAMS
    const float K = moduleParam->pressure;    //pressure constant parameter
    const float p0 = moduleParam->densityRef; //reference density param

    const float h = moduleParam->smoothingRadius; // smoothing radius parameter (float)
    const float h2 = h * h;
    const float h9 = pow(h, 9);

    const float Poly6 = (315.0f / (64.0f * M_PI * h9));

    __global ParticleData *particleA = &dataParticle[g_id];
    __global ParticleDataSPH *sphA = &dataSPH[particleA->index];

    sphA->force1 = 0; // we will compute this (float3)

    // Precomputed part of Spiky kernel:
    const float h6 = pow(h, 6);
    //  again, it should be coming from constant buffer
    const float Spiky_constant = (-45 / (M_PI * h6));

    barrier(CLK_GLOBAL_MEM_FENCE);

    uint global_id = get_global_id(0);
    uint global_size = get_global_size(0);
    uint local_id = get_local_id(0);
    uint local_size = get_local_size(0);

    /*printf("work_dim[%3u], global_id[%3u/%3u], local_id[%3u/%3u] groups_id[%3u/%3u]\n\
               g_id[%i] Pposition[%f|%f|%f Pindex[%i]]\n",
               
               work_dim,
           global_id, global_size,
           local_id, local_size,
            group_id, n_groups,
               get_global_id(0), particleA->position.x, particleA->position.y, particleA->position.z, particleA->index);
               */
    barrier(CLK_GLOBAL_MEM_FENCE);

    for (uint tile = 0; tile < n_groups; ++tile) {
        uint offset = tile * l_size;
        sharedSPH[l_id] = dataSPH[offset + l_id];
        barrier(CLK_LOCAL_MEM_FENCE);

        for (uint i = 0; i < l_size; ++i) {
            ParticleDataSPH sphB = sharedSPH[i];

            const float3 diff = sphA->position - sphB.position;
            const float r2 = dot(diff, diff);
            const float r = sqrt(r2);

            if (r > 0 && r < h) // **avoid division by zero!
            {
                const float3 rNorm = diff / r; // same as normalize(diff)
                const float W = Spiky_constant * pow(h - r, 2);

                sphA->force1 += (sphB.mass / sphA->mass) *
                                ((sphA->pressure + sphB.pressure) /
                                 (2 * sphA->density * sphB.density)) *
                                W * rNorm;
            }
        }

        barrier(CLK_LOCAL_MEM_FENCE);
    }

    sphA->force1 *= -1;
}

void __kernel SPH_UpdateViscosity(__global ParticleData *dataParticle,
                                  __global ParticleDataSPH *dataSPH,
                                  __global ModuleParamSPH *moduleParam,
                                  __local ParticleDataSPH *sharedSPH,
                                  float3 attractorPoint) {
    uint work_dim = get_work_dim();

    uint g_id = (uint)get_global_id(0);
    uint l_id = (uint)get_local_id(0);
    uint l_size = (uint)get_local_size(0);

    uint group_id = (uint)get_group_id(0);
    uint n_groups = (uint)get_num_groups(0);

    // PARAMS
    const float K = moduleParam->pressure;    //pressure constant parameter
    const float p0 = moduleParam->densityRef; //reference density param

    const float h = moduleParam->smoothingRadius; // smoothing radius parameter (float)
    const float h2 = h * h;
    const float h9 = pow(h, 9);

    const float Poly6 = (315.0f / (64.0f * M_PI * h9));

    __global ParticleData *particleA = &dataParticle[g_id];
    __global ParticleDataSPH *sphA = &dataSPH[particleA->index];

    sphA->force2 = 0; // we will compute this (float3)

    // Precomputed part of Spiky kernel:
    const float h6 = pow(h, 6);
    const float h3 = pow(h, 3);
    //  again, it should be coming from constant buffer
    const float Spiky_constant = (-45.f / (M_PI * h6));

    const float e = moduleParam->viscosity; // e = viscosity constant (float) (default = 0.018f)

    barrier(CLK_GLOBAL_MEM_FENCE);

    for (uint tile = 0; tile < n_groups; ++tile) {
        uint offset = tile * l_size;
        sharedSPH[l_id] = dataSPH[offset + l_id];
        barrier(CLK_LOCAL_MEM_FENCE);

        for (uint i = 0; i < l_size; ++i) {
            ParticleDataSPH sphB = sharedSPH[i];

            const float3 diff = sphA->position - sphB.position;
            const float r2 = dot(diff, diff);
            const float r = sqrt(r2);

            if (r > 0 && r < h) // **avoid division by zero!
            {
                const float3 rNorm = diff / r;
                const float r3 = r2 * r;
                const float W = -(r3 / (2 * h3)) + (r2 / h2) +
                                (h / (2 * r)) - 1;

                sphA->force2 += (sphB.mass / sphA->mass) *
                                (1.0f / sphB.density) *
                                (sphB.velocity - sphA->velocity) *
                                W * rNorm;
            }
        }

        barrier(CLK_LOCAL_MEM_FENCE);
    }

    sphA->force2 *= e;

    barrier(CLK_GLOBAL_MEM_FENCE);

    const float deltaTime = 0.016f; // fixed step
    const float3 G = (float3)(0.f, -9.8f, 0.f);
    float3 forceAttract = attractorPoint - sphA->position;
    //float3 ab = normalize(forceAttract) * clamp((20.f - length(forceAttract)), 0.f, 20.f) * 10.f;
    float3 ab = 0.f;
    sphA->velocity += deltaTime * ((sphA->force1 + sphA->force2) / sphA->density + G + ab);

    sphA->position += deltaTime * sphA->velocity;
    sphA->force1 = 0;
    sphA->force2 = 0;

    float3 mid = (float3)(60.0f, 60.0f, 60.0f);
    float3 bounds = (float3)(10.f, 10.f, 10.f);
    float3 min = mid - bounds;
    float3 max = mid + bounds;

    sphA->velocity *= 0.98f;
    float elastic = 0.8;

    float dist = 0.f;
    float Sradius = 5.f;
    float3 Sposition = mid;

    if (sdSphere(Sposition, Sradius, sphA->position, &dist)) {
        sphA->position = Sposition + normalize(sphA->position - Sposition) * Sradius;
        sphA->velocity = normalize(sphA->position - Sposition) * length(sphA->velocity);
    }
    if (sphA->position.x < min.x || sphA->position.x > max.x) {
        sphA->position.x = clamp(sphA->position.x, min.x, max.x);
        sphA->velocity.x *= -elastic;
    }
    if (sphA->position.y < min.y || sphA->position.y > max.y) {
        sphA->position.y = clamp(sphA->position.y, min.y, max.y);
        sphA->velocity.y *= -elastic;
    }
    if (sphA->position.z < min.z || sphA->position.z > max.z) {
        sphA->position.z = clamp(sphA->position.z, min.z, max.z);
        sphA->velocity.z *= -elastic;
    }

    particleA->position = sphA->position;
    particleA->velocity = sphA->velocity;
}
