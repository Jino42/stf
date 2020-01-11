#include "NTL.hl"

void kernel attractor(__global ParticleData *data,
                      __global ParticleDataPhysic *dataPhysic,
                      unsigned int flag,
                      float3 cameraPosition,
                      float deltaTime) {
    uint id = get_global_id(0);
    __global ParticleData *particle = &data[id];
    __global ParticleDataPhysic *particlePhysic = &dataPhysic[id];


    float3 dist = cameraPosition - particle->position;

    float3 acceleration;

    if (fabs(dist.x) > 0.001f && fabs(dist.y) > 0.001f && fabs(dist.z) > 0.001f) {
        acceleration = normalize(dist) / particlePhysic->mass;
        particlePhysic->velocity += acceleration;
    }
}