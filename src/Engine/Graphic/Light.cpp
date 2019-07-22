
#include "Light.hpp"
#include "Engine/Shader.hpp"
#include <iostream>

Light::Light(Model const *model, glm::vec3 const &position, float intensity) :
ActorModel(model),
intensity_(intensity) {
	translate(position);
}
Light::Light(glm::vec3 const &position, float intensity) :
ActorModel(),
intensity_(intensity)  {
	translate(position);
}

Light::~Light() {
}

void 		Light::putLightToShader(Shader const &shader) const {
	shader.setFloat("uLight.intensity", intensity_);
	shader.setVec3("uLight.position", getPosition());
}