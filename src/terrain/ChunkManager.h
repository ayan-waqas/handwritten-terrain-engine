#ifndef CHUNK_MANAGER_H
#define CHUNK_MANAGER_H

#include <vector>
#include <cmath>
#include "../noise/PerlinNoise.h"
#include "Heightmap.h"
#include "Mesh.h"
#include "../math/Mat4.h"

struct Chunk {
    int chunkX;
    int chunkZ;
    Heightmap heightmap;
    Mesh mesh;

    Chunk() : chunkX(0), chunkZ(0), heightmap(33, 33) {}
    Chunk(int cx, int cz) : chunkX(cx), chunkZ(cz), heightmap(33, 33) {}
};

class ChunkManager {
public:
    int chunkSize;
    int renderDistance;
    std::vector<Chunk> activeChunks;

    ChunkManager(int size = 32, int distance = 3)
        : chunkSize(size), renderDistance(distance) {}

    void update(const Vec3& cameraPos, const PerlinNoise& noiseGen) {
        int currentChunkX = (int)std::floor(cameraPos.x / chunkSize);
        int currentChunkZ = (int)std::floor(cameraPos.z / chunkSize);

        for (int cz = currentChunkZ - renderDistance; cz <= currentChunkZ + renderDistance; ++cz) {
            for (int cx = currentChunkX - renderDistance; cx <= currentChunkX + renderDistance; ++cx) {
                bool loaded = false;

                for (int i = 0; i < (int)activeChunks.size(); ++i) {
                    if (activeChunks[i].chunkX == cx && activeChunks[i].chunkZ == cz) {
                        loaded = true;
                        break;
                    }
                }

                if (!loaded) {
                    Chunk newChunk(cx, cz);
                    float worldOffsetX = (float)(cx * chunkSize);
                    float worldOffsetZ = (float)(cz * chunkSize);

                    // generate seamless heights and normals using ridged fbm noise
                    for (int z = 0; z < newChunk.heightmap.depth; ++z) {
                        for (int x = 0; x < newChunk.heightmap.width; ++x) {
                            float sampleX = (worldOffsetX + x) * 0.04f;
                            float sampleZ = (worldOffsetZ + z) * 0.04f;
                            float h = noiseGen.ridgedfBm(sampleX, sampleZ, 4, 0.5f, 2.0f);
                            newChunk.heightmap.heights[z * newChunk.heightmap.width + x] = h * 18.0f;
                        }
                    }

                    newChunk.heightmap.buildMeshSeamless(newChunk.mesh, worldOffsetX, worldOffsetZ, noiseGen);
                    activeChunks.push_back(newChunk);
                }
            }
        }

        for (int i = 0; i < (int)activeChunks.size();) {
            int cx = activeChunks[i].chunkX;
            int cz = activeChunks[i].chunkZ;
            int dist = std::max(std::abs(cx - currentChunkX), std::abs(cz - currentChunkZ));

            if (dist > renderDistance + 1) {
                activeChunks[i].mesh.cleanup();
                activeChunks.erase(activeChunks.begin() + i);
            }
            else {
                ++i;
            }
        }
    }

    void draw(GLuint shaderProgram, const Mat4& projection, const Mat4& view) {
        GLint transformLoc = glGetUniformLocation(shaderProgram, "transform");
        GLint modelLoc = glGetUniformLocation(shaderProgram, "model");

        for (int i = 0; i < (int)activeChunks.size(); ++i) {
            const Chunk& chunk = activeChunks[i];
            float worldX = (float)(chunk.chunkX * chunkSize);
            float worldZ = (float)(chunk.chunkZ * chunkSize);

            Mat4 model = Mat4::translate(Vec3(worldX, -10.0f, worldZ));
            Mat4 mvp = projection * view * model;

            glUniformMatrix4fv(transformLoc, 1, false, mvp.m);
            glUniformMatrix4fv(modelLoc, 1, false, model.m);

            chunk.mesh.draw();
        }
    }

    void cleanup() {
        for (int i = 0; i < (int)activeChunks.size(); ++i)
            activeChunks[i].mesh.cleanup();
        activeChunks.clear();
    }
};

#endif
