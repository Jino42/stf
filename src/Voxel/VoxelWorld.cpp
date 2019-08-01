#include "VoxelWorld.hpp"
#include <boost/filesystem.hpp>
#include <iostream>
#include <Engine/ShaderManager.hpp>
#include <Engine/MainGraphic.hpp>
#include <Engine/Frustum.hpp>
#include <Engine/Display/DisplayWindow.hpp>
#include <Engine/CameraManager.hpp>

VoxelWorld::VoxelWorld(Camera &camera) :
camera_(camera) {
    camera_.getFrustum().setDebug(true);
    boost::filesystem::path pathRoot(ROOT_PATH);

    ShaderManager::Get().addShader("voxel");
    ShaderManager::Get().getShader("voxel").attach((pathRoot / "shader" / "voxel.vert").generic_string());
    ShaderManager::Get().getShader("voxel").attach((pathRoot / "shader" / "voxel.geom").generic_string());
    ShaderManager::Get().getShader("voxel").attach((pathRoot / "shader" / "voxel.frag").generic_string());
    //ShaderManager::Get().getShader("voxel").attach()
    ShaderManager::Get().getShader("voxel").link();
}

void VoxelWorld::start() {

    heightMapBuilder.SetSourceModule (VoxelWorld::Get().myModule);
    heightMapBuilder.SetDestNoiseMap (heightMap);
    heightMapBuilder.SetDestSize (CHUNK_SIZE * 3, CHUNK_SIZE * 3);


    for (int y = -DEFAULT_VOXEL_RADIUS_RENDER_DISTANCE; y < DEFAULT_VOXEL_RADIUS_RENDER_DISTANCE; y++)
        for (int x = -DEFAULT_VOXEL_RADIUS_RENDER_DISTANCE; x < DEFAULT_VOXEL_RADIUS_RENDER_DISTANCE; x++)
            chunks_.emplace_back(glm::vec3(x * CHUNK_SIZE, 0.0f, y * CHUNK_SIZE),
                                 (x + DEFAULT_VOXEL_RADIUS_RENDER_DISTANCE) + (y + DEFAULT_VOXEL_RADIUS_RENDER_DISTANCE) * DEFAULT_VOXEL_RADIUS_RENDER_DISTANCE);

}

void VoxelWorld::update() {

}

void VoxelWorld::render() {

    if (DisplayWindow::Get().getKey(GLFW_KEY_M) == KeyState::kDown)
        camera_.getFrustum().setDebug(!camera_.getFrustum().isDebug());
   camera_.getFrustum().update();


    Shader &shader = ShaderManager::Get().getShader("voxel");

    shader.activate();
    shader.setMat4("projection", Camera::focus->getProjectionMatrix());
    shader.setMat4("view", Camera::focus->getViewMatrix());

    for (auto &i : chunks_) {
        if (camera_.getFrustum().pointIn(i.position_.x + (CHUNK_SIZE >> 1), i.position_.y + (CHUNK_SIZE >> 1), i.position_.z + (CHUNK_SIZE >> 1)))
            i.render();
    }
}

void VoxelWorld::Init(Camera &camera) {
    VoxelWorld::instance_ = std::make_unique<VoxelWorld>(camera);
}

VoxelWorld &VoxelWorld::Get() {
    assert(!!instance_);
    return *instance_;
}

std::unique_ptr<VoxelWorld> VoxelWorld::instance_ = nullptr;