#include "NTL.hl"

void kernel initMeshParticulizer(__global ParticleData *dataParticle,
                                __global ModuleMeshParticulizerData *moduleData,
                                float3 particleSystemPosition
                                ) {
    size_t id = get_global_id(0);
    __global ParticleData *particle = &dataParticle[id];

    particle->position = moduleData[id].position;
    particle->position.x += particleSystemPosition.x;
    particle->position.y += particleSystemPosition.y;
    particle->position.z += particleSystemPosition.z;
}

void kernel spawnMeshParticulizer(__global ParticleData *dataParticle,
                                __global int *arrayParticlesAlive2,
                                __global ModuleMeshParticulizerData *moduleData) {
    size_t id = arrayParticlesAlive2[get_global_id(0)];
    __global ParticleData *particle = &dataParticle[id];

    particle->position = moduleData[id].position;
}