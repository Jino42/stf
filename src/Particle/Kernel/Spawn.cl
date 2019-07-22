#include "ParticleData.hl"
#include "ModuleStruct.hl"


void kernel spawnParticle(__global ModuleSpawnParams *moduleParams,
                            __global ParticleData *data,
                            float3 particleSystemPosition,
                            int seed) {
    size_t id = get_global_id(0);
    __global ParticleData *particle = &data[id];

    resetParticleData(particle);

    particle->color.w = 1.f;
    particle->color.x = 1.f;
    particle->color.y = 1.f;
    particle->color.z = 1.f;

    particle->position.x += particleSystemPosition.x;
    particle->position.y += particleSystemPosition.y;
    particle->position.z += particleSystemPosition.z;

    particle->age = 0.f;
    particle->lifeTime = getRandomRangef(&moduleParams->startLifeTime, seed + id);
    particle->size = getRandomRangef(&moduleParams->startSize, seed + id);
    particle->rotate = getRandomRangef(&moduleParams->startRotation, seed + id);
}