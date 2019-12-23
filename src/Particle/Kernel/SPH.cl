#include "NTL.hl"

typedef struct ALIGN sModuleParamSPH {
    float pressure;
    float densityRef;
    float smoothingRadius;
    float viscosity;
} ModuleParamSPH;

typedef struct ALIGN sParticleDataSPH {
    float3 position;
    float density;
    float pressure;
    float mass;
    float3 force1;
    float3 force2;
    float3 velocity;
} ParticleDataSPH;

void __kernel SPH_Init(__global ParticleDataSPH *dataSPH,
                       __global ModuleParamSPH *moduleParam) {
    __global ParticleDataSPH *sph = &dataSPH[get_global_id(0)];

    sph->density = 0.f;
    sph->pressure = 0.f;
    sph->mass = 1.f;
    sph->force1 = 0.0f;
    sph->force2 = 0.0f;
    sph->velocity = 0.0f;
}
/*
bool solveQuadratic(const float a, const float b, const float c, float *x0, float *x1) {
    float discr = b * b - 4 * a * c;
    if (discr < 0)
        return false;
    else if (discr == 0)
        *x0 = *x1 = -0.5 * b / a;
    else {
        float q = (b > 0) ? -0.5 * (b + sqrt(discr)) : -0.5 * (b - sqrt(discr));
        *x0 = q / a;
        *x1 = c / q;
    }
    if (*x0 > *x1) {
        float tmp = *x1;
        *x1 = *x0;
        *x0 = tmp;
    }

    return true;
}

bool inter_sphere(float3 center, float radius, float3 dir, float3 origin, float3 *t) {
    float t0, t1; // solutions for t if the ray intersects
        // analytic solution
    float3 L = origin - center;
    float a = dot(dir, dir);
    float b = 2 * dot(dir, L);
    float c = dot(L, L) - radius * radius;
    if (!solveQuadratic(a, b, c, &t0, &t1))
        return false;
    if (t0 > t1) {
        float tmp = t0;
        t0 = t1;
        t1 = tmp;
    }

    if (t0 < 0) {
        t0 = t1; // if t0 is negative, let's use t1 instead
        if (t0 < 0)
            return false; // both t0 and t1 are negative
    }

    *t = t0;
    return true;
}
*/

bool sdSphere(float3 centerOfSphere, float radius, float3 checkPosition, float *t) {
    float t0 = length(centerOfSphere - checkPosition);
    *t = t0 - radius;
    return (t0 < radius);
}

void __kernel SPH_UpdateDensity(__global ParticleData *dataParticle,
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

    float3 mid = (float3)(82.5f, 258.f, 200.f);
    float3 bounds = (float3)(10.f, 10.f, 10.f);
    float3 min = mid - bounds;
    float3 max = mid + bounds;

    sphA->velocity *= 0.98f;
    float elastic = 0.8;

    float dist = 0.f;
    float Sradius = 5.f;
    float3 Sposition = (float3)(82.5f, 258.f, 200.f);
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