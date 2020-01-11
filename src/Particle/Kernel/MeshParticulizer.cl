#include "NTL.hl"

void kernel initMeshParticulizer(__global ParticleData *dataParticle,
                                 __global EmitterParam *emitterParam,
                                 __global ParticleDataMeshParticulizer *particleParticulizerData
                                 ) {
    size_t id = get_global_id(0);
    __global ParticleData *particle = &dataParticle[id];

    particle->position = particleParticulizerData[id].position;
    particle->position += emitterParam->position;
}

void kernel spawnMeshParticulizer(__global ParticleData *dataParticle,
                                  __global int *arrayParticlesAlive2,
                                  __global ParticleDataMeshParticulizer *particleParticulizerData) {
    size_t id = arrayParticlesAlive2[get_global_id(0)];
    __global ParticleData *particle = &dataParticle[id];

    particle->position = particleParticulizerData[id].position;
}