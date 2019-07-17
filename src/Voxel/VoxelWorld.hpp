#pragma once

#include <Engine/Shader.hpp>
#include <Voxel/Chunk.hpp>
#include <Engine/Camera.hpp>
#include <memory>

#define DEFAULT_VOXEL_RADIUS_RENDER_DISTANCE 2

class VoxelWorld {
public:
    VoxelWorld(Camera &camera);
    Shader &getShaderChunk();

    void update();
    void render();

    static void Init(Camera &camera);
    static VoxelWorld &Get();

private:
    Camera &camera_;
    Shader shaderChunk_;
    std::vector< Chunk > chunks_;

    static std::unique_ptr<VoxelWorld> instance_;
};