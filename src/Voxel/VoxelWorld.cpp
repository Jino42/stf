#include "VoxelWorld.hpp"
#include <boost/filesystem.hpp>
#include <iostream>

VoxelWorld::VoxelWorld(Camera &camera) :
camera_(camera) {
    boost::filesystem::path pathRoot(ROOT_PATH);

    shaderChunk_.attach((pathRoot / "shader" / "voxel.vert").generic_string());
    shaderChunk_.attach((pathRoot / "shader" / "voxel.geom").generic_string());
    shaderChunk_.attach((pathRoot / "shader" / "voxel.frag").generic_string());
    shaderChunk_.link();

    for (int y = -DEFAULT_VOXEL_RADIUS_RENDER_DISTANCE; y < DEFAULT_VOXEL_RADIUS_RENDER_DISTANCE; y++)
        for (int x = -DEFAULT_VOXEL_RADIUS_RENDER_DISTANCE; x < DEFAULT_VOXEL_RADIUS_RENDER_DISTANCE; x++)
            chunks_.emplace_back(glm::vec3(x * CHUNK_SIZE_X, 0.0f, y * CHUNK_SIZE_Z),
                                 (x + DEFAULT_VOXEL_RADIUS_RENDER_DISTANCE) + (y + DEFAULT_VOXEL_RADIUS_RENDER_DISTANCE) * DEFAULT_VOXEL_RADIUS_RENDER_DISTANCE);
}

Shader &VoxelWorld::getShaderChunk() {
    return shaderChunk_;
}

void VoxelWorld::update() {

}

void VoxelWorld::render() {
    for (auto &i : chunks_)
        i.render();
}

void VoxelWorld::Init(Camera &camera) {
    VoxelWorld::instance_ = std::make_unique<VoxelWorld>(camera);
}

VoxelWorld &VoxelWorld::Get() {
    assert(!!instance_);
    return *instance_;
}

std::unique_ptr<VoxelWorld> VoxelWorld::instance_ = nullptr;