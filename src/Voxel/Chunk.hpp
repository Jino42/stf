#pragma once

#include <NTL.hpp>

#define CHUNK_SIZE_X 16
#define CHUNK_SIZE_Y 256
#define CHUNK_SIZE_Z 16
#define CHUNK_TOTAL_SIZE (CHUNK_SIZE_X * CHUNK_SIZE_Y * CHUNK_SIZE_Z)

struct PointVertex {
    glm::ivec3 position3;
    unsigned int color;
};

class Chunk {
public:
    Chunk(glm::vec3 position, int id);

    void render();
private:
    int id_;
    glm::vec3 position_;
    unsigned int VBO;
    unsigned int VAO;
    PointVertex array_[CHUNK_TOTAL_SIZE];
};