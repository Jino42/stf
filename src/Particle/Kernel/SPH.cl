#include "NTL.hl"

typedef struct ALIGN sParticleDataSPH {
    float3	position;
    float	density;
    float	pressure;
    float	mass;
    float3	force1;
    float3	force2;
    float3	velocity;
}   ParticleDataSPH;

void __kernel SPH_Init(__global ParticleDataSPH *dataSPH) {
    __global ParticleDataSPH *sph = &dataSPH[get_global_id(0)];

    sph->density = 0.f;
    sph->pressure = 0.f;
    sph->mass = 1.f;
    sph->force1 = 0.0f;
    sph->force2 = 0.0f;
    sph->velocity = 0.0f;
}

void __kernel SPH_UpdateDensity( __global ParticleData *dataParticle,
                        __global ParticleDataSPH *dataSPH,
                        __local ParticleDataSPH  *sharedSPH
                        ) {
    uint work_dim = get_work_dim();

    uint g_id = (uint) get_global_id(0);
    uint l_id = (uint) get_local_id(0);
    uint l_size = (uint) get_local_size(0);

    uint group_id = (uint) get_group_id(0);
    uint n_groups = (uint) get_num_groups(0);

    // PARAMS
    const float K = 250.f; //pressure constant parameter
    const float p0 = 1.f; //reference density param

    const float h = 1.f; // smoothing radius parameter (float)
    const float h2 = h * h;
    const float h9 = pow(h, 9);

    const float Poly6 = (315.0f / (64.0f * M_PI * h9));

    __global ParticleData *particleA = &dataParticle[g_id];
    __global ParticleDataSPH *sphA = &dataSPH[particleA->index];


    sphA->position = particleA->position;
    sphA->density = 0;

    barrier(CLK_GLOBAL_MEM_FENCE);

    for (uint tile = 0; tile < n_groups; ++tile)
    {
        uint offset = tile * l_size;
        sharedSPH[l_id] = dataSPH[offset + l_id];
        barrier(CLK_LOCAL_MEM_FENCE);

        for(uint i = 0; i < l_size; ++i)
        {
            ParticleDataSPH sphB = sharedSPH[i];

            const float3 diff = sphA->position - sphB.position;
            const float r2 = dot(diff, diff);
            if(r2 < h2) // h2 = h*h
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



    barrier(CLK_GLOBAL_MEM_FENCE);
    particleA->position = sphA->position;
}

void __kernel SPH_UpdatePressure( __global ParticleData *dataParticle,
                        __global ParticleDataSPH *dataSPH,
                        __local ParticleDataSPH  *sharedSPH
                        ) {
    uint work_dim = get_work_dim();

    uint g_id = (uint) get_global_id(0);
    uint l_id = (uint) get_local_id(0);
    uint l_size = (uint) get_local_size(0);

    uint group_id = (uint) get_group_id(0);
    uint n_groups = (uint) get_num_groups(0);

    // PARAMS
    const float K = 250.f; //pressure constant parameter
    const float p0 = 1.f; //reference density param

    const float h = 1.f; // smoothing radius parameter (float)
    const float h2 = h * h;
    const float h9 = pow(h, 9);

    const float Poly6 = (315.0f / (64.0f * M_PI * h9));

    __global ParticleData *particleA = &dataParticle[g_id];
    __global ParticleDataSPH *sphA = &dataSPH[particleA->index];


    sphA->position = particleA->position;
    sphA->force1 = 0; // we will compute this (float3)

    // Precomputed part of Spiky kernel:
    const float h6 = pow(h, 6);
    //  again, it should be coming from constant buffer
    const float Spiky_constant = (-45 / (M_PI * h6));

    barrier(CLK_GLOBAL_MEM_FENCE);


    for (uint tile = 0; tile < n_groups; ++tile)
    {
        uint offset = tile * l_size;
        sharedSPH[l_id] = dataSPH[offset + l_id];
        barrier(CLK_LOCAL_MEM_FENCE);

        for(uint i = 0; i < l_size; ++i)
        {
            ParticleDataSPH sphB = sharedSPH[i];


            const float3 diff = sphA->position - sphB.position;
            const float r2 = dot(diff, diff);
            const float r = sqrt(r2);

            if(r > 0 && r < h) // **avoid division by zero!
            {
                const float3 rNorm = diff / r; // same as normalize(diff)
                const float W = Spiky_constant * pow(h - r, 2);

                sphA->force1 += (sphB.mass / sphA->mass) *
                    ((sphA->pressure + sphB.pressure) /
                    (2 * sphA->density * sphB.density)) * W * rNorm;
            }

        }

        barrier(CLK_LOCAL_MEM_FENCE);

    }

    sphA->force1 *= -1;



    barrier(CLK_GLOBAL_MEM_FENCE);
    particleA->position = sphA->position;
}

void __kernel SPH_UpdateViscosity( __global ParticleData *dataParticle,
                        __global ParticleDataSPH *dataSPH,
                        __local ParticleDataSPH  *sharedSPH
                        ) {
    uint work_dim = get_work_dim();

    uint g_id = (uint) get_global_id(0);
    uint l_id = (uint) get_local_id(0);
    uint l_size = (uint) get_local_size(0);

    uint group_id = (uint) get_group_id(0);
    uint n_groups = (uint) get_num_groups(0);

    // PARAMS
    const float K = 250.f; //pressure constant parameter
    const float p0 = 1.f; //reference density param

    const float h = 1.f; // smoothing radius parameter (float)
    const float h2 = h * h;
    const float h9 = pow(h, 9);

    const float Poly6 = (315.0f / (64.0f * M_PI * h9));

    __global ParticleData *particleA = &dataParticle[g_id];
    __global ParticleDataSPH *sphA = &dataSPH[particleA->index];


    sphA->position = particleA->position;
    sphA->force2 = 0; // we will compute this (float3)

    // Precomputed part of Spiky kernel:
    const float h6 = pow(h, 6);
    const float h3 = pow(h, 3);
    //  again, it should be coming from constant buffer
    const float Spiky_constant = (-45 / (M_PI * h6));

    const float e = 0.018f;// e = viscosity constant (float) (default = 0.018f)

    barrier(CLK_GLOBAL_MEM_FENCE);


    for (uint tile = 0; tile < n_groups; ++tile)
    {
        uint offset = tile * l_size;
        sharedSPH[l_id] = dataSPH[offset + l_id];
        barrier(CLK_LOCAL_MEM_FENCE);

        for(uint i = 0; i < l_size; ++i)
        {
            ParticleDataSPH sphB = sharedSPH[i];


            const float3 diff = sphA->position - sphB.position;
            const float r2 = dot(diff, diff);
            const float r = sqrt(r2);

            if(r > 0 && r < h) // **avoid division by zero!
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
    const float3 G = (float3)(0.f, -1.8f, 0.f);
    sphA->velocity += deltaTime * ((sphA->force1 + sphA->force2) / sphA->density + G);

    float3 mid = (float3)(82.5f, 258.f, 200.f);
    float3 min = mid - (float3)(20.f, 20.f, 20.f);
    float3 max = mid + (float3)(20.f, 20.f, 20.f);
    if (sphA->position.x < min.x || sphA->position.x > max.x) {
        sphA->velocity.x = clamp(sphA->velocity.x, min.x, max.x);
        sphA->velocity.x *= 0;
    }
    if (sphA->position.y < min.y || sphA->position.y > max.y) {
        sphA->velocity.y = clamp(sphA->velocity.y, min.y, max.y);
        sphA->velocity.y *= -0;
    }
    if (sphA->position.z < min.z || sphA->position.z > max.z) {
        sphA->velocity.z = clamp(sphA->velocity.z, min.z, max.z);
        sphA->velocity.z *= -0;
    }

    sphA->position += deltaTime * sphA->velocity;
    sphA->force1 = 0;
    sphA->force2 = 0;

    particleA->position = sphA->position;
}