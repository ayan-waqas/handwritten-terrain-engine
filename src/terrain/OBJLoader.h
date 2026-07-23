#ifndef OBJ_LOADER_H
#define OBJ_LOADER_H

#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include "../math/Vec3.h"

class OBJLoader {
public:

    struct OBJVertex {
        Vec3 position;
        Vec3 normal;
    };

    // loads an obj file into vertex and index arrays ready for Mesh::setup()
    static bool load(const char* filePath, std::vector<OBJVertex>& outVertices, std::vector<unsigned int>& outIndices) {
        std::ifstream file(filePath);
        if (!file.is_open()) {
            std::cerr << "obj file not found: " << filePath << std::endl;
            return false;
        }

        std::vector<Vec3> positions;
        std::vector<Vec3> normals;

        std::string line;
        while (std::getline(file, line)) {
            std::istringstream stream(line);
            std::string prefix;
            stream >> prefix;

            if (prefix == "v") {
                float x, y, z;
                stream >> x >> y >> z;
                positions.push_back(Vec3(x, y, z));
            }
            else if (prefix == "vn") {
                float x, y, z;
                stream >> x >> y >> z;
                normals.push_back(Vec3(x, y, z));
            }
            else if (prefix == "f") {
                // parse face indices - supports f v//vn and f v/vt/vn
                std::string token;
                int faceVertCount = 0;
                unsigned int firstIdx = 0;
                unsigned int prevIdx = 0;

                while (stream >> token) {
                    int posIdx = 0;
                    int normIdx = 0;

                    // find first slash
                    int slashPos1 = -1;
                    for (int c = 0; c < (int)token.length(); ++c) {
                        if (token[c] == '/') {
                            slashPos1 = c;
                            break;
                        }
                    }

                    if (slashPos1 == -1) {
                        // format: v
                        posIdx = std::stoi(token);
                    }
                    else {
                        // get position index before first slash
                        posIdx = std::stoi(token.substr(0, slashPos1));

                        // find second slash
                        int slashPos2 = -1;
                        for (int c = slashPos1 + 1; c < (int)token.length(); ++c) {
                            if (token[c] == '/') {
                                slashPos2 = c;
                                break;
                            }
                        }

                        if (slashPos2 != -1) {
                            // format: v//vn or v/vt/vn
                            std::string normStr = token.substr(slashPos2 + 1);
                            if (normStr.length() > 0)
                                normIdx = std::stoi(normStr);
                        }
                    }

                    // obj indices are 1-based
                    OBJVertex vert;
                    vert.position = positions[posIdx - 1];

                    if (normIdx > 0 && normIdx <= (int)normals.size())
                        vert.normal = normals[normIdx - 1];
                    else
                        vert.normal = Vec3(0.0f, 1.0f, 0.0f);

                    unsigned int currentIdx = (unsigned int)outVertices.size();
                    outVertices.push_back(vert);

                    // triangulate faces with more than 3 vertices using fan method
                    if (faceVertCount == 0) {
                        firstIdx = currentIdx;
                    }
                    else if (faceVertCount >= 2) {
                        outIndices.push_back(firstIdx);
                        outIndices.push_back(prevIdx);
                        outIndices.push_back(currentIdx);
                    }

                    prevIdx = currentIdx;
                    faceVertCount++;
                }
            }
        }

        std::cout << "loaded obj: " << filePath << " (" << outVertices.size() << " vertices, " << outIndices.size() / 3 << " triangles)" << std::endl;
        return true;
    }
};

#endif
