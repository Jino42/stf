#include "NTL.hl"

void kernel updateMoveToTarget(__global ParticleData *dataParticle,
        __global ModuleTargetData *targetDataParticles,
        float deltaTime) {

    size_t id = get_global_id(0);
    __global ParticleData *particle = &dataParticle[id];
    __global ModuleTargetData *target = &targetDataParticles[particle->index];


    float speed = 10.f * deltaTime;

    float3 dir = normalize(target->target - particle->position);

    particle->position += dir * speed;
}