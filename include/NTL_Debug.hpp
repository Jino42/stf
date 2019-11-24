#pragma once

#include <Particle/ParticleModule/ParticleRequiredModule.hpp>
#include <Particle/ParticleModule/ParticleAttractorModule.hpp>
#include <Particle/ParticleModule/ParticleMovementModule.hpp>
#include <Particle/ParticleModule/ParticleColorModule.hpp>
#include <Particle/ParticleModule/ParticleSpawnModule.hpp>
#include <Particle/ParticleModule/ModuleSizeOverLifetime.hpp>
#include <Particle/PaticleEmitter/ParticleEmitterSprite.hpp>
#include <Particle/ParticleData.hpp>

void printStructSizeCPU();
void printStructSizeGPU(AParticleEmitter &emitter, cl::CommandQueue &queue);
void printStructSizeGPUBase(cl::CommandQueue &queue);