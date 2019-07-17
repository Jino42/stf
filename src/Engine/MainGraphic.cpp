#include "MainGraphic.hpp"
#include "Time.hpp"
#include <Voxel/VoxelWorld.hpp>
#include "Engine/Camera.hpp"
#include "Engine/Display/DisplayWindow.hpp"


MainGraphic::MainGraphic() :
        renderBuffer(DisplayWindow::Get().getWidthWindow() / 2.0f, DisplayWindow::Get().getHeightWindow()),
		deltaTime_(0.014f),
		projection_(1.f),
		view_(1.f) {
	glEnable(GL_DEPTH_TEST);
	glPointSize(1.0f);
	glLineWidth(5.0f);
	glDepthFunc(GL_LESS);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glDepthMask(GL_FALSE);

	projection_ = glm::perspective(glm::radians(45.0f),
	        (float) (((float)DisplayWindow::Get().getWidthWindow() / 2.0f) / (float)DisplayWindow::Get().getHeightWindow()),
	        NEAR_PLANE, MAX_PLANE);
	Camera::Get().setPosition(glm::vec3(0.f, 258.f, -5.f));


    VoxelWorld::Init(Camera::Get());

}

MainGraphic &MainGraphic::Get() {
    if (!instance_)
        instance_ = std::make_unique<MainGraphic>();
	return *MainGraphic::instance_;
}

void MainGraphic::render() {
    view_ = Camera::Get().getViewMatrix();

	if (DisplayWindow::Get().getKey(GLFW_KEY_RIGHT) || DisplayWindow::Get().getKey(GLFW_KEY_D))
        Camera::Get().processPosition(Camera::Movement::RIGHT, deltaTime_ * 5);
	if (DisplayWindow::Get().getKey(GLFW_KEY_LEFT) || DisplayWindow::Get().getKey(GLFW_KEY_A))
        Camera::Get().processPosition(Camera::Movement::LEFT, deltaTime_ * 5);
	if (DisplayWindow::Get().getKey(GLFW_KEY_DOWN) || DisplayWindow::Get().getKey(GLFW_KEY_S))
        Camera::Get().processPosition(Camera::Movement::BACKWARD, deltaTime_ * 5);
	if (DisplayWindow::Get().getKey(GLFW_KEY_UP) || DisplayWindow::Get().getKey(GLFW_KEY_W))
        Camera::Get().processPosition(Camera::Movement::FORWARD, deltaTime_ * 5);

    renderBuffer.bind();
    renderBuffer.clear();

    VoxelWorld::Get().render();
    renderBuffer.unbind();
}

void MainGraphic::update() {
    if (DisplayWindow::Get().getKeyState(GLFW_KEY_SPACE) == KeyState::kDown)
        DisplayWindow::Get().swapCursor();
}

glm::mat4 const &MainGraphic::getViewMatrix() const {
	return view_;
}
glm::mat4 const &MainGraphic::getProjectionMatrix() const {
	return projection_;
}

std::unique_ptr<MainGraphic> MainGraphic::instance_ = nullptr;