#pragma once

#include <Gui/AEditor.hpp>

class ParticleSystem;

class EditorParticleSystems : public  AEditor {
public:
	EditorParticleSystems();
	void render() override;

private:
	ParticleSystem *itemCurrent;
	char inputName_[36];
};
