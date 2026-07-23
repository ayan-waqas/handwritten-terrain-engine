#ifndef CHUNK_MANAGER_H
#define CHUNK_MANAGER_H

#include <vector>
#include <cmath>
#include "../noise/PerlinNoise.h"
#include "Heightmap.h"
#include "Mesh.h"
#include "Tree.h"
#include "Rock.h"
#include "Grass.h"
#include "../math/Mat4.h"

struct Chunk {
    int chunkX;
    int chunkZ;
    Heightmap heightmap;
    Mesh mesh;
    std::vector<Vec3> treePositions;
    std::vector<Vec3> rockPositions;
    std::vector<Vec3> grassPositions;

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

                    for (int z = 0; z < newChunk.heightmap.depth; ++z) {
                        for (int x = 0; x < newChunk.heightmap.width; ++x) {
                            float wx = worldOffsetX + x;
                            float wz = worldOffsetZ + z;
                            float heightVal = noiseGen.terrainHeight(wx, wz);
                            newChunk.heightmap.heights[z * newChunk.heightmap.width + x] = heightVal;

                            // trees in valleys (sparse)
                            if (x % 9 == 0 && z % 9 == 0 && heightVal > 3.0f && heightVal < 6.0f)
                                newChunk.treePositions.push_back(Vec3(wx, heightVal, wz));

                            // rocks on mountain slopes
                            if (x % 10 == 3 && z % 10 == 3 && heightVal > 6.0f && heightVal < 12.0f)
                                newChunk.rockPositions.push_back(Vec3(wx, heightVal, wz));

                            // continuous dense grass carpet joined together seamlessly across green valley meadows
                            if (heightVal > 1.2f && heightVal < 7.0f) {
                                for (int subZ = 0; subZ < 2; ++subZ) {
                                    for (int subX = 0; subX < 2; ++subX) {
                                        float px = wx + (float)subX * 0.5f - 0.25f;
                                        float pz = wz + (float)subZ * 0.5f - 0.25f;
                                        for (int k = 0; k < 3; ++k) {
                                            float angle = (float)k * 2.094395f + (float)(subX + subZ) * 0.7f;
                                            float radius = 0.12f;
                                            float offsetRight = radius * std::cos(angle);
                                            float offsetForward = radius * std::sin(angle);

                                            newChunk.grassPositions.push_back(Vec3(px + offsetRight, heightVal, pz + offsetForward));
                                        }
                                    }
                                }
                            }
                        }
                    }

                    newChunk.heightmap.applyErosion(8);
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

    void drawTrees(GLuint treeShader, const Mat4& projection, const Mat4& view, float time, const Tree& treeMesh, const Vec3& camPos) {
        glUseProgram(treeShader);
        GLint transformLoc = glGetUniformLocation(treeShader, "transform");
        GLint modelLoc = glGetUniformLocation(treeShader, "model");
        GLint timeLoc = glGetUniformLocation(treeShader, "time");
        GLint lightDirLoc = glGetUniformLocation(treeShader, "lightDir");
        GLint lightColorLoc = glGetUniformLocation(treeShader, "lightColor");
        GLint viewPosLoc = glGetUniformLocation(treeShader, "viewPos");

        glUniform1f(timeLoc, time);
        glUniform3f(lightDirLoc, -0.5f, -0.8f, -0.3f);
        glUniform3f(lightColorLoc, 1.0f, 0.95f, 0.9f);
        glUniform3f(viewPosLoc, camPos.x, camPos.y, camPos.z);

        for (int i = 0; i < (int)activeChunks.size(); ++i) {
            const Chunk& chunk = activeChunks[i];
            for (int t = 0; t < (int)chunk.treePositions.size(); ++t) {
                Vec3 pos = chunk.treePositions[t];

                float distSq = (pos.x - camPos.x)*(pos.x - camPos.x) + (pos.z - camPos.z)*(pos.z - camPos.z);
                if (distSq > 160000.0f)
                    continue;

                Mat4 model = Mat4::translate(Vec3(pos.x, pos.y - 10.0f, pos.z));
                Mat4 mvp = projection * view * model;

                glUniformMatrix4fv(transformLoc, 1, false, mvp.m);
                glUniformMatrix4fv(modelLoc, 1, false, model.m);
                treeMesh.draw();
            }
        }
    }

    void drawRocks(GLuint treeShader, const Mat4& projection, const Mat4& view, float time, const Rock& rockMesh, const Vec3& camPos) {
        glUseProgram(treeShader);
        GLint transformLoc = glGetUniformLocation(treeShader, "transform");
        GLint modelLoc = glGetUniformLocation(treeShader, "model");
        GLint timeLoc = glGetUniformLocation(treeShader, "time");
        GLint lightDirLoc = glGetUniformLocation(treeShader, "lightDir");
        GLint lightColorLoc = glGetUniformLocation(treeShader, "lightColor");
        GLint viewPosLoc = glGetUniformLocation(treeShader, "viewPos");

        glUniform1f(timeLoc, time);
        glUniform3f(lightDirLoc, -0.5f, -0.8f, -0.3f);
        glUniform3f(lightColorLoc, 1.0f, 0.95f, 0.9f);
        glUniform3f(viewPosLoc, camPos.x, camPos.y, camPos.z);

        for (int i = 0; i < (int)activeChunks.size(); ++i) {
            const Chunk& chunk = activeChunks[i];
            for (int r = 0; r < (int)chunk.rockPositions.size(); ++r) {
                Vec3 pos = chunk.rockPositions[r];

                float distSq = (pos.x - camPos.x)*(pos.x - camPos.x) + (pos.z - camPos.z)*(pos.z - camPos.z);
                if (distSq > 160000.0f)
                    continue;

                Mat4 model = Mat4::translate(Vec3(pos.x, pos.y - 10.0f, pos.z));
                Mat4 mvp = projection * view * model;

                glUniformMatrix4fv(transformLoc, 1, false, mvp.m);
                glUniformMatrix4fv(modelLoc, 1, false, model.m);
                rockMesh.draw();
            }
        }
    }

    void drawGrass(GLuint grassShader, const Mat4& projection, const Mat4& view, float time, const Grass& grassMesh, const Vec3& camPos) {
        glUseProgram(grassShader);
        GLint transformLoc = glGetUniformLocation(grassShader, "transform");
        GLint modelLoc = glGetUniformLocation(grassShader, "model");
        GLint timeLoc = glGetUniformLocation(grassShader, "time");
        GLint lightDirLoc = glGetUniformLocation(grassShader, "lightDir");
        GLint lightColorLoc = glGetUniformLocation(grassShader, "lightColor");
        GLint viewPosLoc = glGetUniformLocation(grassShader, "viewPos");

        glUniform1f(timeLoc, time);
        glUniform3f(lightDirLoc, -0.5f, -0.8f, -0.3f);
        glUniform3f(lightColorLoc, 1.0f, 0.95f, 0.9f);
        glUniform3f(viewPosLoc, camPos.x, camPos.y, camPos.z);

        glDisable(GL_CULL_FACE);

        for (int i = 0; i < (int)activeChunks.size(); ++i) {
            const Chunk& chunk = activeChunks[i];
            for (int g = 0; g < (int)chunk.grassPositions.size(); ++g) {
                Vec3 pos = chunk.grassPositions[g];

                // distance culling to 40 meters for high performance on laptop/integrated GPUs
                float distSq = (pos.x - camPos.x)*(pos.x - camPos.x) + (pos.z - camPos.z)*(pos.z - camPos.z);
                if (distSq > 1600.0f)
                    continue;

                Mat4 model = Mat4::translate(Vec3(pos.x, pos.y - 10.0f, pos.z));
                Mat4 mvp = projection * view * model;

                glUniformMatrix4fv(transformLoc, 1, false, mvp.m);
                glUniformMatrix4fv(modelLoc, 1, false, model.m);
                grassMesh.draw();
            }
        }
    }

    void cleanup() {
        for (int i = 0; i < (int)activeChunks.size(); ++i)
            activeChunks[i].mesh.cleanup();
        activeChunks.clear();
    }
};

#endif
