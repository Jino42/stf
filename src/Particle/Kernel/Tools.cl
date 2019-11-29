#include "NTL.hl"

bool particleIsActive(__global ParticleData const *particle)
{
	return !particle->lifeTime || particle->age < particle->lifeTime;
}

void resetParticleData(__global ParticleData *particle)
{
	particle->position = (float3)(0.f, 0.f, 0.f);
	particle->velocity = (float3)(0.f, 0.f, 0.f);
	particle->color = (float4)(0.f, 0.f, 0.f, 1.f);
	particle->rotate = 0.f;
	particle->size = 1.f;
	particle->isAlive = 0;
	particle->age = 0.f;
	//particle->lifeTime = 5.f;
}

void killParticle(__global ParticleData *particle)
{
    particle->isAlive = 0;
    particle->lifeTime = 5.f;
    particle->age = particle->lifeTime + 1.f;
}

float random(uint seed)
{
    //seed = (seed * 0x5DEECE66DL + 0xBL) & ((1L << 48) - 1);
    //return ((float)(seed >> 16) / (float)SEED_MAX);

    seed = (seed ^ 61) ^ (seed >> 16);
    seed *= 9;
    seed = seed ^ (seed >> 4);
    seed *= 0x27d4eb2d;
    seed = seed ^ (seed >> 15);
    return -1.f + ((float)(seed % SEED_MAX) / (float)SEED_MAX) * 2.f;
}

float randomMinMax(uint seed, float min, float max)
{
    seed = (seed ^ 61) ^ (seed >> 16);
    seed *= 9;
    seed = seed ^ (seed >> 4);
    seed *= 0x27d4eb2d;
    seed = seed ^ (seed >> 15);
    return min + ((float)(seed % SEED_MAX) / (float)SEED_MAX) * (max - min);
}

float getRandomRangef(__global Rangef *range, int seed)
{
    if (range->isRange)
        return randomMinMax(seed, range->min, range->max);
    return range->max;
}