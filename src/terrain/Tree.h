#ifndef TREE_H
#define TREE_H

#include <glad/glad.h>
#include <vector>
#include <cmath>
#include "../math/Vec3.h"
#include "OBJLoader.h"

struct TreeVertex {
    Vec3 position;
    Vec3 color;
};

class Tree {
public:
    GLuint vao;
    GLuint vbo;
    GLuint ebo;
    unsigned int indexCount;

    Tree() : vao(0), vbo(0), ebo(0), indexCount(0) {}

    void setup() {
        std::vector<TreeVertex> vertices;
        std::vector<unsigned int> indices;

        std::vector<OBJLoader::OBJVertex> objVerts;
        std::vector<unsigned int> objIndices;

        bool loaded = OBJLoader::load("models/tree/Lowpoly_tree_sample.obj", objVerts, objIndices);
        if (!loaded)
            loaded = OBJLoader::load("../models/tree/Lowpoly_tree_sample.obj", objVerts, objIndices);

        if (loaded && objVerts.size() > 0) {
            float minY = 1e9f, maxY = -1e9f;
            float minX = 1e9f, maxX = -1e9f;
            float minZ = 1e9f, maxZ = -1e9f;

            for (int i = 0; i < (int)objVerts.size(); ++i) {
                Vec3 p = objVerts[i].position;
                if (p.y < minY) minY = p.y;
                if (p.y > maxY) maxY = p.y;
                if (p.x < minX) minX = p.x;
                if (p.x > maxX) maxX = p.x;
                if (p.z < minZ) minZ = p.z;
                if (p.z > maxZ) maxZ = p.z;
            }

            float centerX = (minX + maxX) * 0.5f;
            float centerZ = (minZ + maxZ) * 0.5f;
            float modelHeight = maxY - minY;
            float scale = 6.5f / (modelHeight > 0.001f ? modelHeight : 1.0f);

            Vec3 barkColor = Vec3(0.24f, 0.16f, 0.08f);
            Vec3 leafColorLower = Vec3(0.12f, 0.36f, 0.15f);
            Vec3 leafColorUpper = Vec3(0.20f, 0.48f, 0.22f);

            for (int i = 0; i < (int)objVerts.size(); ++i) {
                Vec3 rawP = objVerts[i].position;
                Vec3 pos = Vec3((rawP.x - centerX) * scale, (rawP.y - minY) * scale, (rawP.z - centerZ) * scale);
                float normalizedY = pos.y / 6.5f;

                Vec3 col;
                if (normalizedY < 0.25f)
                    col = barkColor;
                else
                    col = leafColorLower * (1.0f - normalizedY) + leafColorUpper * normalizedY;

                vertices.push_back({ pos, col });
            }
            indices = objIndices;
        }

        indexCount = (unsigned int)indices.size();

        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);

        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(TreeVertex), vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(TreeVertex), (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(TreeVertex), (void*)(sizeof(Vec3)));
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
