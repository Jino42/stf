#pragma once

#include <glm/glm.hpp>
#include <memory>
#include "RenderBuffer.hpp"


#define NEAR_PLANE 0.1f
#define MAX_PLANE 300.f

class MainGraphic {
public:
    MainGraphic();
	MainGraphic &operator=(MainGraphic const &rhs) = delete;
	MainGraphic(MainGraphic const &src) = delete;

	void render();
    void update();

	glm::mat4 const &getViewMatrix() const;
	glm::mat4 const &getProjectionMatrix() const;
	static MainGraphic &Get();


    RenderBuffer                    renderBuffer;
private:
	float							deltaTime_;
	glm::mat4						projection_;
	glm::mat4						view_;

	static std::unique_ptr<MainGraphic> instance_;
};