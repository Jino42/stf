#include "ActorModel.hpp"

ActorModel::ActorModel() :
		Transform(),
		flag_(0),
		model_(nullptr) {}

ActorModel::ActorModel(Model const *model) :
		Transform(),
		flag_(0),
		model_(model) {
	setCenter(model->getPositionCenterRelativeToOrigin());
	setInterScaling(model->getInterScaling());
}

ActorModel::ActorModel(ActorModel const &src) :
		Transform(src),
		flag_(src.flag_),
		model_(src.model_) {}

void ActorModel::assign(Model const *model) {
	model_ = model;
	setCenter(model->getPositionCenterRelativeToOrigin());
	setInterScaling(model->getInterScaling());
}

void	ActorModel::render(Shader &shader, GLenum typeOfDraw) {
	updateTransform();
	glm::mat4 modelMatrix = transform_;
	shader.setMat4("model", modelMatrix);
	model_->render(shader, typeOfDraw);
}

Model const		*ActorModel::getModel() const {
	return (model_);
}
