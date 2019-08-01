#pragma once

#include <NTL.hpp>

class DebugGraphic;

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

    void	        setDebug(bool b = false);
    void            setColor(glm::vec4 const &color);
    bool            isDebug() const;

    glm::vec4 color;

protected:
    virtual void    updateLines() = 0;
    virtual void    initDebug() = 0;
	void            updateDebug();


	std::vector<DebugVertex>		linesObject_;
private:
	bool debug_;
	size_t  pastSizeBufferLines_;
};