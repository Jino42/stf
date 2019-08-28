#include "VoxelWorld.hpp"
#include <boost/filesystem.hpp>
#include <iostream>
#include <Engine/ShaderManager.hpp>
#include <Engine/MainGraphic.hpp>
#include <Engine/Frustum.hpp>
#include <Engine/Display/DisplayWindow.hpp>
#include <Engine/CameraManager.hpp>
#include <Engine/Time.hpp>

VoxelWorld::VoxelWorld(Camera &camera) :
camera_(camera) {
    camera_.getFrustum().setDebug(true);
    boost::filesystem::path pathRoot(ROOT_PATH);

    ShaderManager::Get().addShader("voxel");
    ShaderManager::Get().getShader("voxel").attach((pathRoot / "shader" / "voxel.vert").generic_string());
    ShaderManager::Get().getShader("voxel").attach((pathRoot / "shader" / "voxel.geom").generic_string());
    ShaderManager::Get().getShader("voxel").attach((pathRoot / "shader" / "voxel.frag").generic_string());
    ShaderManager::Get().getShader("voxel").link();
}

void VoxelWorld::start() {

    lastChucnkPosition_.x = camera_.getPosition().x / CHUNK_SIZE_X;
    lastChucnkPosition_.y = camera_.getPosition().z / CHUNK_SIZE_Z;



    heightMapBuilder.SetSourceModule (VoxelWorld::Get().myModule);
    heightMapBuilder.SetDestNoiseMap (heightMap);
    heightMapBuilder.SetDestSize (CHUNK_SIZE_X * 3, CHUNK_SIZE_Z * 3);


    for (int z = -DEFAULT_VOXEL_RADIUS_RENDER_DISTANCE; z < DEFAULT_VOXEL_RADIUS_RENDER_DISTANCE; z++)
        for (int x = -DEFAULT_VOXEL_RADIUS_RENDER_DISTANCE; x < DEFAULT_VOXEL_RADIUS_RENDER_DISTANCE; x++)
            chunks_.emplace_back(
                    glm::vec2(lastChucnkPosition_.x + x , lastChucnkPosition_.y + z),
                    (x + DEFAULT_VOXEL_RADIUS_RENDER_DISTANCE) + (z + DEFAULT_VOXEL_RADIUS_RENDER_DISTANCE) * DEFAULT_VOXEL_RADIUS_RENDER_DISTANCE);
}

void VoxelWorld::update() {
    std::cout << "Update" << std::endl;
    Timer timer;
    timer.start();

    glm::ivec2 newChunkPosition;

    newChunkPosition.x = camera_.getPosition().x / CHUNK_SIZE_X;
    newChunkPosition.y = camera_.getPosition().z / CHUNK_SIZE_Z;

    //std::cout << "newChunkPosition" << std::endl;
    //std::cout << "x[" << newChunkPosition.x << "]y[" << newChunkPosition.y << "]" << std::endl;

    std::list< Chunk > newChunk;
    if (lastChucnkPosition_ != newChunkPosition) {


        float f = 0.082f;
        heightMapBuilder.SetBounds ((newChunkPosition.x - DEFAULT_VOXEL_RADIUS_RENDER_DISTANCE - 1) * f,
                (newChunkPosition.x + DEFAULT_VOXEL_RADIUS_RENDER_DISTANCE + 2) * f,
                (newChunkPosition.y - DEFAULT_VOXEL_RADIUS_RENDER_DISTANCE - 1) * f,
                (newChunkPosition.y + DEFAULT_VOXEL_RADIUS_RENDER_DISTANCE + 2) * f);
        heightMapBuilder.Build ();
        renderer.SetSourceNoiseMap (heightMap);
        renderer.SetDestImage (image);
        renderer.Render ();

        glm::ivec2 test = lastChucnkPosition_ - newChunkPosition;
        //std::cout << "newChunkPosition" << std::endl;
        //std::cout << "x[" << test.x << "]y[" << test.y << "]" << std::endl;
        for(auto it = chunks_.begin(); it != chunks_.end();)
        {
            glm::ivec2 test2 = newChunkPosition - it->chunkPosition_;
            //std::cout << "test" << std::endl;
            //std::cout << "x[" << test2.x << "]y[" << test2.y << "]" << std::endl;
            if (test2.x < -DEFAULT_VOXEL_RADIUS_RENDER_DISTANCE
                || test2.x > DEFAULT_VOXEL_RADIUS_RENDER_DISTANCE
                || test2.y < -DEFAULT_VOXEL_RADIUS_RENDER_DISTANCE
                || test2.y > DEFAULT_VOXEL_RADIUS_RENDER_DISTANCE)
            {
                it->setRender(false);
            }
                ++it;
        }
        for (int z = -DEFAULT_VOXEL_RADIUS_RENDER_DISTANCE; z < DEFAULT_VOXEL_RADIUS_RENDER_DISTANCE; z++) {
            for (int x = -DEFAULT_VOXEL_RADIUS_RENDER_DISTANCE; x < DEFAULT_VOXEL_RADIUS_RENDER_DISTANCE; x++) {
                auto it = std::find_if(chunks_.begin(), chunks_.end(), [x, z, newChunkPosition](Chunk const &chunk)->bool {
                    return (chunk.chunkPosition_.x == newChunkPosition.x + x
                    && chunk.chunkPosition_.y == newChunkPosition.y + z);
                });
                if (it == chunks_.end()) {

                    auto voided = std::find_if(chunks_.begin(), chunks_.end(), [x, z, newChunkPosition](Chunk const &chunk)->bool {
                        return (!chunk.getRender());
                    });

                    if (voided != chunks_.end()) {
                        voided->reload(glm::vec2(newChunkPosition.x + x , newChunkPosition.y + z),
                                        (x + DEFAULT_VOXEL_RADIUS_RENDER_DISTANCE) + (z + DEFAULT_VOXEL_RADIUS_RENDER_DISTANCE) * DEFAULT_VOXEL_RADIUS_RENDER_DISTANCE);
                        voided->setRender(true);
                    }

                    //chunks_.emplace_back(
                    //        glm::vec2(newChunkPosition.x + x , newChunkPosition.y + z),
                    //        (x + DEFAULT_VOXEL_RADIUS_RENDER_DISTANCE) + (z + DEFAULT_VOXEL_RADIUS_RENDER_DISTANCE) * DEFAULT_VOXEL_RADIUS_RENDER_DISTANCE);
                }
            }
        }
        std::cout << "Size lst [" << chunks_.size() << "]" << std::endl;
        lastChucnkPosition_ = newChunkPosition;
    }
    timer.stop();

    std::cout << "Elapsed time : " << timer.count() << "ms" << std::endl;
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
        if (!camera_.getFrustum().isDebug() || camera_.getFrustum().pointIn(i.worldPosition_.x + (CHUNK_SIZE_X >> 1), i.worldPosition_.y + (CHUNK_SIZE_Y >> 1), i.worldPosition_.z + (CHUNK_SIZE_Z >> 1)))
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