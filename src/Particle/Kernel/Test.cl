typedef struct s_ParticleData {
	float3          position; 	// Center point of particle
	float3          velocity; 	// Current particle velocity
	float4          color;    	// Particle color
	float           rotate;  	// Rotate the particle center
	float           size;    	// Size of the particle
	float           age;
	float           lifeTime;	//If < 0 = Inifite lifeTime
	int             index;
} ParticleData;

typedef struct s_ParticleSpriteData {
    float2      offset1;
    float2      offset2;
    float       blend;
} ParticleSpriteData;

typedef struct sRangef {
    int isRange;
    float min;
    float max;
} Rangef;

typedef struct sRangei {
    int isRange;
    int min;
    int max;
} Rangei;

typedef struct sModuleRequiredParams {
    float gravityModifier;
    float simulationSpeed;
} ModuleRequiredParams;

typedef struct sModuleSpawnParams {
    Rangef startLifeTime;
    Rangef startSpeed;
    Rangef startSize;
    Rangef startRotation;
} ModuleSpawnParams;

typedef struct sModuleSizeOverLifetimeParams {
    Rangef size;
} ModuleSizeOverLifetimeParams;

void kernel Test() {
    printf(">>> GPU SIZE STRUCT <<< \n");
        printf("[%5li] ModuleRequiredParams \n", sizeof(ModuleRequiredParams));
        printf("[%5li] ModuleSpawnParams \n", sizeof(ModuleSpawnParams));
        printf("[%5li] ModuleSizeOverLifetimeParams \n", sizeof(ModuleSizeOverLifetimeParams));
        printf("[%5li] Rangef \n", sizeof(Rangef));
        printf("[%5li] Rangei \n", sizeof(Rangei));
        printf("[%5li] ParticleData \n", sizeof(ParticleData));
        printf("[%5li] ParticleSpriteData \n", sizeof(ParticleSpriteData));
}