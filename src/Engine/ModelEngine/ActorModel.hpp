#pragma once

#include "Model.hpp"
#include "Transform.hpp"

class ActorModel : public Transform {
public:

	ActorModel();
	ActorModel(Model const *model);
	virtual ~ActorModel() = default;
	ActorModel(ActorModel const &src);

	Model const		*getModel() const;
	void			assign(Model const *model);

	void			render(Shader &shader, GLenum typeOfDraw = GL_TRIANGLES);

private:
	std::bitset<16>		flag_;
	Model const			*model_;

	ActorModel &operator=(ActorModel const &src) = delete;
};
