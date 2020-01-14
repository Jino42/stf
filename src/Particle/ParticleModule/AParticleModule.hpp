#pragma once

#include "Cl/ClKernel.hpp"
#include <NTL.hpp>
#include <boost/filesystem.hpp>
#include <json.hpp>

class AParticleEmitter;
class ClQueue;

class AParticleModule {
public:
  explicit AParticleModule(AParticleEmitter &emitter);
  virtual ~AParticleModule() = default;
  virtual void init(){};
  virtual void update(float deltaTime){};
  virtual void spawn(unsigned int, unsigned int){};
  virtual void reload(){};

  std::string getModuleName() const;

  virtual void jsonParse(json &itModule){};
  virtual void gui() {};


protected :
  AParticleEmitter &emitter_;
  ClQueue &queue_;
  unsigned int &nbParticleMax_;
  unsigned int &nbParticlePerSec_;
  unsigned int &nbParticleActive_;
  boost::filesystem::path &pathKernel_;

  ClKernel kernelInit_;
  ClKernel kernelUpdate_;
  ClKernel kernelSpawn_;

  static bool debug_;
};
