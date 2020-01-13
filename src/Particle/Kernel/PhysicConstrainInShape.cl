#include "NTL.hl"

bool sd_AABB(float3 positionPoint, float3 positionBox, float3 sizeBox, float *t) {
    float3 position = positionBox - positionPoint;
    float3 q = fabs(position) - sizeBox / 2.f;
    *t = length(maxf3(q, (float3)(0.0f, 0.0f, 0.0f))) + min(max(q.x, max(q.y, q.z)), 0.0f);

    return *t < 0.0f;
}
float sd_AABBDist(float3 positionPoint, float3 positionBox, float3 sizeBox) {
    float3 position = positionBox - positionPoint;
    float3 q = fabs(position) - sizeBox / 2.f;
    return length(maxf3(q, (float3)(0.0f, 0.0f, 0.0f))) + min(max(q.x, max(q.y, q.z)), 0.0f);
}
/*
bool sdSphere3(float3 centerOfSphere, float radius, float3 checkPosition, float *t) {
    float t0 = length(centerOfSphere - checkPosition);
    *t = t0 - radius;
    return *t < 0.0f;
}
sph->position = Sposition + normalize(sph->position - Sposition) * Sradius;
sph->velocity = normalize(sph->position - Sposition) * length(sph->velocity);
*/
float3 reflect(float3 d, float3 n) {
    return d - 2.f * dot(d, n) * n;
}

void kernel PhysicConstrainInShapeUpdate(__global ParticleData *data,
                                         __global ParticleDataPhysic *dataPhysic,
                                         __global Shape const *shape) {
    uint id = get_global_id(0);
    __global ParticleData *particle = &data[id];
    __global ParticleDataPhysic *particlePhysic = &dataPhysic[id];

    float t;

    if (!id) {
        printf("Shape [%f][%f][%f]\n",
               shape->position.x, shape->position.y, shape->position.z);
    }
    if (((shape->flag & kMaskShape) & kAABB) == kAABB) {
        __global AABB const *aabb = (__global AABB const *)shape;
        if (!id) {
            printf("Shape [%f][%f][%f]\n",
                   shape->position.x, shape->position.y, shape->position.z);
            printf("Box [%f][%f][%f] [%f][%f][%f] \n",
                   aabb->position.x, aabb->position.y, aabb->position.z,
                   aabb->size.x, aabb->size.y, aabb->size.z);
        }
        if (!sd_AABB(particle->position, aabb->position, aabb->size, &t)) {
            if (!id)
                printf("Yes %f\n", t);

            float2 e = (float2)(1.0f, -1.0f) * 0.5773f * 0.0005f;
            float3 norm = normalize(e.xyy * sd_AABBDist(particle->position + e.xyy, aabb->position, aabb->size) +
                                    e.yyx * sd_AABBDist(particle->position + e.yyx, aabb->position, aabb->size) +
                                    e.yxy * sd_AABBDist(particle->position + e.yxy, aabb->position, aabb->size) +
                                    e.xxx * sd_AABBDist(particle->position + e.xxx, aabb->position, aabb->size));

            particle->position = particle->position + -norm * t;
            particlePhysic->velocity = reflect(particlePhysic->velocity, norm);
        }
    }
}