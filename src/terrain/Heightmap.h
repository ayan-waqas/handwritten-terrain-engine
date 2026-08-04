#ifndef HEIGHTMAP_H
#define HEIGHTMAP_H

#include <vector>
#include <cmath>
#include <iostream>
#include "../noise/PerlinNoise.h"
#include "Mesh.h"

class Heightmap {
public:
    int width;
    int depth;
    std::vector<float> heights;

    Heightmap(int w = 33, int d = 33) : width(w), depth(d), heights(w * d, 0.0f) {}

    float getHeight(int x, int z) const {
        if (x < 0)
            x = 0;
        if (x >= width)
            x = width - 1;
        if (z < 0)
            z = 0;
        if (z >= depth)
            z = depth - 1;
        return heights[z * width + x];
    }

    void setHeight(int x, int z, float val) {
        if (x >= 0 && x < width && z >= 0 && z < depth)
            heights[z * width + x] = val;
    }

    // Thermal & Hydraulic erosion pass to carve natural river channels and scree slopes
    void applyErosion(int iterations = 2) {
        float talusThreshold = 0.80f;

        for (int iter = 0; iter < iterations; ++iter) {
            for (int z = 1; z < depth - 1; ++z) {
                for (int x = 1; x < width - 1; ++x) {
                    int idx = z * width + x;
                    float h = heights[idx];

                    int lowestIdx = idx;
                    float minH = h;

                    int neighbors[4] = {
                        z * width + (x - 1),
                        z * width + (x + 1),
                        (z - 1) * width + x,
                        (z + 1) * width + x
                    };

                    for (int n = 0; n < 4; ++n) {
                        if (heights[neighbors[n]] < minH) {
                            minH = heights[neighbors[n]];
                            lowestIdx = neighbors[n];
                        }
                    }

                    float diff = h - minH;
                    if (diff > talusThreshold) {
                        float delta = (diff - talusThreshold) * 0.25f;
                        heights[idx] -= delta;
                        heights[lowestIdx] += delta;
                    }
                }
            }
        }
    }

    void fixBorderHeights(float worldOffsetX, float worldOffsetZ, const PerlinNoise& noiseGen) {
        for (int z = 0; z < depth; ++z) {
            for (int x = 0; x < width; ++x) {
                if (x == 0 || x == width - 1 || z == 0 || z == depth - 1) {
                    float wx = worldOffsetX + (float)x;
                    float wz = worldOffsetZ + (float)z;
                    heights[z * width + x] = noiseGen.terrainHeight(wx, wz);
                }
            }
        }
    }

    // seamless mesh with normals computed directly from eroded heightmap grid
    void buildMeshSeamless(Mesh& mesh, float worldOffsetX, float worldOffsetZ, const PerlinNoise& noiseGen) const {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;

        for (int z = 0; z < depth; ++z) {
            for (int x = 0; x < width; ++x) {
                Vertex v;
                v.position = Vec3((float)x, getHeight(x, z), (float)z);

                float wx = worldOffsetX + (float)x;
                float wz = worldOffsetZ + (float)z;

                // Compute normals directly from eroded heightmap grid so lighting catches every carved gully
                if (x == 0 || x == width - 1 || z == 0 || z == depth - 1) {
                    float hL = noiseGen.terrainHeight(wx - 1.0f, wz);
                    float hR = noiseGen.terrainHeight(wx + 1.0f, wz);
                    float hD = noiseGen.terrainHeight(wx, wz - 1.0f);
                    float hU = noiseGen.terrainHeight(wx, wz + 1.0f);
                    v.normal = Vec3(hL - hR, 2.0f, hD - hU).normalize();
                }
                else {
                    float hL = getHeight(x - 1, z);
                    float hR = getHeight(x + 1, z);
                    float hD = getHeight(x, z - 1);
                    float hU = getHeight(x, z + 1);
                    v.normal = Vec3(hL - hR, 2.0f, hD - hU).normalize();
                }

                // Biome noise scale set to 0.022 so Forest, Desert, and Tundra biomes are visible nearby
                float biomeRaw = noiseGen.fBm(wx * 0.022f, wz * 0.022f, 3, 0.5f, 2.0f);
                float bVal = biomeRaw * 2.5f + 0.5f;
                if (bVal < 0.0f)
                    bVal = 0.0f;
                if (bVal > 1.0f)
                    bVal = 1.0f;
                v.biome = bVal;

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
