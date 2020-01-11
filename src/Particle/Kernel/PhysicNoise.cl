#include "NTL.hl"

void kernel PhysicNoiseUpdate(__global ParticleDataPhysic *dataPhysic,
                              __global ModuleParamPhysicNoise *moduleParam) {
    uint id = get_global_id(0);
    __global ParticleDataPhysic *particlePhysic = &dataPhysic[id];

    float3 noise;
    float osef;

    osef = particlePhysic->velocity.x;
    noise.x = randomMinMax(id + (uint)(particlePhysic->velocity.x * 100.f) ^ *((uint *)&osef) + id * 4, 0.0f, 2.0f) - 1.f;
    osef = particlePhysic->velocity.y;
    noise.y = randomMinMax(id + (uint)(particlePhysic->velocity.y * 100.f) ^ *((uint *)&osef) + id * 2, 0.0f, 2.0f) - 1.f;
    osef = particlePhysic->velocity.z;
    noise.z = randomMinMax(id + (uint)(particlePhysic->velocity.z * 100.f) ^ *((uint *)&osef) + id, 0.0f, 2.0f) - 1.f;

    particlePhysic->velocity += normalize(noise) * .1f;
}