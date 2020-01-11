#include "NTL.hl"

__kernel void ResolvePhysicInitialisation(__global ParticleDataPhysic *dataPhysic) {
    uint id = get_global_id(0);
    __global ParticleDataPhysic *physic = &dataPhysic[id];

    physic->velocity = (float3)(0.0f, 0.0f, 0.0f);
    physic->mass = 1.0f;
}

__kernel void ResolvePhysicUpdate(__global ParticleData *data,
                                   __global ParticleDataPhysic *dataPhysic,
                                   float deltaTime) {
    uint id = get_global_id(0);
    __global ParticleData *particle = &data[id];
    __global ParticleDataPhysic *particlePhysic = &dataPhysic[id];

    
    /*
        //FRICTION
    float3 friction = normalize(particlePhysic->velocity) * -1;
    float frictionCoef = 0.01f;
    float magnitudeNormal = 1.f;

    friction *= (frictionCoef * magnitudeNormal);
    particlePhysic->velocity += friction / particlePhysic->mass;
    */
    
    //Centered Particle
    if (particlePhysic->velocity.x > 0.1f)
        particlePhysic->velocity.x *= 0.997f;
    if (particlePhysic->velocity.y > 0.1f)
        particlePhysic->velocity.y *= 0.997f;
    if (particlePhysic->velocity.z > 0.1f)
        particlePhysic->velocity.z *= 0.997f;
 
    particle->position += particlePhysic->velocity * deltaTime;

    particle->velocity = particlePhysic->velocity;
}