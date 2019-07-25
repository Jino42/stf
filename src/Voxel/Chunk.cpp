#include "Chunk.hpp"
#include <Engine/MainGraphic.hpp>
#include <Voxel/VoxelWorld.hpp>
#include <Engine/ShaderManager.hpp>
#include <noiseutils.h>
#include <iostream>

Chunk::Chunk(glm::vec3 position, int id) :
    id_(id),
    position_(position) {

    VoxelWorld &vw = VoxelWorld::Get();

    vw.heightMapBuilder.SetBounds (position.x / 16.0f, position.x / 16.0f + 1.0f, position.z / 16.0f, position.z / 16.0f + 1.0f);
    vw.heightMapBuilder.Build ();
    vw.renderer.SetSourceNoiseMap (vw.heightMap);
    vw.renderer.SetDestImage (vw.image);
    vw.renderer.Render ();

    utils::WriterBMP writer;
    writer.SetSourceImage (vw.image);
    writer.SetDestFilename ("tutorial.bmp");
    writer.WriteDestFile ();

    int i = 0;
    for (int z = 0; z < CHUNK_SIZE; z++) {
        for (int x = 0; x < CHUNK_SIZE; x++) {


            int south = vw.image.GetValue(x + 1, z).blue / 16.0f;
            int north = vw.image.GetValue(x - 1, z).blue / 16.0f;
            int east = vw.image.GetValue(x, z + 1).blue / 16.0f;
            int west = vw.image.GetValue(x, z - 1).blue / 16.0f;

            for (int y = vw.image.GetValue(x, z).blue / 16.0f; y ; y--) {
                array_[i].position3.x = x;
                array_[i].position3.y = y;
                array_[i].position3.z = z;
                i++;

                if ((!x || north >= y)
                    && (x + 1 == CHUNK_SIZE || south >= y)
                    && (!z || west >= y)
                    && (z + 1 == CHUNK_SIZE || east >= y))
                {
                    break ;
                }
            }
        }
    }
    std::cout << i << std::endl;

/*
    for (int y = 0; y < CHUNK_SIZE; y++) {
        for (int x = 0; x < CHUNK_SIZE; x++) {
            for (int z = 0; z < CHUNK_SIZE; z++) {
                if (sqrt((float) (x-CHUNK_SIZE/2)*(x-CHUNK_SIZE/2) + (y-CHUNK_SIZE/2)*(y-CHUNK_SIZE/2) + (z-CHUNK_SIZE/2)*(z-CHUNK_SIZE/2)) <= CHUNK_SIZE/2)
                {
                    array_[i].position3.x = x;
                    array_[i].position3.y = y;
                    array_[i].position3.z = z;

                    i++;
                }
            }
        }
    }
*/
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, i * sizeof(PointVertex), &array_[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_INT, GL_FALSE, sizeof(PointVertex), (void *)offsetof(PointVertex, position3));
    glVertexAttribPointer(1, 1, GL_UNSIGNED_INT, GL_FALSE, sizeof(PointVertex), (void *)offsetof(PointVertex, color));

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
}


void Chunk::render() {
    Shader &shader = ShaderManager::Get().getShader("voxel");

    shader.setInt("positionX", position_.x);
    shader.setInt("positionZ", position_.z);
    shader.setInt("id", id_);

    glBindVertexArray(VAO);
    glDrawArrays(GL_POINTS, 0, CHUNK_TOTAL_SIZE);
    glBindVertexArray(0);
}