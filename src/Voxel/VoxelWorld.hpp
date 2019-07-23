#pragma once

#include <Voxel/Chunk.hpp>
#include <Engine/Camera.hpp>
#include <memory>
#include <noise/noise.h>
#include <noiseutils.h>

#define DEFAULT_VOXEL_RADIUS_RENDER_DISTANCE 8

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
    std::vector< Chunk > chunks_;

    static std::unique_ptr<VoxelWorld> instance_;
};