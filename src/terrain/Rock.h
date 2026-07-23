#ifndef ROCK_H
#define ROCK_H

#include <glad/glad.h>
#include <vector>
#include <cmath>
#include "../math/Vec3.h"

struct RockVertex {
    Vec3 position;
    Vec3 color;
};

class Rock {
public:
    GLuint vao;
    GLuint vbo;
    GLuint ebo;
    unsigned int indexCount;

    Rock() : vao(0), vbo(0), ebo(0), indexCount(0) {}

    // generates a low poly irregular rock mesh
    void setup() {
        std::vector<RockVertex> vertices;
        std::vector<unsigned int> indices;

        // deformed octahedron for natural rock shape
        Vec3 basePositions[] = {
            Vec3( 0.0f,  0.6f,  0.0f),   // top
            Vec3( 0.5f,  0.1f,  0.3f),   // front right
            Vec3(-0.4f,  0.15f, 0.45f),  // front left
            Vec3(-0.55f, 0.0f, -0.3f),   // back left
            Vec3( 0.35f, 0.05f,-0.5f),   // back right
            Vec3( 0.0f, -0.3f,  0.0f)    // bottom
        };

        // grey rock colors with slight variation
        Vec3 colors[] = {
            Vec3(0.48f, 0.46f, 0.44f),
            Vec3(0.42f, 0.40f, 0.38f),
            Vec3(0.45f, 0.43f, 0.40f),
            Vec3(0.40f, 0.38f, 0.36f),
            Vec3(0.44f, 0.42f, 0.40f),
            Vec3(0.38f, 0.36f, 0.34f)
        };

        for (int i = 0; i < 6; ++i)
            vertices.push_back({ basePositions[i], colors[i] });

        // top faces
        unsigned int topFaces[] = {
            0, 1, 2,
            0, 2, 3,
            0, 3, 4,
            0, 4, 1
        };

        // bottom faces
        unsigned int bottomFaces[] = {
            5, 2, 1,
            5, 3, 2,
            5, 4, 3,
            5, 1, 4
        };

        for (int i = 0; i < 12; ++i)
            indices.push_back(topFaces[i]);
        for (int i = 0; i < 12; ++i)
            indices.push_back(bottomFaces[i]);

        indexCount = (unsigned int)indices.size();

        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);

        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(RockVertex), vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(RockVertex), (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(RockVertex), (void*)(sizeof(Vec3)));
        glEnableVertexAttribArray(1);

        glBindVertexArray(0);
    }

    void draw() const {
        if (vao != 0 && indexCount > 0) {
            glBindVertexArray(vao);
            glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
        }
    }

    void cleanup() {
        if (vao != 0)
            glDeleteVertexArrays(1, &vao);
        if (vbo != 0)
            glDeleteBuffers(1, &vbo);
        if (ebo != 0)
            glDeleteBuffers(1, &ebo);
        vao = vbo = ebo = 0;
        indexCount = 0;
    }
};

#endif
