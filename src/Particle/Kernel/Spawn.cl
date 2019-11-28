#include "NTL.hl"

__kernel void spawnParticle(
                        __global ParticleData *data,
                        __global int *arrayParticlesAlive,
                        __global int *arrayParticlesAlive2,
                        __global int *arrayParticlesDeath,
                        __global int *arrayParticlesLengthSub,

                        __global ModuleSpawnParams *moduleParams,

                        float3 particleSystemPosition,
                        int seed) {




    size_t id = get_global_id(0);
    int res = removeDeath(
                      arrayParticlesDeath,
                      arrayParticlesLengthSub,
                      id
                  );
    addAlive2222222ToArray(arrayParticlesAlive2, arrayParticlesLengthSub, res);
    __global ParticleData *particle = &data[res];

    resetParticleData(particle);

    particle->color.w = 1.f;
    particle->color.x = 1.f;
    particle->color.y = 1.f;
    particle->color.z = 1.f;

    particle->position.x += particleSystemPosition.x;
    particle->position.y += particleSystemPosition.y;
    particle->position.z += particleSystemPosition.z;

    particle->isAlive = 1;
    particle->age = 0.f;
    particle->lifeTime = getRandomRangef(&moduleParams->startLifeTime, seed + id);
    particle->size = getRandomRangef(&moduleParams->startSize, seed + id);
    particle->rotate = getRandomRangef(&moduleParams->startRotation, seed + id);
}