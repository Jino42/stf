#include "NTL.hl"

void kernel spawnMovementRandom(__global ParticleData *dataParticle,
                                __global int *arrayParticlesAlive2,
                                __global ParticleDataMovement *dataMovement,
                                int seed) {
    size_t id = arrayParticlesAlive2[get_global_id(0)];


    __global ParticleDataMovement *movement = &dataMovement[dataParticle[id].index];

    movement->acceleration = (float3)(0.f, 0.f, 0.f);
    movement->velocity = (float3)(0.f, 0.f, 0.f);
    movement->masse = 10.f;

    float3 force = normalize((float3) (randomMinMax(seed + id, -1.f, 1.f), randomMinMax((seed >> 1) + id, -1.f, 1.f), randomMinMax((seed << 2) + id, -1.f, 1.f))) * 3.f;
    movement->acceleration += force / movement->masse;
    force = normalize((float3) (0.f, -0.1f * movement->masse, 0.f));
    movement->acceleration += force / movement->masse;
}

void kernel movement(__global ParticleData *dataParticle,
        __global ParticleDataMovement *dataMovement,
        float3 attractor) {
    size_t id = get_global_id(0);
    __global ParticleData *particle = &dataParticle[id];
    __global ParticleDataMovement *movement = &dataMovement[particle->index];

    //FRICTION
    float3 friction = normalize(movement->velocity) * -1;
    float frictionCoef = 0.01f;
    float magnitudeNormal = 1.f;

    friction *= (frictionCoef * magnitudeNormal);
    movement->acceleration += friction / movement->masse;


    //ATTRACTOR
    float3 dist = attractor - particle->position;
    float distForce = clamp(length(dist), 5.f, 30.f);
    float m = (0.4f * 1.f * 10.f) / (distForce * distForce);
    dist = normalize(dist) * m;
    movement->acceleration += dist / movement->masse;


    movement->velocity += movement->acceleration;
    particle->position += movement->velocity;

    movement->acceleration = (float3)(0.f, 0.f, 0.f);

    //if (fabs(particle->position.x) > 30)
    //    movement->velocity.x *= -1;
    //if (fabs(particle->position.y) > 30)
    //        movement->velocity.y *= -1;
    //if (fabs(particle->position.z) > 30)
    //        movement->velocity.z *= -1;
}