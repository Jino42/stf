#pragma once

#include <boost/filesystem.hpp>
#include <NTL.hpp>

class AParticleEmitter;
class ClQueue;

class AParticleModule {
public:
	explicit AParticleModule (AParticleEmitter &emitter);
	virtual			~AParticleModule() = default;
	virtual	void	init() = 0;
	virtual	void	update(float deltaTime) = 0;
	virtual	void	spawn(unsigned int, unsigned int) {};
	virtual void    reload() = 0;

protected:
	AParticleEmitter		&emitter_;
    ClQueue					&queue_;
    unsigned int 			&nbParticleMax_;
    unsigned int 			&nbParticlePerSec_;
    unsigned int			&nbParticleActive_;
	boost::filesystem::path	pathKernel_;

	static bool debug_;
};