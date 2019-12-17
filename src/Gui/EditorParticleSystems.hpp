#pragma once

#include <Gui/AEditor.hpp>
#include <string>

class ParticleSystem;

class EditorParticleSystems : public AEditor {
  public:
    EditorParticleSystems();
    void render() override;
    void reload() override;

  private:
    std::string keySystem_;
    size_t indexEmitter_;
    size_t indexModule_;
    char inputName_[36];
};
