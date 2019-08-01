#pragma once

#include <NTL.hpp>

#define CHUNK_SIZE_X 16
#define CHUNK_SIZE_Z 16
#define CHUNK_SIZE_Y 256
#define CHUNK_TOTAL_SIZE (CHUNK_SIZE_Z * CHUNK_SIZE_Y * CHUNK_SIZE_X)

struct PointVertex {
    glm::ivec3 position3;
    unsigned int color;
};

class Chunk {
public:
    Chunk(glm::ivec2 chunkPosition, int id);

    void render();

    glm::ivec2 chunkPosition_;
    glm::vec3 worldPosition_;

private:
    int id_;
    unsigned int VBO;
    unsigned int VAO;
    unsigned int finalSizeChunk_;
    PointVertex array_[CHUNK_TOTAL_SIZE];
};