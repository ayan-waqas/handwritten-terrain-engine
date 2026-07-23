#ifndef WATER_H
#define WATER_H

#include <glad/glad.h>
#include <vector>
#include "../math/Vec3.h"

class Water {
public:
    GLuint vao;
    GLuint vbo;

    Water() : vao(0), vbo(0) {}

    // sets up flat water quad mesh at y = -4.5
    void setup(float size = 150.0f) {
        float half = size / 2.0f;
        float waterHeight = -4.5f;

        float vertices[] = {
            -half, waterHeight, -half,
             half, waterHeight, -half,
             half, waterHeight,  half,

            -half, waterHeight, -half,
             half, waterHeight,  half,
            -half, waterHeight,  half
        };

        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);

        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glBindVertexArray(0);
    }

    void draw() const {
        if (vao != 0) {
            glBindVertexArray(vao);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            glBindVertexArray(0);
        }
    }

    void cleanup() {
        if (vao != 0)
            glDeleteVertexArrays(1, &vao);
        if (vbo != 0)
            glDeleteBuffers(1, &vbo);
        vao = 0;
        vbo = 0;
    }
};

#endif
