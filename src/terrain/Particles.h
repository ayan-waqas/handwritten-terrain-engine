#ifndef PARTICLES_H
#define PARTICLES_H

#include <glad/glad.h>
#include <vector>
#include <cmath>
#include <cstdlib>
#include "../math/Vec3.h"

struct ParticleVertex {
    Vec3 position;
    Vec3 color;
    float phase;
    float size;
};

class Particles {
public:
    GLuint vao;
    GLuint vbo;
    unsigned int particleCount;

    Particles() : vao(0), vbo(0), particleCount(0) {}

    void setup(int count = 600) {
        particleCount = (unsigned int)count;
        std::vector<ParticleVertex> vertices;

        Vec3 goldGlow = Vec3(1.00f, 0.85f, 0.45f);
        Vec3 cyanGlow = Vec3(0.45f, 0.88f, 0.95f);

        for (int i = 0; i < count; ++i) {
            float rx = ((float)rand() / (float)RAND_MAX) * 200.0f - 100.0f;
            float ry = ((float)rand() / (float)RAND_MAX) * 25.0f - 5.0f;
            float rz = ((float)rand() / (float)RAND_MAX) * 200.0f - 100.0f;

            float phase = ((float)rand() / (float)RAND_MAX) * 6.28318f;
            float ptSize = 6.0f + ((float)rand() / (float)RAND_MAX) * 10.0f;

            Vec3 col = (i % 3 == 0) ? cyanGlow : goldGlow;
            vertices.push_back({ Vec3(rx, ry, rz), col, phase, ptSize });
        }

        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);

        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(ParticleVertex), vertices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(ParticleVertex), (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(ParticleVertex), (void*)(sizeof(Vec3)));
        glEnableVertexAttribArray(1);

        glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(ParticleVertex), (void*)(sizeof(Vec3) * 2));
        glEnableVertexAttribArray(2);

        glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(ParticleVertex), (void*)(sizeof(Vec3) * 2 + sizeof(float)));
        glEnableVertexAttribArray(3);

        glBindVertexArray(0);
    }

    void draw() const {
        if (vao != 0 && particleCount > 0) {
            glBindVertexArray(vao);
            glDrawArrays(GL_POINTS, 0, particleCount);
            glBindVertexArray(0);
        }
    }

    void cleanup() {
        if (vao != 0)
            glDeleteVertexArrays(1, &vao);
        if (vbo != 0)
            glDeleteBuffers(1, &vbo);
        vao = vbo = 0;
        particleCount = 0;
    }
};

#endif
