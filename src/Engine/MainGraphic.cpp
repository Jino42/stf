#include "MainGraphic.hpp"
#include "Time.hpp"
#include <Voxel/VoxelWorld.hpp>
#include "Engine/CameraManager.hpp"
#include "Engine/Display/DisplayWindow.hpp"
#include <Engine/TestParticle.hpp>
#include <Engine/ModelEngine/MainGraphicExtendModel.hpp>

MainGraphic::MainGraphic() :
        renderBuffer_(DisplayWindow::Get().getWidthWindow() / 2, DisplayWindow::Get().getHeightWindow()),
		deltaTime_(0.014f) {
	glEnable(GL_DEPTH_TEST);
	glPointSize(1.0f);
	glLineWidth(5.0f);
	glDepthFunc(GL_LESS);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glDepthMask(GL_FALSE);
}

MainGraphic &MainGraphic::Get() {
    if (!instance_)
        instance_ = std::make_unique<MainGraphic>();
	return *MainGraphic::instance_;
}

void MainGraphic::init() {
	CameraManager::Get().getFocus().setPosition(glm::vec3(2.7f, 38.f, 55.f));

	MainGraphicExtendModel::Get();
	//TestParticle::Get().init();
    CameraManager::Get().addCamera("Voxel");
    CameraManager::Get().getCamera("Voxel").setFar(682.0f);
    CameraManager::Get().getCamera("Voxel").setFov(59.7f);
    CameraManager::Get().getCamera("Voxel").setPosition(glm::vec3(-18.5f, 140.35f, 167.15f));
	VoxelWorld::Init(CameraManager::Get().getCamera("Voxel"));
	VoxelWorld::Get().start();

    CameraManager::Get().getCamera("Default").setFar(1000.0f);
    CameraManager::Get().getCamera("Default").setPosition(glm::vec3(82.5f, 300.35f, 226.15f));
}

void MainGraphic::render() {
	if (DisplayWindow::Get().getKey(GLFW_KEY_RIGHT) || DisplayWindow::Get().getKey(GLFW_KEY_D))
		Camera::focus->processPosition(Camera::Movement::RIGHT, deltaTime_ * 5);
	if (DisplayWindow::Get().getKey(GLFW_KEY_LEFT) || DisplayWindow::Get().getKey(GLFW_KEY_A))
		Camera::focus->processPosition(Camera::Movement::LEFT, deltaTime_ * 5);
	if (DisplayWindow::Get().getKey(GLFW_KEY_DOWN) || DisplayWindow::Get().getKey(GLFW_KEY_S))
		Camera::focus->processPosition(Camera::Movement::BACKWARD, deltaTime_ * 5);
	if (DisplayWindow::Get().getKey(GLFW_KEY_UP) || DisplayWindow::Get().getKey(GLFW_KEY_W))
		Camera::focus->processPosition(Camera::Movement::FORWARD, deltaTime_ * 5);
	if (DisplayWindow::Get().getKey(GLFW_KEY_RIGHT_CONTROL) || DisplayWindow::Get().getKey(GLFW_KEY_Q))
		Camera::focus->processPosition(Camera::Movement::DOWN, deltaTime_ * 5);
	if (DisplayWindow::Get().getKey(GLFW_KEY_RIGHT_SHIFT) || DisplayWindow::Get().getKey(GLFW_KEY_E))
		Camera::focus->processPosition(Camera::Movement::UP, deltaTime_ * 5);

    renderBuffer_.bind();
    renderBuffer_.clear();

    MainGraphicExtendModel::Get().update(0.014f);
    //TestParticle::Get().update(0.014f);
    VoxelWorld::Get().update();
    VoxelWorld::Get().render();

	debug_.render();


    renderBuffer_.unbind();
}

void MainGraphic::update() {
    if (DisplayWindow::Get().getKeyState(GLFW_KEY_SPACE) == KeyState::kDown)
        DisplayWindow::Get().swapCursor();
}

std::unique_ptr<MainGraphic> MainGraphic::instance_ = nullptr;

RenderBuffer &MainGraphic::getRenderBuffer() {
	return renderBuffer_;
}

DebugGraphic &MainGraphic::getDebugGraphic() {
	return debug_;
}
