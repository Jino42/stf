#include "NTL.hl"

void kernel sizeUpdate(__global ParticleData *data,
        __global ModuleSizeOverLifetimeParams *moduleParams) {
    size_t id = get_global_id(0);
    __global ParticleData *particle = &data[id];

    particle->size = moduleParams->size.min + moduleParams->size.max * (particle->age / particle->lifeTime);
}