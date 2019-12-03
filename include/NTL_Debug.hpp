#pragma once

#include <Particle/ParticleModule/ModuleRequired.hpp>
#include <Particle/ParticleModule/ModuleAttractor.hpp>
#include <Particle/ParticleModule/ModuleMovement.hpp>
#include <Particle/ParticleModule/ModuleColor.hpp>
#include <Particle/ParticleModule/ParticleSpawnModule.hpp>
#include <Particle/ParticleModule/ModuleSizeOverLifetime.hpp>
#include <Particle/PaticleEmitter/ParticleEmitterSprite.hpp>
#include <Particle/ParticleData.hpp>

void printStructSizeCPU();
void printStructSizeGPU(AParticleEmitter &emitter, cl::CommandQueue &queue);
void printStructSizeGPUBase(cl::CommandQueue &queue);
void printSubArrayParticle(AParticleEmitter &emitter, cl::CommandQueue &queue);