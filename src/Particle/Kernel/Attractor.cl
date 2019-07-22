#include "ParticleData.hl"
#include "Tools.hl"

void kernel attractor(__global ParticleData *data,
                        unsigned int flag,
                        float3 cameraPosition,
                        float deltaTime) {
    size_t id = get_global_id(0);
    __global ParticleData *particle = &data[id];

    float3 position;
    position.x = particle->position.x;
    position.y = particle->position.y;
    position.z = particle->position.z;

    float3 dist = cameraPosition - position;

    float3 acceleration;
    acceleration.x = 0.f;
    acceleration.y = 0.f;
    acceleration.z = 0.f;


    //Centered Particle
    if (particle->velocity.x > 0.1f)
        particle->velocity.x *= 0.95f;
    if (particle->velocity.y > 0.1f)
        particle->velocity.y *= 0.95f;
    if (particle->velocity.z > 0.1f)
        particle->velocity.z *= 0.95f;

    if (fabs(dist.x) > 0.001f && fabs(dist.y) > 0.001f && fabs(dist.z) > 0.001f)
    {
        acceleration = normalize(dist) * 100;
        particle->velocity.x += acceleration.x;
        particle->velocity.y += acceleration.y;
        particle->velocity.z += acceleration.z;
    }

    particle->position.x += particle->velocity.x * deltaTime;
    particle->position.y += particle->velocity.y * deltaTime;
    particle->position.z += particle->velocity.z * deltaTime;


    //Plane Collision
    //if (particle->position.x > 20.f)
    //    particle->position.x  = 20.f;
    //if (particle->position.y > 20.f)
    //    particle->position.y  = 20.f;
    //if (particle->position.z > 20.f)
    //    particle->position.z  = 20.f;
}
