#pragma  once

#include "AParticleEmitter.hpp"

class Shader;

class ParticleEmitterPoint : public AParticleEmitter {
public:
	ParticleEmitterPoint(ParticleSystem &system, ClQueue &queue, std::string const &name, size_t nbParticle);

	void init() override;

	void render() override;

	void update(float deltaTime) override;

private:
	unsigned int	VAO;
};

