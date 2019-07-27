#pragma once

#include <ALifeTimeObject.hpp>
#include <NTL.hpp>
#include <glad/glad.h>

struct DebugVertex {
	DebugVertex() = default;
	DebugVertex(glm::vec3 aPosition, glm::vec4 aColor) :
	position(aPosition),
	color(aColor) { }

	glm::vec3 position;
	glm::vec4 color;
};

class ADebugObject {
	friend class DebugGraphic;
public:
	virtual ~ADebugObject() = default;

protected:
	virtual void	init() = 0;
	virtual void	updateLines() = 0;
	std::vector<DebugVertex>		linesObject_;
};