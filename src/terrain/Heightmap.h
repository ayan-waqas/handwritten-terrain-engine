#ifndef HEIGHTMAP_H
#define HEIGHTMAP_H

#include <vector>
#include "../noise/PerlinNoise.h"
#include "Mesh.h"

class Heightmap {
public:
    int width;
    int depth;
    std::vector<float> heights; // 1d array storing 2d grid heights

    Heightmap(int w = 128, int d = 128) : width(w), depth(d), heights(w * d, 0.0f) {}

    // generates height values using fbm noise
    void generate(const PerlinNoise& noiseGen, float scale = 0.04f, int octaves = 4, float persistence = 0.5f, float lacunarity = 2.0f, float heightMultiplier = 18.0f) {
        for (int z = 0; z < depth; ++z) {
            for (int x = 0; x < width; ++x) {
                // sample ridged fbm noise at grid coordinate
                float sampleX = (float)x * scale;
                float sampleZ = (float)z * scale;
                float h = noiseGen.ridgedfBm(sampleX, sampleZ, octaves, persistence, lacunarity);

                // store scaled height value
                heights[z * width + x] = h * heightMultiplier;
            }
        }
    }

    // helper to get height at grid coordinate (x, z)
    float getHeight(int x, int z) const {
        if (x >= 0 && x < width && z >= 0 && z < depth)
            return heights[z * width + x];
        return 0.0f;
    }

    // builds mesh vertices and indices from heightmap grid
    void buildMesh(Mesh& mesh) const {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;

        // generate vertices and normals
        for (int z = 0; z < depth; ++z) {
            for (int x = 0; x < width; ++x) {
                Vertex v;
                v.position = Vec3((float)x - width / 2.0f, getHeight(x, z), (float)z - depth / 2.0f);

                // compute surface normal from neighbor heights
                float hL = getHeight(x - 1, z);
                float hR = getHeight(x + 1, z);
                float hD = getHeight(x, z - 1);
                float hU = getHeight(x, z + 1);
                v.normal = Vec3(hL - hR, 2.0f, hD - hU).normalize();

                vertices.push_back(v);
            }
        }

        // generate triangle indices for grid quads
        for (int z = 0; z < depth - 1; ++z) {
            for (int x = 0; x < width - 1; ++x) {
                unsigned int i0 = z * width + x;
                unsigned int i1 = z * width + (x + 1);
                unsigned int i2 = (z + 1) * width + x;
                unsigned int i3 = (z + 1) * width + (x + 1);

                // triangle 1
                indices.push_back(i0);
                indices.push_back(i2);
                indices.push_back(i1);

                // triangle 2
                indices.push_back(i1);
                indices.push_back(i2);
                indices.push_back(i3);
            }
        }

        mesh.setup(vertices, indices);
    }

    // builds seamless mesh vertices and normals across chunk boundaries using ridged noise
    void buildMeshSeamless(Mesh& mesh, float worldOffsetX, float worldOffsetZ, const PerlinNoise& noiseGen) const {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;

        for (int z = 0; z < depth; ++z) {
            for (int x = 0; x < width; ++x) {
                Vertex v;
                v.position = Vec3((float)x, getHeight(x, z), (float)z);

                // sample world noise for continuous neighbor normals across chunk borders
                float wx = worldOffsetX + (float)x;
                float wz = worldOffsetZ + (float)z;

                float hL = noiseGen.ridgedfBm((wx - 1.0f) * 0.04f, wz * 0.04f, 4, 0.5f, 2.0f) * 18.0f;
                float hR = noiseGen.ridgedfBm((wx + 1.0f) * 0.04f, wz * 0.04f, 4, 0.5f, 2.0f) * 18.0f;
                float hD = noiseGen.ridgedfBm(wx * 0.04f, (wz - 1.0f) * 0.04f, 4, 0.5f, 2.0f) * 18.0f;
                float hU = noiseGen.ridgedfBm(wx * 0.04f, (wz + 1.0f) * 0.04f, 4, 0.5f, 2.0f) * 18.0f;

                v.normal = Vec3(hL - hR, 2.0f, hD - hU).normalize();
                vertices.push_back(v);
            }
        }

        for (int z = 0; z < depth - 1; ++z) {
            for (int x = 0; x < width - 1; ++x) {
                unsigned int i0 = z * width + x;
                unsigned int i1 = z * width + (x + 1);
                unsigned int i2 = (z + 1) * width + x;
                unsigned int i3 = (z + 1) * width + (x + 1);

                indices.push_back(i0);
                indices.push_back(i2);
                indices.push_back(i1);

                indices.push_back(i1);
                indices.push_back(i2);
                indices.push_back(i3);
            }
        }

        mesh.setup(vertices, indices);
    }
};

#endif
