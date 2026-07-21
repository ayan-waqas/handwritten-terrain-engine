#ifndef HEIGHTMAP_H
#define HEIGHTMAP_H

#include <vector>
#include "../noise/PerlinNoise.h"

class Heightmap {
public:
    int width;
    int depth;
    std::vector<float> heights; // 1d array storing 2d grid heights

    Heightmap(int w = 128, int d = 128) : width(w), depth(d), heights(w * d, 0.0f) {}

    // generates height values using fbm noise
    void generate(const PerlinNoise& noiseGen, float scale = 0.05f, int octaves = 4, float persistence = 0.5f, float lacunarity = 2.0f, float heightMultiplier = 3.0f) {
        for (int z = 0; z < depth; ++z) {
            for (int x = 0; x < width; ++x) {
                // sample fbm noise at grid coordinate
                float sampleX = (float)x * scale;
                float sampleZ = (float)z * scale;
                float h = noiseGen.fBm(sampleX, sampleZ, octaves, persistence, lacunarity);

                // store scaled height value
                heights[z * width + x] = h * heightMultiplier;
            }
        }
    }

    // helper to get height at grid coordinate (x, z)
    float getHeight(int x, int z) const {
        if (x >= 0 && x < width && z >= 0 && z < depth) {
            return heights[z * width + x];
        }
        return 0.0f;
    }
};

#endif
