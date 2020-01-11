#include "NTL.hl"

void kernel updateMoveToTarget(__global ParticleData *dataParticle,
        __global ParticleDataTarget *targetDataParticles,
        float deltaTime) {

    size_t id = get_global_id(0);
    __global ParticleData *particle = &dataParticle[id];
    __global ParticleDataTarget *target = &targetDataParticles[particle->index];


    float speed = 10.f * deltaTime;

    float3 dir = target->target - particle->position;

    if (length(dir) > 0.01f)
        particle->position += normalize(dir) * speed;
}