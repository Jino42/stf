#pragma once

#include <Voxel/Chunk.hpp>
#include <Engine/Camera.hpp>
#include <memory>

#define DEFAULT_VOXEL_RADIUS_RENDER_DISTANCE 1

class VoxelWorld {
public:
    VoxelWorld(Camera &camera);

    void update();
    void render();

    static void Init(Camera &camera);
    static VoxelWorld &Get();

private:
    Camera &camera_;
    std::vector< Chunk > chunks_;

    static std::unique_ptr<VoxelWorld> instance_;
};