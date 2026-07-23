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
};

#endif
