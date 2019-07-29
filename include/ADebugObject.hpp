#pragma once

#include <NTL.hpp>

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
	ADebugObject();

	virtual ~ADebugObject() = default;

protected:
	void	setDebug(bool b);
	virtual void	init() = 0;
	virtual void	updateDebug() = 0;

	std::vector<DebugVertex>		linesObject_;
private:
	bool debug_;
};