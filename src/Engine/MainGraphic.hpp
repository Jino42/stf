#pragma once

#include <glm/glm.hpp>
#include <memory>
#include "RenderBuffer.hpp"

class MainGraphic {
public:
    MainGraphic();
	MainGraphic &operator=(MainGraphic const &rhs) = delete;
	MainGraphic(MainGraphic const &src) = delete;

	void init();
	void render();
    void update();

	static MainGraphic &Get();

	RenderBuffer					&getRenderBuffer();

private:
	float							deltaTime_;
	RenderBuffer                    renderBuffer_;

	static std::unique_ptr<MainGraphic> instance_;
};