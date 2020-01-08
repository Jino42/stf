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

float3 reflect(float3 d, float3 n) {
    return d - 2.f * dot(d, n) * n;;
}

bool sdSphere2(float3 centerOfSphere, float radius, float3 checkPosition, float *t) {
    float t0 = length(centerOfSphere - checkPosition);
    *t = t0 - radius;
    return *t < 0.0f;
}
bool sdPlane2(float3 position, float3 planePosition, float3 n, float *t) {
    *t = dot(planePosition - position, n.xyz);
    return *t < 0.0f;
}
bool sdBox2(float3 positionPoint, float3 positionBox, float3 sizeBox) {
    float3 position = positionBox - positionPoint;
    float3 q = fabs(position) - sizeBox;
    float t = length(maxf3(q, (float3)(0.0f, 0.0f, 0.0f))) + min(max(q.x, max(q.y, q.z)), 0.0f);

    return t < 0.0f;
}

bool sdTorus2(float3 positionPoint, float3 positionTorus, float2 pt,  float *t) {
    float3 p = positionTorus - positionPoint;

    p = rotate_float3(p, (float3)(0.0f, 0.0f, 1.0f));

    float2 q = (float2)(length(p.xz) - pt.x, p.y);
    *t =  length(q) - pt.y;
    return *t < 0.0f;
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

__kernel void SPH_Print(__global ParticleData *dataParticle,
                        __global ParticleDataSPH *dataSPH,
                        __global ModuleParamSPH *moduleParam,
                        __global unsigned int *dataCellIndex,
                        __global unsigned int *dataParticleIndex,
                        __global unsigned int *cellOffsetBuffer) {

    uint g_id = (uint)get_global_id(0);
    uint particleIndex = dataParticleIndex[g_id];
    __global ParticleData *particleA = &dataParticle[particleIndex];
    __global ParticleDataSPH *sphA = &dataSPH[particleA->index];

    int3 cellIndex = convert_int3(floor(particleA->position / moduleParam->smoothingRadius));

    printf("g_id[%2i] particleIndex[%2i] index->%2i dataCellIndex[%7i] | SPHposition[%3.2f %3.2f %3.2f] flat[%3i %3i %3i]\n",
           g_id, particleIndex, particleA->index, dataCellIndex[particleIndex],
           sphA->position.x, sphA->position.y, sphA->position.z,
           cellIndex.x, cellIndex.y, cellIndex.z);
}

void __kernel SPH_UpdateCellIndex(__global ParticleData *dataParticle,
                                  __global ParticleDataSPH *dataSPH,
                                  __global ModuleParamSPH *moduleParam,
                                  __global unsigned int *dataCellIndex,
                                  __global unsigned int *dataParticleIndex,
                                  __global unsigned int *cellOffsetBuffer) {
    uint g_id = (uint)get_global_id(0);

    dataParticleIndex[g_id] = g_id;

    uint particleIndex = dataParticleIndex[g_id];
    __global ParticleData *particleA = &dataParticle[particleIndex];
    __global ParticleDataSPH *sphA = &dataSPH[particleA->index];

    sphA->position = particleA->position;
    sphA->density = 0;

    int3 cellIndex = convert_int3(floor(particleA->position / moduleParam->smoothingRadius));

    uint flatCellIndex = GetFlatCellIndex(cellIndex);

    dataCellIndex[particleIndex] = flatCellIndex;
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
    const float h2 = moduleParam->h2;
    const float h9 = moduleParam->h9;

    const float Poly6 = moduleParam->Poly6;

    __global ParticleData *particleA = &dataParticle[dataParticleIndex[g_id]];

    dataSPH[particleA->index].flag = 0;
    if (dataParticleIndex[g_id] == focus) {
        dataSPH[particleA->index].flag = FLAG_FOCUS;
    }
    ParticleDataSPH sphA = dataSPH[particleA->index];
    barrier(CLK_GLOBAL_MEM_FENCE);

    int3 cellIndex = convert_int3(floor(particleA->position / moduleParam->smoothingRadius));

    for (int i = -1; i <= 1; ++i) {
        for (int j = -1; j <= 1; ++j) {
            for (int k = -1; k <= 1; ++k) {
                int3 neighborIndex = cellIndex + (int3)(i, j, k);

                uint flatNeighborIndex = GetFlatCellIndex(neighborIndex);

                // look up the offset to the cell:
                uint neighborIterator = cellOffsetBuffer[flatNeighborIndex];

                // iterate through particles in the neighbour cell (if iterator offset is valid)
                while (neighborIterator != 0xFFFFFFFF && neighborIterator < get_global_size(0)) {
                    uint particleIndexB = dataParticleIndex[neighborIterator];

                    if (dataCellIndex[particleIndexB] != flatNeighborIndex) {
                        break; // it means we stepped out of the neighbour cell list!
                    }

                    // Here you can load particleB and do the SPH evaluation logic
                    __global ParticleDataSPH *sphB = &dataSPH[particleIndexB];
                    if (dataParticleIndex[g_id] == focus)
                        sphB->flag = 2;

                    const float3 diff = sphA.position - sphB->position;
                    const float r2 = dot(diff, diff);
                    if (r2 < h2) // h2 = h*h
                    {
                        const float W = Poly6 * pow(h2 - r2, 3);
                        sphA.density += sphB->mass * W;
                    }

                    neighborIterator++;
                }
                barrier(CLK_GLOBAL_MEM_FENCE);
            }
        }
    }
    sphA.density = max(p0, sphA.density);
    sphA.pressure = K * (sphA.density - p0);

    barrier(CLK_GLOBAL_MEM_FENCE);
    int f = dataSPH[particleA->index].flag;
    barrier(CLK_GLOBAL_MEM_FENCE);

    dataSPH[particleA->index] = sphA;
    if (f && dataSPH[particleA->index].flag != 1)
        dataSPH[particleA->index].flag = f;
}

void __kernel SPH_UpdatePressure(__global ParticleData *dataParticle,
                                 __global ParticleDataSPH *dataSPH,
                                 __global ModuleParamSPH *moduleParam,
                                 __global unsigned int *dataCellIndex,
                                 __global unsigned int *dataParticleIndex,
                                 __global unsigned int *cellOffsetBuffer) {
    uint work_dim = get_work_dim();

    uint g_id = (uint)get_global_id(0);
    uint l_id = (uint)get_local_id(0);
    uint l_size = (uint)get_local_size(0);

    uint group_id = (uint)get_group_id(0);
    uint n_groups = (uint)get_num_groups(0);

    // PARAMS
    const float K = moduleParam->pressure;    //pressure constant parameter
    const float p0 = moduleParam->densityRef; //reference density param

    const float h = moduleParam->smoothingRadius; // smoothing radius parameter
    const float h2 = moduleParam->h2;
    const float h9 = moduleParam->h9;

    const float Poly6 = moduleParam->Poly6;

    __global ParticleData *particleA = &dataParticle[dataParticleIndex[g_id]];
    ParticleDataSPH sphA = dataSPH[particleA->index];

    sphA.force1 = 0;
    const float h6 = moduleParam->h6;
    const float Spiky_constant = moduleParam->Spiky;

    barrier(CLK_GLOBAL_MEM_FENCE);

    int3 cellIndex = convert_int3(floor(particleA->position / moduleParam->smoothingRadius));

    for (int i = -1; i <= 1; ++i) {
        for (int j = -1; j <= 1; ++j) {
            for (int k = -1; k <= 1; ++k) {
                int3 neighborIndex = cellIndex + (int3)(i, j, k);

                uint flatNeighborIndex = GetFlatCellIndex(neighborIndex);

                // look up the offset to the cell:
                uint neighborIterator = cellOffsetBuffer[flatNeighborIndex];

                // iterate through particles in the neighbour cell (if iterator offset is valid)
                while (neighborIterator != 0xFFFFFFFF && neighborIterator < get_global_size(0)) {
                    uint particleIndexB = dataParticleIndex[neighborIterator];

                    if (dataCellIndex[particleIndexB] != flatNeighborIndex) {
                        break; // it means we stepped out of the neighbour cell list!
                    }

                    // Here you can load particleB and do the SPH evaluation logic
                    __global ParticleDataSPH *sphB = &dataSPH[particleIndexB];

                    const float3 diff = sphA.position - sphB->position;
                    const float r2 = dot(diff, diff);
                    const float r = sqrt(r2);

                    if (r > 0 && r < h) // **avoid division by zero!
                    {
                        const float3 rNorm = diff / r; // same as normalize(diff)
                        const float W = Spiky_constant * pow(h - r, 2);

                        sphA.force1 += (sphB->mass / sphA.mass) *
                                       ((sphA.pressure + sphB->pressure) /
                                        (2.f * sphA.density * sphB->density)) *
                                       W * rNorm;
                    }

                    neighborIterator++;
                }
            }
        }
    }
    barrier(CLK_GLOBAL_MEM_FENCE);
    sphA.force1 *= -1;
    dataSPH[particleA->index] = sphA;
}

void __kernel SPH_UpdateViscosity(__global ParticleData *dataParticle,
                                  __global ParticleDataSPH *dataSPH,
                                  __global ModuleParamSPH *moduleParam,
                                  __global unsigned int *dataCellIndex,
                                  __global unsigned int *dataParticleIndex,
                                  __global unsigned int *cellOffsetBuffer,
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
    const float h2 = moduleParam->h2;
    const float h9 = moduleParam->h9;

    const float Poly6 = moduleParam->Poly6;

    __global ParticleData *particleA = &dataParticle[dataParticleIndex[g_id]];
    ParticleDataSPH sphA = dataSPH[particleA->index];
    sphA.force2 = 0;
    const float h6 = moduleParam->h6;
    const float h3 = moduleParam->h3;
    const float Spiky_constant = moduleParam->Spiky;
    const float e = moduleParam->viscosity;

    barrier(CLK_GLOBAL_MEM_FENCE);

    int3 cellIndex = convert_int3(floor(particleA->position / moduleParam->smoothingRadius));

    for (int i = -1; i <= 1; ++i) {
        for (int j = -1; j <= 1; ++j) {
            for (int k = -1; k <= 1; ++k) {
                int3 neighborIndex = cellIndex + (int3)(i, j, k);

                uint flatNeighborIndex = GetFlatCellIndex(neighborIndex);

                // look up the offset to the cell:
                uint neighborIterator = cellOffsetBuffer[flatNeighborIndex];

                // iterate through particles in the neighbour cell (if iterator offset is valid)
                while (neighborIterator != 0xFFFFFFFF && neighborIterator < get_global_size(0)) {
                    uint particleIndexB = dataParticleIndex[neighborIterator];

                    if (dataCellIndex[particleIndexB] != flatNeighborIndex) {
                        break; // it means we stepped out of the neighbour cell list!
                    }

                    // Here you can load particleB and do the SPH evaluation logic
                    __global ParticleDataSPH *sphB = &dataSPH[particleIndexB];

                    const float3 diff = sphA.position - sphB->position;
                    const float r2 = dot(diff, diff);
                    const float r = sqrt(r2);

                    if (r > 0 && r < h) // **avoid division by zero!
                    {
                        const float3 rNorm = diff / r;
                        const float r3 = r2 * r;
                        const float W = -(r3 / (2 * h3)) + (r2 / h2) +
                                        (h / (2 * r)) - 1;

                        sphA.force2 += (sphB->mass / sphA.mass) *
                                       (1.0f / sphB->density) *
                                       (sphB->velocity - sphA.velocity) *
                                       W * rNorm;
                    }

                    neighborIterator++;
                }
                barrier(CLK_GLOBAL_MEM_FENCE);
            }
        }
    }
    sphA.force2 *= e;
    dataSPH[particleA->index] = sphA;

    barrier(CLK_GLOBAL_MEM_FENCE);

    __global ParticleDataSPH *sph = &dataSPH[particleA->index];

    const float deltaTime = 0.016f; // fixed step
    const float3 G = (float3)(0.f, -9.8f, 0.f);
    float3 forceAttract = attractorPoint - sph->position;
    //float3 ab = normalize(forceAttract) * clamp((20.f - length(forceAttract)), 0.f, 20.f) * 10.f;
    float3 ab = 0.f;
    sph->velocity += deltaTime * ((sph->force1 + sph->force2) / sph->density + G + ab);

    sph->position += deltaTime * sph->velocity;

    float3 mid = (float3)(60.0f, 60.0f, 60.0f);
    float3 bounds = (float3)(10.f, 10.f, 10.f);
    float3 min = mid - bounds;
    float3 max = mid + bounds;

    sph->velocity *= 0.98f;
    float elastic = 1.f;

    float dist = 0.f;
    float Sradius = 5.f;
    float3 Sposition = mid;

    if (sdSphere2(Sposition, Sradius, sph->position, &dist)) {
        sph->position = Sposition + normalize(sph->position - Sposition) * Sradius;
        sph->velocity = normalize(sph->position - Sposition) * length(sph->velocity);
    }

    float3 n = normalize((float3)(0.2f, -1.f, 0.f));
    if (sdPlane2(sph->position, (float3)(60.0f, 40.0f, 60.0f), n, &dist)) {
        sph->position = sph->position + n * dist;
        sph->velocity = reflect(sph->velocity, -n);
    }

    n = normalize((float3)(-1.0f, 0.f, 0.f));
    if (sdPlane2(sph->position, (float3)(10.0f, 40.0f, 60.0f), n, &dist)) {
        sph->position = sph->position + n * dist;
        sph->velocity = reflect(sph->velocity, -n);
    }

    n = normalize((float3)(0.0f, 0.f, -1.f));
    if (sdPlane2(sph->position, (float3)(60.0f, 40.0f, 40.0f), n, &dist)) {
        sph->position = sph->position + n * dist;
        sph->velocity = reflect(sph->velocity, -n);
    }

        n = normalize((float3)(0.0f, 0.f, 1.f));
    if (sdPlane2(sph->position, (float3)(60.0f, 40.0f, 80.0f), n, &dist)) {
        sph->position = sph->position + n * dist;
        sph->velocity = reflect(sph->velocity, -n);
    }


    float2 torusParam = (float2)(7.0f, 3.0f);
    float3 torusPosition = (float3)(float3)(40.0f, 40.0f, 60.0f);
    if (sdTorus2(sph->position, torusPosition, torusParam, &dist)) {
        float2 e = (float2)(1.0f, -1.0f) * 0.5773f * 0.0005f;
        float3 norm = normalize(e.xyy * sdTorus(sph->position + e.xyy, torusPosition, torusParam) +
                                e.yyx * sdTorus(sph->position + e.yyx, torusPosition, torusParam) +
                                e.yxy * sdTorus(sph->position + e.yxy, torusPosition, torusParam) +
                                e.xxx * sdTorus(sph->position + e.xxx, torusPosition, torusParam));

        sph->position = sph->position + -norm * dist;
        sph->velocity = reflect(sph->velocity, norm);
    }

    /*
    dot(planePosition - position, n.xyz);
    tmp = sdBox(cameraPosition + dir * t, (float3)(40.0f, 40.0f, 60.0f), (float3)(10.f, 1.f, 10.f));
    if (tmp < d) {
        d = tmp;
        fcolor = (float4)(1.0f, 1.f, 0.5f, 1.0f);
    }

    tmp = sdBox(cameraPosition + dir * t, (float3)(30.0f, 40.0f, 60.0f), (float3)(1.f, 3.f, 10.f));
    if (tmp < d) {
        d = tmp;
        fcolor = (float4)(1.0f, 0.5f, 0.5f, 1.0f);
    }
    tmp = sdBox(cameraPosition + dir * t, (float3)(40.0f, 40.0f, 50.0f), (float3)(100.f, 3.f, 1.f));
    if (tmp < d) {
        d = tmp;
        fcolor = (float4)(1.0f, 0.5f, 0.5f, 1.0f);
    }
    tmp = sdBox(cameraPosition + dir * t, (float3)(40.0f, 40.0f, 70.0f), (float3)(100.f, 3.f, 1.f));
    if (tmp < d) {
        d = tmp;
        fcolor = (float4)(1.0f, 0.5f, 0.5f, 1.0f);
    }
    */
    /*
    if (sph->position.x < min.x || sph->position.x > max.x) {
        sph->position.x = clamp(sph->position.x, min.x, max.x);
        sph->velocity.x *= -elastic;
    }
    if (sph->position.y < min.y || sph->position.y > max.y) {
        sph->position.y = clamp(sph->position.y, min.y, max.y);
        sph->velocity.y *= -elastic;
    }
    if (sph->position.z < min.z || sph->position.z > max.z) {
        sph->position.z = clamp(sph->position.z, min.z, max.z);
        sph->velocity.z *= -elastic;
    }
    */
    particleA->position = sph->position;
    particleA->velocity = sph->velocity;
}
