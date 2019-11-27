#include "NTL.hl"

__kernel void getNbParticleActiveSafe(
        __global ParticleData const *data,
        __global float *bufferDist,
        __global int *nbParticleActive,
int nbParticleMax)
{
    int id = get_global_id(0);
    __global ParticleData const *particle = &data[id];

    int s = 0;
    int c = nbParticleMax / 2;

    int left = 0;
    int right = nbParticleMax;
    while (left <= right)
    {
        int m = floor((float)(left + right) / 2.0f);
        if (particleIsActive(&data[m]))
        left = m + 1;
        else
        right = m - 1;
    }
    for (int i = left + 1; i < nbParticleMax; i++)
    {
        if (particleIsActive(&data[i]))
        left++;
    }

    *nbParticleActive = left;
}

//__kernel void getNbParticleActiveSafe(
__kernel void getNbParticleActiveSafe2(
        __global ParticleData const *data,
__global float *bufferDist,
        __global int *nbParticleActive,
int nbParticleMax)
{
int id = get_global_id(0);
__global ParticleData const *particle = &data[id];

    int i = 0;
    int s = 0;
    while (i < nbParticleMax)
    {
        if (particleIsActive(&data[i]))
            s++;
        i++;
    }
    *nbParticleActive = s;
}

__kernel void calculateDistanceBetweenParticleAndCamera(
    __global ParticleData const *data,
    __global float *bufferDist,
    float3 cameraPosition)
{
    int id = get_global_id(0);
    __global ParticleData const *particle = &data[id];

    bufferDist[id] = fabs((particle->position.x - cameraPosition.x) + (particle->position.y - cameraPosition.y) + (particle->position.z - cameraPosition.z));
    if (!particleIsActive(particle))
        bufferDist[id] = -10.f;
}

__kernel void ParallelSelection(
    __global ParticleData *data,
    __global ParticleSpriteData *spriteData,
    __global float *bufferDist,
    float3 cameraPosition)
{
    int i = get_global_id(0); // current thread
    int n = get_global_size(0); // input size
    ParticleData iDataParticle = data[i];
    ParticleSpriteData iDataSprite = spriteData[i];

    float iKey = bufferDist[i];
    int pos = 0;
    for (int j = 0; j < n; j++)
    {
        if ((bufferDist[j] > iKey) || (bufferDist[j] == iKey && j < i))
            pos += 1;
    }
    data[pos] = iDataParticle;
    spriteData[pos] = iDataSprite;
}
