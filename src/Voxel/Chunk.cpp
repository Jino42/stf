#include "Chunk.hpp"
#include <Engine/MainGraphic.hpp>
#include <Voxel/VoxelWorld.hpp>
#include <Engine/ShaderManager.hpp>
#include <noiseutils.h>
#include <iostream>
#include <Engine/Time.hpp>

Chunk::Chunk(glm::ivec2 chunkPosition, int id) :
    id_(id),
    render_(true),
    chunkPosition_(chunkPosition),
    worldPosition_(glm::vec3(chunkPosition_.x * CHUNK_SIZE_X, 0.f, chunkPosition_.y * CHUNK_SIZE_Z)),
    finalSizeChunk_(0) {

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    //glBufferData(GL_ARRAY_BUFFER, i * sizeof(PointVertex), &array_[0], GL_STATIC_DRAW);
    glBufferData(GL_ARRAY_BUFFER, CHUNK_TOTAL_SIZE * sizeof(PointVertex), nullptr, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_INT, GL_FALSE, sizeof(PointVertex), (void *)offsetof(PointVertex, position3));
    glVertexAttribPointer(1, 1, GL_UNSIGNED_INT, GL_FALSE, sizeof(PointVertex), (void *)offsetof(PointVertex, color));

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    reload(chunkPosition, id);
}

void Chunk::reload(glm::ivec2 chunkPosition, int id) {

    std::cout << "reload" << std::endl;
    Timer timer;
    timer.start();
    id_ = id;
    chunkPosition_ = chunkPosition;
    worldPosition_ = glm::vec3(chunkPosition_.x * CHUNK_SIZE_X, 0.f, chunkPosition_.y * CHUNK_SIZE_Z);

    VoxelWorld &vw = VoxelWorld::Get();

    float f = 0.082f;
    vw.heightMapBuilder.SetBounds ((chunkPosition.x - 1) * f, (chunkPosition.x + 2) * f, (chunkPosition.y - 1) * f, (chunkPosition.y + 2) * f);
    vw.heightMapBuilder.Build ();
    vw.renderer.SetSourceNoiseMap (vw.heightMap);
    vw.renderer.SetDestImage (vw.image);
    vw.renderer.Render ();

    int i = 0;
    timer.stop();
    std::cout << "Image Write time : " << timer.count() << "ms" << std::endl;

    for (int z = 0; z < CHUNK_SIZE_Z; z++) {
        for (int x = 0; x < CHUNK_SIZE_X; x++) {

            int south = vw.image.GetValue(CHUNK_SIZE_X + x + 1, CHUNK_SIZE_Z + z).blue;
            int north = vw.image.GetValue(CHUNK_SIZE_X + x - 1, CHUNK_SIZE_Z + z).blue;
            int east = vw.image.GetValue(CHUNK_SIZE_X + x, CHUNK_SIZE_Z + z + 1).blue;
            int west = vw.image.GetValue(CHUNK_SIZE_X + x, CHUNK_SIZE_Z + z - 1).blue;

            for (int y = vw.image.GetValue(CHUNK_SIZE_X + x, CHUNK_SIZE_Z + z).blue; y ; y--) {
                array_[i].position3.x = x;
                array_[i].position3.y = y;
                array_[i].position3.z = z;
                i++;

                if ((north >= y)
                    && (south >= y)
                    && (west >= y)
                    && (east >= y))
                {
                    break ;
                }
            }
        }
    }

    finalSizeChunk_ = i;
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(PointVertex) * finalSizeChunk_, reinterpret_cast<void *>(&array_[0]));

    std::cout << "end_reload" << std::endl;

}

void Chunk::render() {
    if (!render_)
        return ;
    Shader &shader = ShaderManager::Get().getShader("voxel");

    shader.setInt("positionX", worldPosition_.x);
    shader.setInt("positionZ", worldPosition_.z);
    shader.setInt("id", id_);

    glBindVertexArray(VAO);
    glDrawArrays(GL_POINTS, 0, finalSizeChunk_);
    glBindVertexArray(0);
}

void Chunk::setRender(bool b) {
    render_ = b;
}
bool Chunk::getRender() const {
    return render_;
}