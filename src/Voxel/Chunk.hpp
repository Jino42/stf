#pragma once

#include <NTL.hpp>

#define CHUNK_SIZE 16
#define CHUNK_TOTAL_SIZE (CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE)

struct PointVertex {
    glm::ivec3 position3;
    unsigned int color;
};

class Chunk {
public:
    Chunk(glm::vec3 position, int id);

    void render();

    glm::vec3 position_;

private:
    int id_;
    unsigned int VBO;
    unsigned int VAO;
    unsigned int finalSizeChunk_;
    PointVertex array_[CHUNK_TOTAL_SIZE];
};