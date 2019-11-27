#pragma once

#include "AParticleModule.hpp"
#include "Cl/ClQueue.hpp"
#include "OCGL_Buffer.hpp"
#include "Range.hpp"

class ParticleEmitterMesh;

struct ModuleSizeOverLifetimeParams {
    Rangef size;
};


class ModuleSizeOverLifetime : public AParticleModule {
public:
    ModuleSizeOverLifetime(AParticleEmitter &emitter);
    void	init() override;
    void	update(float deltaTime) override;
    void    reload() override;

    void setSizeMin(float min);
    void setSizeMax(float min);

    float &getSizeMin();
    float &getSizeMax();

private:
    cl::Buffer		bufferModule_;
    ModuleSizeOverLifetimeParams   moduleSizeOverLifetimeParams_;
};
