#ifndef ENGINE_H
#define ENGINE_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include "math/Mat4.h"
#include "noise/PerlinNoise.h"
#include "terrain/Heightmap.h"

class Engine {
private:
    GLFWwindow* window;
    unsigned int width;
    unsigned int height;
    std::string title;
    GLuint shaderProgram;
    GLuint vao;
    GLuint vbo;
    PerlinNoise noiseGen;
    Heightmap heightmap;

    // helper functions for shader loading and compilation
    std::string loadShaderSource(const char* filePath);
    GLuint compileShader(GLenum type, const std::string& source);
    GLuint createShaderProgram(const char* vertPath, const char* fragPath);

    void init();
    void setupBuffers();
    void cleanup();

public:
    Engine(const std::string& windowTitle = "terrain", unsigned int winWidth = 800, unsigned int winHeight = 600);
    ~Engine();

    void run();
};

#endif
