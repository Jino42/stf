#include "NTL.hl"

__kernel void PrintParticle(
    __global ParticleData const *data)
{
    int id = get_global_id(0);
    __global ParticleData const *particle = &data[id];
    if (!id)
    {
        for(int i = 0; i < get_global_size(0); i++)
        {
            particle = &data[i];
            printf("%i index[%i] life[%f/%f]\n", i, particle->index, particle->age, particle->lifeTime);
        }
    }

}

__kernel void PrintStructSize() {
    printf(">>> GPU SIZE STRUCT <<< \n");
    printf("[%5li] ParticleData\n", sizeof(ParticleData));
    printf("[%5li] ModuleParamRequired\n", sizeof(ModuleParamRequired));
    printf("[%5li] ModuleParamSpawn\n", sizeof(ModuleParamSpawn));
    printf("[%5li] ModuleParamSizeOverLifetime\n", sizeof(ModuleParamSizeOverLifetime));
    printf("[%5li] Rangef\n", sizeof(Rangef));
    printf("[%5li] Rangei\n", sizeof(Rangei));
    printf("[%5li] ParticleData\n", sizeof(ParticleData));
    printf("[%5li] ParticleDataSprite\n", sizeof(ParticleDataSprite));
    printf("[%5li] ParticleDataMovement\n", sizeof(ParticleDataMovement));
}



__kernel void PrintShapeSize() {
    printf(">>> GPU SIZE STRUCT SHAPE <<<\n");
    printf("[%5li] Shape\n", sizeof(Shape));
    printf("[%5li] Sphere\n", sizeof(Sphere));
    printf("[%5li] AABB\n", sizeof(AABB));
}