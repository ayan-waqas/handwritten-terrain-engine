#ifndef MESH_H
#define MESH_H

#include <glad/glad.h>
#include <vector>
#include "../math/Vec3.h"

// vertex struct holding position, normal, and biome value
struct Vertex {
    Vec3 position;
    Vec3 normal;
    float biome;
};

class Mesh {
public:
    GLuint vao;
    GLuint vbo;
    GLuint ebo;
    unsigned int indexCount;

    Mesh() : vao(0), vbo(0), ebo(0), indexCount(0) {}

    // sets up gpu buffers for mesh
    void setup(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices) {
        indexCount = (unsigned int)indices.size();

        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);

        glBindVertexArray(vao);

        // upload vertex buffer data
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

        // upload index buffer data
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

        // position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
        glEnableVertexAttribArray(0);

        // normal attribute
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
        glEnableVertexAttribArray(1);

        // biome attribute
        glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, biome));
        glEnableVertexAttribArray(2);

        glBindVertexArray(0);
    }

    // draws mesh using index buffer
    void draw() const {
        if (vao != 0 && indexCount > 0) {
            glBindVertexArray(vao);
            glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
        }
    }

    // cleans up gpu buffers
    void cleanup() {
        if (vao != 0)
            glDeleteVertexArrays(1, &vao);
        if (vbo != 0)
            glDeleteBuffers(1, &vbo);
        if (ebo != 0)
            glDeleteBuffers(1, &ebo);

        vao = 0;
        vbo = 0;
        ebo = 0;
        indexCount = 0;
    }
};

#endif
