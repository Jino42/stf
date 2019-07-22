#include "Chunk.hpp"
#include <Engine/MainGraphic.hpp>
#include <Voxel/VoxelWorld.hpp>
#include <Engine/ShaderManager.hpp>

Chunk::Chunk(glm::vec3 position, int id) :
    id_(id),
    position_(position) {

    for (int y = 0; y < CHUNK_SIZE_Y; y++) {
        for (int x = 0; x < CHUNK_SIZE_X; x++) {
            for (int z = 0; z < CHUNK_SIZE_Z; z++) {
//                array_[x + y * CHUNK_SIZE_X + z * CHUNK_SIZE_X * CHUNK_SIZE_Y].position = 0;
//                array_[x + y * CHUNK_SIZE_X + z * CHUNK_SIZE_X * CHUNK_SIZE_Y].position += (x << 16);
//                array_[x + y * CHUNK_SIZE_X + z * CHUNK_SIZE_X * CHUNK_SIZE_Y].position += (y << 8);
//                array_[x + y * CHUNK_SIZE_X + z * CHUNK_SIZE_X * CHUNK_SIZE_Y].position += z;

                array_[x + y * CHUNK_SIZE_X + z * CHUNK_SIZE_X * CHUNK_SIZE_Y].position3.x = x;
                array_[x + y * CHUNK_SIZE_X + z * CHUNK_SIZE_X * CHUNK_SIZE_Y].position3.y = y;
                array_[x + y * CHUNK_SIZE_X + z * CHUNK_SIZE_X * CHUNK_SIZE_Y].position3.z = z;
            }
        }
    }
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, CHUNK_TOTAL_SIZE * sizeof(PointVertex), &array_[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_INT, GL_FALSE, sizeof(PointVertex), (void *)offsetof(PointVertex, position3));
    glVertexAttribPointer(1, 1, GL_UNSIGNED_INT, GL_FALSE, sizeof(PointVertex), (void *)offsetof(PointVertex, color));



    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
}

void Chunk::render()
{
    Shader &shader = ShaderManager::Get().getShader("voxel");
    shader.activate();
    shader.setMat4("projection", MainGraphic::Get().getProjectionMatrix());
    shader.setMat4("view", MainGraphic::Get().getViewMatrix());

    shader.setInt("positionX", position_.x);
    shader.setInt("positionZ", position_.z);
    shader.setInt("id", id_);

    glBindVertexArray(VAO);
    glDrawArrays(GL_POINTS, 0, CHUNK_TOTAL_SIZE);
    glBindVertexArray(0);
}