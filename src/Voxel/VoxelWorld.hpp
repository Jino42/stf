#pragma once

#include <Voxel/Chunk.hpp>
#include <Engine/Camera.hpp>
#include <memory>
#include <noise/noise.h>
#include <noiseutils.h>
#include <list>

#define DEFAULT_VOXEL_RADIUS_RENDER_DISTANCE 16

class VoxelWorld {
public:
    VoxelWorld(Camera &camera);

    void start();
    void update();
    void render();

    static void Init(Camera &camera);
    static VoxelWorld &Get();

    noise::module::Perlin myModule;
    utils::NoiseMap heightMap;
    utils::NoiseMapBuilderPlane heightMapBuilder;
    utils::RendererImage renderer;
    utils::Image image;

private:
    Camera &camera_;
    glm::ivec2 lastChucnkPosition_;
    std::list< Chunk > chunks_;

    static std::unique_ptr<VoxelWorld> instance_;
};