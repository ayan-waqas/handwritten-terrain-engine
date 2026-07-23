#ifndef GRASS_H
#define GRASS_H

#include <glad/glad.h>
#include <vector>
#include <cmath>
#include "../math/Vec3.h"

struct GrassVertex {
    Vec3 position;
    Vec3 color;
    float windWeight;
};

class Grass {
public:
    GLuint vao;
    GLuint vbo;
    GLuint ebo;
    unsigned int indexCount;

    Grass() : vao(0), vbo(0), ebo(0), indexCount(0) {}

    void setup() {
        std::vector<GrassVertex> vertices;
        std::vector<unsigned int> indices;

        Vec3 rootColor = Vec3(0.08f, 0.38f, 0.10f);
        Vec3 midColor  = Vec3(0.14f, 0.46f, 0.16f);
        Vec3 tipColor  = Vec3(0.22f, 0.58f, 0.20f);

        float w = 0.20f;   // blade width
        float h = 0.24f;   // small natural grass blade height (24cm)
        float pi = 3.14159265f;

        // 3 rotated curved blade planes (star tuft with forward bend)
        for (int p = 0; p < 3; ++p) {
            float angle = (float)p * (pi / 3.0f);
            float cosA = std::cos(angle);
            float sinA = std::sin(angle);

            unsigned int baseIdx = (unsigned int)vertices.size();

            // 5-vertex curved blade ribbon
            Vec3 p0 = Vec3(-w * 0.5f * cosA, 0.0f, -w * 0.5f * sinA);
            Vec3 p1 = Vec3( w * 0.5f * cosA, 0.0f,  w * 0.5f * sinA);

            float bendMid = 0.03f;
            Vec3 p2 = Vec3(-w * 0.35f * cosA + bendMid * sinA, h * 0.5f, -w * 0.35f * sinA + bendMid * cosA);
            Vec3 p3 = Vec3( w * 0.35f * cosA + bendMid * sinA, h * 0.5f,  w * 0.35f * sinA + bendMid * cosA);

            float bendTip = 0.07f;
            Vec3 p4 = Vec3(bendTip * sinA, h, bendTip * cosA);

            vertices.push_back({ p0, rootColor, 0.0f });
            vertices.push_back({ p1, rootColor, 0.0f });
            vertices.push_back({ p2, midColor,  0.5f });
            vertices.push_back({ p3, midColor,  0.5f });
            vertices.push_back({ p4, tipColor,  1.0f });

            indices.push_back(baseIdx + 0);
            indices.push_back(baseIdx + 1);
            indices.push_back(baseIdx + 3);

            indices.push_back(baseIdx + 3);
            indices.push_back(baseIdx + 2);
            indices.push_back(baseIdx + 0);

            indices.push_back(baseIdx + 2);
            indices.push_back(baseIdx + 3);
            indices.push_back(baseIdx + 4);
        }

        indexCount = (unsigned int)indices.size();

        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);

        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GrassVertex), vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GrassVertex), (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GrassVertex), (void*)(sizeof(Vec3)));
        glEnableVertexAttribArray(1);

        glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(GrassVertex), (void*)(sizeof(Vec3) * 2));
        glEnableVertexAttribArray(2);

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
