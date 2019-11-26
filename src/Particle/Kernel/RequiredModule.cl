
#ifdef WIN32
#define ALIGN __attribute__((packed))
#else
#define ALIGN
#endif

#define SUB_LENGTH_ALIVE 0
#define SUB_LENGTH_ALIVE2 1
#define SUB_LENGTH_DEATH 2

typedef struct ALIGN s_ParticleData {
    float3          position; 	// Center point of particle
    float3          velocity; 	// Current particle velocity
    float4          color;    	// Particle color
    float           rotate;  	// Rotate the particle center
    float           size;    	// Size of the particle
    float           age;
    float           lifeTime;	//If < 0 = Inifite lifeTime
    int             isAlive;

    int             index;
} ParticleData;

typedef struct ALIGN s_ParticleSpriteData {
    float2      offset1;
    float2      offset2;
    float       blend;
} ParticleSpriteData;


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
////////////////

#define SEED_MAX 2147483647

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

typedef struct ALIGN sRangef {
    int isRange;
    float min;
    float max;
} Rangef;

typedef struct ALIGN sRangei {
    int isRange;
    int min;
    int max;
} Rangei;

float getRandomRangef(__global Rangef *range, int seed)
{
    if (range->isRange)
        return randomMinMax(seed, range->min, range->max);
    return range->max;
}

typedef struct ALIGN sModuleRequiredParams {
    float gravityModifier;
    float simulationSpeed;
} ModuleRequiredParams;

typedef struct ALIGN sModuleSpawnParams {
    Rangef startLifeTime;
    Rangef startSpeed;
    Rangef startSize;
    Rangef startRotation;
} ModuleSpawnParams;

typedef struct ALIGN sModuleSizeOverLifetimeParams {
    Rangef size;
} ModuleSizeOverLifetimeParams;

#define offsetof(st, m) ((size_t)&(((st *)0)->m))

typedef struct ALIGN s_ParticleMovementModuleData {
	float3  acceleration;
	float3  velocity;
	float   masse;
} ParticleMovementModuleData;

////COLOR
typedef struct ALIGN {
	float r;       // a fraction between 0 and 1
	float g;       // a fraction between 0 and 1
	float b;       // a fraction between 0 and 1
} rgb;

typedef struct ALIGN {
	float h;       // angle in degrees
	float s;       // a fraction between 0 and 1
	float v;       // a fraction between 0 and 1
} hsv;

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
        __global ModuleRequiredParams *moduleParams,

        __global ParticleData *data,
        __global int *arrayParticlesAlive,
        __global int *arrayParticlesAlive2,
        __global int *arrayParticlesDeath,
        __global int *arrayParticlesLengthSub,

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
    int cubicParticle = (int)cbrt((float)gs);
    float3 index;
    index.x = id % cubicParticle;
    index.y = (id / cubicParticle) % cubicParticle;
    index.z = (id / (cubicParticle * cubicParticle)) % cubicParticle;

    particle->position.x = index.x - cubicParticle / 2;
    particle->position.y = index.y * -1 + cubicParticle / 2;
    particle->position.z = (-index.z / 8) + index.z;

    particle->position.x += particleSystemPosition.x;
    particle->position.y += particleSystemPosition.y;
    particle->position.z += particleSystemPosition.z;

    particle->lifeTime = 5.f;
///////////

    particle->age = 5.f;
    particle->lifeTime = 5.f;

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

void kernel CleanAlive(__global int *arrayParticlesAlive,
                        __global int *arrayParticlesAlive2,
                        __global int *arrayParticlesLengthSub) {
    arrayParticlesAlive[get_global_id(0)] = -1;
    arrayParticlesAlive2[get_global_id(0)] = -1;
    if (!get_global_id(0)) {
        arrayParticlesLengthSub[SUB_LENGTH_ALIVE] = 0;
        arrayParticlesLengthSub[SUB_LENGTH_ALIVE2] = 0;
    }
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
	printf("[%5li] ModuleRequiredParams\n", sizeof(ModuleRequiredParams));
	printf("[%5li] ModuleSpawnParams\n", sizeof(ModuleSpawnParams));
	printf("[%5li] ModuleSizeOverLifetimeParams\n", sizeof(ModuleSizeOverLifetimeParams));
	printf("[%5li] Rangef\n", sizeof(Rangef));
	printf("[%5li] Rangei\n", sizeof(Rangei));
	printf("[%5li] ParticleData\n", sizeof(ParticleData));
	printf("[%5li] ParticleSpriteData\n", sizeof(ParticleSpriteData));
	printf("[%5li] ParticleMovementModuleData\n", sizeof(ParticleMovementModuleData));
}

void kernel spawnMovementRandom(__global ParticleData *dataParticle,
                                __global int *arrayParticlesAlive2,
                                __global ParticleMovementModuleData *dataMovement,
                                int seed) {
    size_t id = arrayParticlesAlive2[get_global_id(0)];


    __global ParticleMovementModuleData *movement = &dataMovement[dataParticle[id].index];

    movement->acceleration = (float3)(0.f, 0.f, 0.f);
    movement->velocity = (float3)(0.f, 0.f, 0.f);
    movement->masse = 10.f;

    float3 force = normalize((float3) (randomMinMax(seed + id, -1.f, 1.f), randomMinMax((seed >> 1) + id, -1.f, 1.f), randomMinMax((seed << 2) + id, -1.f, 1.f))) * 3.f;
    movement->acceleration += force / movement->masse;
    force = normalize((float3) (0.f, -0.1f * movement->masse, 0.f));
    movement->acceleration += force / movement->masse;
}

void kernel movement(__global ParticleData *dataParticle,
        __global ParticleMovementModuleData *dataMovement,
        float deltaTime,
        float3 attractor) {
    size_t id = get_global_id(0);
    __global ParticleData *particle = &dataParticle[id];
    __global ParticleMovementModuleData *movement = &dataMovement[particle->index];

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

void kernel sizeUpdate(__global ParticleData *data,
        __global ModuleSizeOverLifetimeParams *moduleParams) {
    size_t id = get_global_id(0);
    __global ParticleData *particle = &data[id];

    particle->size = moduleParams->size.min + moduleParams->size.max * (particle->age / particle->lifeTime);
}

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
        __global ParticleData *data,
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



__kernel void spawnParticle(
                        __global ModuleSpawnParams *moduleParams,
                        __global ParticleData *data,

                        __global int *arrayParticlesAlive,
                        __global int *arrayParticlesAlive2,
                        __global int *arrayParticlesDeath,
                        __global int *arrayParticlesLengthSub,

                        float3 particleSystemPosition,
                        int seed) {




    size_t id = get_global_id(0);
    int res = removeDeath(
                      arrayParticlesDeath,
                      arrayParticlesLengthSub,
                      id
                  );
    addAlive2222222ToArray(arrayParticlesAlive2, arrayParticlesLengthSub, res);
    __global ParticleData *particle = &data[res];

    resetParticleData(particle);

    particle->color.w = 1.f;
    particle->color.x = 1.f;
    particle->color.y = 1.f;
    particle->color.z = 1.f;

    particle->position.x += particleSystemPosition.x;
    particle->position.y += particleSystemPosition.y;
    particle->position.z += particleSystemPosition.z;

    particle->isAlive = 1;
    particle->age = 0.f;
    particle->lifeTime = getRandomRangef(&moduleParams->startLifeTime, seed + id);
    particle->size = getRandomRangef(&moduleParams->startSize, seed + id);
    particle->rotate = getRandomRangef(&moduleParams->startRotation, seed + id);
}


float2 setSpriteOffset(unsigned int numberRows, int index) {
    int column;
    int row ;
    if (numberRows != 0) {
        column = index % numberRows;
        row = index / numberRows;
    }
    float2 offset;
    if (numberRows != 0) {
        offset.x = (float)column / numberRows;
        offset.y = (float)row / numberRows;
    }
    else
    {
        offset.x = 0;
        offset.y = 0;
    }
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


/*

 void PrintParticle(
        __global ParticleData const *data)
{
    int id = get_global_id(0);
    __global ParticleData const *particle = &data[id];
    printf("%i \n", id);
    if (!id)
    {
        for (int i = 0; i < get_global_size(0); i++)
        {
            particle = &data[i];
            //printf("%i index %i  life %f / %f \n", i, particle->index, particle->age, particle->lifeTime);
        }
    }

}
*/

void kernel printStructSizeFunc() {
    printf(">>> GPU SIZE STRUCT <<< \n");
    printf("[%5li] ModuleRequiredParams \n", sizeof(ModuleRequiredParams));
    printf("[%5li] ModuleSpawnParams \n", sizeof(ModuleSpawnParams));
    printf("[%5li] ModuleSizeOverLifetimeParams \n", sizeof(ModuleSizeOverLifetimeParams));
    printf("[%5li] Rangef \n", sizeof(Rangef));
    printf("[%5li] Rangei \n", sizeof(Rangei));
    printf("[%5li] ParticleData \n", sizeof(ParticleData));
    printf("[%5li] ParticleSpriteData \n", sizeof(ParticleSpriteData));
    printf("[%5li] ParticleMovementModuleData \n", sizeof(ParticleMovementModuleData));
}

void kernel attractor(__global ParticleData *data,
		unsigned int flag,
		float3 cameraPosition,
		float deltaTime) {
	size_t id = get_global_id(0);
	__global ParticleData *particle = &data[id];

	float3 position;
	position.x = particle->position.x;
	position.y = particle->position.y;
	position.z = particle->position.z;

	float3 dist = cameraPosition - position;

	float3 acceleration;
	acceleration.x = 0.f;
	acceleration.y = 0.f;
	acceleration.z = 0.f;


	//Centered Particle
	if (particle->velocity.x > 0.1f)
	particle->velocity.x *= 0.95f;
	if (particle->velocity.y > 0.1f)
	particle->velocity.y *= 0.95f;
	if (particle->velocity.z > 0.1f)
	particle->velocity.z *= 0.95f;

	if (fabs(dist.x) > 0.001f && fabs(dist.y) > 0.001f && fabs(dist.z) > 0.001f)
	{
		acceleration = normalize(dist) * 100;
		particle->velocity.x += acceleration.x;
		particle->velocity.y += acceleration.y;
		particle->velocity.z += acceleration.z;
	}

	particle->position.x += particle->velocity.x * deltaTime;
	particle->position.y += particle->velocity.y * deltaTime;
	particle->position.z += particle->velocity.z * deltaTime;


	//Plane Collision
	//if (particle->position.x > 20.f)
	//    particle->position.x  = 20.f;
	//if (particle->position.y > 20.f)
	//    particle->position.y  = 20.f;
	//if (particle->position.z > 20.f)
	//    particle->position.z  = 20.f;
}

static hsv   rgb2hsv(rgb in);
static rgb   hsv2rgb(hsv in);

float4 hsv_to_rgb(float H, float S, float V)
{
	hsv in;
	in.h = H;
	in.s = S;
	in.v = V;

	float      hh, p, q, t, ff;
	long        i;
	rgb         out;

	if(in.s <= 0.0) {
		float4 ret;
		ret.x = in.v;
		ret.y = in.v;
		ret.z = in.v;
		ret.w = 1.f;
		return ret;
	}
	hh = in.h;
	if(hh >= 360.0f)
	{
		hh = 0.0f;
	}

	hh = hh / 60.0f;
	i = (long)hh;
	ff = hh - i;
	p = in.v * (1.0f - in.s);
	q = in.v * (1.0f - (in.s * ff));
	t = in.v * (1.0f - (in.s * (1.0f - ff)));

	switch(i) {
		case 0:
			out.r = in.v;
			out.g = t;
			out.b = p;
			break;
		case 1:
			out.r = q;
			out.g = in.v;
			out.b = p;
			break;
		case 2:
			out.r = p;
			out.g = in.v;
			out.b = t;
			break;

		case 3:
			out.r = p;
			out.g = q;
			out.b = in.v;
			break;
		case 4:
			out.r = t;
			out.g = p;
			out.b = in.v;
			break;
		case 5:
		default:
			out.r = in.v;
			out.g = p;
			out.b = q;
			break;
	}
	float4 ret;
	ret.x = out.r;
	ret.y = out.g;
	ret.z = out.b;
	ret.w = 1.f;
	return ret;
}

float4 lerpColorVec4(float4 a, float4 b, float t)
{
	float4 ret;

	ret.x = a.x + t * (b.x - a.x);
	ret.y = a.y + t * (b.y - a.y);
	ret.z = a.z + t * (b.z - a.z);
	ret.w = a.w + t * (b.w - a.w);

	return ret;
}

void kernel color_radius_from_position(__global ParticleData *data,
		float3 from,
	float radius,
	float deltaTime) {
	size_t id = get_global_id(0);
	__global ParticleData *particle = &data[id];

	float4 end = hsv_to_rgb(0.f, 0.93f, 0.84f);
	float4 start = hsv_to_rgb(60.f, 1.f, 0.92f);

	float3 position;
	position.x = particle->position.x;
	position.y = particle->position.y;
	position.z = particle->position.z;
	particle->color = lerpColorVec4(start, end, clamp(fabs(length(from - position)) / radius, 0.f, 1.f));
}
