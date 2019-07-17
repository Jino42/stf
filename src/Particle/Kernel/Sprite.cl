#include "ParticleData.hl"

float2 setSpriteOffset(unsigned int numberRows, int index) {
    int column = index % numberRows;
    int row = index / numberRows;
    float2 offset;
    offset.x = (float)column / numberRows;
    offset.y = (float)row / numberRows;
    return offset;
}

void updateSpriteCoordInfo(
                __global ParticleData *particle,
                __global ParticleSpriteData *spriteData,
                unsigned int numberRows) {
    float lifeFactor = fmod(particle->age / particle->lifeTime, 1.f);
    int stageCount = numberRows * numberRows;
    float atlasProgression = lifeFactor * stageCount;
    int index1 = (int)floor(atlasProgression);
    int index2 = index1 < stageCount -1 ? index1 + 1 : index1;
    spriteData->blend = fmod(atlasProgression, 1);
    spriteData->offset1 = setSpriteOffset(numberRows, index1);
    spriteData->offset2 = setSpriteOffset(numberRows, index2);
}

__kernel void sprite(__global ParticleData *data,
                    __global ParticleSpriteData *spriteData,
                    unsigned int numberRows) {
    size_t id = get_global_id(0);
    __global ParticleData *particle = &data[id];

    updateSpriteCoordInfo(particle, &spriteData[id], numberRows);
}

