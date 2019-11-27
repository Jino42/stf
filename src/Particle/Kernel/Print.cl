#include "NTL.hl"

__kernel void PrintParticle(
    __global ParticleData const *data)
{
    int id = get_global_id(0);
    __global ParticleData *particle = &data[id];
    if (!id)
    {
        for(int i = 0; i < get_global_size(0); i++)
        {
            particle = &data[i];
            printf("%i index[%i] life[%f/%f]\n", i, particle->index, particle->age, particle->lifeTime);
        }
    }

}