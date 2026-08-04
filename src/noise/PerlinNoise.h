#ifndef PERLIN_NOISE_H
#define PERLIN_NOISE_H

#include <vector>
#include <cmath>
#include <numeric>
#include <algorithm>
#include <random>

class PerlinNoise {
private:
    std::vector<int> p; // permutation table for randomness

    // fade function to smooth out grid edges
    static float fade(float t) {
        return t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f);
    }

    // linear interpolation helper
    static float lerp(float t, float a, float b) {
        return a + t * (b - a);
    }

    // calculates dot product between random gradient and distance vector
    static float grad(int hash, float x, float y) {
        int h = hash & 7;
        float u = h < 4 ? x : y;
        float v = h < 4 ? y : x;
        return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
    }

public:
    // constructor sets up shuffled permutation table
    PerlinNoise(unsigned int seed = 2026) {
        p.resize(256);
        std::iota(p.begin(), p.end(), 0);

        std::default_random_engine engine(seed);
        std::shuffle(p.begin(), p.end(), engine);

        // duplicate array to prevent out of bounds when wrapping
        p.insert(p.end(), p.begin(), p.end());
    }

    // gets 2d perlin noise value
    float noise(float x, float y) const {
        int X = (int)std::floor(x) & 255;
        int Y = (int)std::floor(y) & 255;

        x -= std::floor(x);
        y -= std::floor(y);

        float u = fade(x);
        float v = fade(y);

        int A = p[X] + Y;
        int B = p[X + 1] + Y;

        return lerp(v, lerp(u, grad(p[A], x, y),
                               grad(p[B], x - 1, y)),
                       lerp(u, grad(p[A + 1], x, y - 1),
                               grad(p[B + 1], x - 1, y - 1)));
    }

    // layers multiple noise passes together for natural terrain look
    float fBm(float x, float y, int octaves, float persistence, float lacunarity) const {
        float total = 0.0f;
        float frequency = 1.0f;
        float amplitude = 1.0f;
        float maxValue = 0.0f;

        for (int i = 0; i < octaves; ++i) {
            total += noise(x * frequency, y * frequency) * amplitude;
            maxValue += amplitude;
            amplitude *= persistence;
            frequency *= lacunarity;
        }

        return total / maxValue;
    }

    // ridged fbm noise for sharp jagged mountain ridgelines
    float ridgedfBm(float x, float y, int octaves, float persistence, float lacunarity) const {
        float total = 0.0f;
        float frequency = 1.0f;
        float amplitude = 1.0f;
        float maxValue = 0.0f;

        for (int i = 0; i < octaves; ++i) {
            float n = noise(x * frequency, y * frequency);
            n = 1.0f - std::abs(n);
            n = n * n;

            total += n * amplitude;
            maxValue += amplitude;
            amplitude *= persistence;
            frequency *= lacunarity;
        }

        return total / maxValue;
    }

    // Geographic landforms: Continental noise partitions world into Plains, Canyons, and Alpine Ranges
    float terrainHeight(float wx, float wz) const {
        float cx = wx * 0.005f;
        float cz = wz * 0.005f;

        // Continentalness noise (-1.0 to 1.0)
        float continental = fBm(cx, cz, 3, 0.5f, 2.0f);

        // Domain warping for natural mountain ridges
        float sx = wx * 0.035f;
        float sz = wz * 0.035f;
        float warpX = fBm(sx * 0.4f, sz * 0.4f, 2, 0.5f, 2.0f) * 0.6f;
        float warpZ = fBm((sx + 4.2f) * 0.4f, (sz + 2.8f) * 0.4f, 2, 0.5f, 2.0f) * 0.6f;

        // 1. Lush Rolling Plains / Meadows (low continentalness)
        float plainsNoise = fBm(wx * 0.012f, wz * 0.012f, 4, 0.5f, 2.0f);
        float plainsHeight = (plainsNoise * 0.5f + 0.5f) * 6.0f - 1.5f;

        // 2. Carved River Canyons / Terraced Plateaus (mid continentalness)
        float canyonNoise = fBm(wx * 0.025f, wz * 0.025f, 4, 0.5f, 2.0f);
        float canyonStep = std::floor(canyonNoise * 4.0f) * 2.5f;
        float canyonHeight = canyonStep + canyonNoise * 2.0f + 1.0f;

        // 3. Alpine Mountain Ranges (high continentalness)
        float ridge = ridgedfBm(sx + warpX, sz + warpZ, 5, 0.48f, 2.0f);
        float mountainHeight = (ridge - 0.20f) * 26.0f;

        // Blend between Plains (cont < -0.05), Canyons (-0.05 to 0.20), and Alpine Peaks (cont >= 0.20)
        if (continental < -0.05f) {
            float t = std::clamp((continental + 0.5f) / 0.45f, 0.0f, 1.0f);
            return lerp(t, plainsHeight, plainsHeight * 0.7f + canyonHeight * 0.3f);
        }
        else if (continental < 0.20f) {
            float t = (continental + 0.05f) / 0.25f;
            return lerp(t, canyonHeight, mountainHeight * 0.8f);
        }
        else {
            return mountainHeight;
        }
    }
};

#endif
