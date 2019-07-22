#pragma once

#include "AParticleModule.hpp"
#include "Cl/ClQueue.hpp"
#include "DeviceBuffer.hpp"
#include "Range.hpp"

class AParticleEmitter;

//Every particle emitter contains this module which contains all the properties that are required by a particle emitter.

struct ModuleRequiredParams {

    float gravityModifier = 1.0f;
    float simulationSpeed = 1.0f;
};

class ParticleRequiredModule : public AParticleModule {
public:
	ParticleRequiredModule(AParticleEmitter &emitter);
	void	init() override;
	void	update(float deltaTime) override;
    void    reload() override;

    //EmitterPARAMETER
	//Material ?
	//EmitterOrigin
	//EmitterRotation
	//Screen Alignment LATTER

	//bKillOnDesativate
	//sortMode


	//DurationPARAMETER

	//Sub UV
	//InterpolationMethod

	//Rendering
	//bMaxDrawCount
	//MaxDrawCount
private:
    ModuleRequiredParams    moduleRequiredParams_;
    cl::Buffer              bufferModuleParams_;
};
