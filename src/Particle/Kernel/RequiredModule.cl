#include "NTL.hl"


////////////////////////////////////////

void kernel PrintSubArrayParticle(
        __global int *arrayParticlesAlive,
        __global int *arrayParticlesAlive2,
        __global int *arrayParticlesDeath,
        __global int *arrayParticlesLengthSub,
        int nbMaxParticles)
{
    printf("STTT : [ALIV][ALI2][DEAT]\n");
    for (int i = 0; i < nbMaxParticles; i++) {
        printf("%4d : [%4d][%4d][%4d]\n",
                            i,
                            arrayParticlesAlive[i],
                            arrayParticlesAlive2[i],
                            arrayParticlesDeath[i]
                            );
    }

    printf("INDEX : [%4d][%4d][%4d]\n",
        arrayParticlesLengthSub[SUB_LENGTH_ALIVE],
        arrayParticlesLengthSub[SUB_LENGTH_ALIVE2],
        arrayParticlesLengthSub[SUB_LENGTH_DEATH]
    );
}

void kernel RequiredInitialisation(

        __global ParticleData *data,
        __global int *arrayParticlesAlive,
        __global int *arrayParticlesAlive2,
        __global int *arrayParticlesDeath,
        __global int *arrayParticlesLengthSub,

        __global ModuleParamRequired *moduleParams,


        float4 particleSystemPosition,
        int seed,
        int nbMaxParticles){
	size_t id = get_global_id(0);
    __global ParticleData *particle = &data[id];

    arrayParticlesAlive[id] = -1;
    arrayParticlesAlive2[id] = -1;
    arrayParticlesDeath[id] = id;

    unsigned int i = 0;
    while (i < sizeof(ParticleData)) {
        ((__global char *)particle)[i] = 0;
        i++;
    }

    particle->index = id;

///////////
    particle->position.x = 0.f;
    particle->position.y = 0.f;
    particle->position.z = id * 0.1f;
    particle->color.w = 1.f;
    particle->color.x = 1.f;
    particle->color.y = 1.f;
    particle->color.z = 1.f;

    int gs = get_global_size(0);
    int cubicParticle = (int)ceil(cbrt((float)gs));
    float3 index;
    index.x = id % cubicParticle;
    index.y = (id / cubicParticle) % cubicParticle;
    index.z = (id / (cubicParticle * cubicParticle)) % cubicParticle;

    float f = 0.5f;
    particle->position.x = index.x - cubicParticle / 2;
    particle->position.y = index.y * -1 + cubicParticle / 2;
    particle->position.z = (-index.z / 8) + index.z;
    particle->position *= f;

    particle->position.x += particleSystemPosition.x;
    particle->position.y += particleSystemPosition.y;
    particle->position.z += particleSystemPosition.z;

    particle->lifeTime = 5.f;
///////////

    particle->age = 5.f;
    particle->lifeTime = 5.f;

    printf("g_id[%i] p[%f][%f][%f]\n", id, particle->position.x, particle->position.y, particle->position.z);
    
    //particle->lifeTime = getRandomRangef(&moduleParams->startLifeTime, seed + id);
    //particle->age = particle->lifeTime;
}



void addDeathToArray(
                     __global int *arrayParticlesDeath,
                     __global int *arrayParticlesLengthSub,
                     size_t id)
{
    int szDeath = atomic_add(&(arrayParticlesLengthSub[SUB_LENGTH_DEATH]), 1) + 1;

    //check §

    if (szDeath - 1 >= 0)
        arrayParticlesDeath[szDeath - 1] = id;
}

void addAliveToArray(
                     __global int *arrayParticlesAlive,
                     __global int *arrayParticlesLengthSub,
                     size_t id)
{
    int szAlive = atomic_add(&(arrayParticlesLengthSub[SUB_LENGTH_ALIVE]), 1) + 1;

    //check §

    if (szAlive - 1 >= 0)
        arrayParticlesAlive[szAlive - 1] = id;
}

void addAlive2222222ToArray(
                     __global int *arrayParticlesAlive2,
                     __global int *arrayParticlesLengthSub,
                     size_t id)
{
    int szAlive2 = atomic_add(&(arrayParticlesLengthSub[SUB_LENGTH_ALIVE2]), 1) + 1;

    //check §

    if (szAlive2 - 1 >= 0)
        arrayParticlesAlive2[szAlive2 - 1] = id;
}

int removeDeath(    __global int *arrayParticlesDeath,
                     __global int *arrayParticlesLengthSub,
                     size_t id)
{
    int szDeath = atomic_sub(&(arrayParticlesLengthSub[SUB_LENGTH_DEATH]), 1);

    if (szDeath - 1 < 0)
        printf("-----XXXX-----------XXXxXConnard [%i]\n", szDeath);

    int ret = arrayParticlesDeath[szDeath - 1];
    arrayParticlesDeath[szDeath - 1] = -1;
    //check §

    return ret;
}

void kernel RequiredUpdate(__global ParticleData *data,
                            __global int *arrayParticlesAlive,
                            __global int *arrayParticlesAlive2,
                            __global int *arrayParticlesDeath,
                            __global int *arrayParticlesLengthSub,
                                float deltaTime) {
    size_t id = get_global_id(0);
    __global ParticleData *particle = &data[id];

    particle->age += deltaTime;

    if (id < arrayParticlesLengthSub[SUB_LENGTH_ALIVE2])
            arrayParticlesAlive2[id] = -1;

    if (particle->isAlive && !particleIsActive(particle))
    {
        particle->isAlive = 0;
        addDeathToArray(arrayParticlesDeath, arrayParticlesLengthSub, id);
    }
    else if (particle->isAlive)
    {
        addAliveToArray(arrayParticlesAlive, arrayParticlesLengthSub, id);
    }
}

void kernel do_nothing(__global ParticleData *data) {
    (void)data;
}
//////////
//////////

void kernel printStructSize(__global ParticleData *dataParticle) {
	printf(">>> GPU SIZE STRUCT <<<\n");
	printf("%f\n", dataParticle->rotate);
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

void kernel printStructSizeFunc() {
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

