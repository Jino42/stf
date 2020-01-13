#include "NTL.hl"

void kernel PhysicConstrainInShapeUpdate(__global ParticleDataPhysic *dataPhysic,
                                    __global ModuleParamPhysicGravity *moduleParam) {
    uint id = get_global_id(0);
    __global ParticleDataPhysic *particlePhysic = &dataPhysic[id];

    particlePhysic->velocity += moduleParam->gravity;
}