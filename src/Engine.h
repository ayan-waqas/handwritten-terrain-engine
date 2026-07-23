#ifndef ENGINE_H
#define ENGINE_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include "math/Mat4.h"
#include "noise/PerlinNoise.h"
#include "terrain/ChunkManager.h"
#include "terrain/Water.h"
#include "terrain/Skybox.h"
#include "camera/Camera.h"

class Engine {
private:
    GLFWwindow* window;
    unsigned int width;
    unsigned int height;
    std::string title;

    GLuint shaderProgram;
    GLuint waterShaderProgram;
    GLuint skyboxShaderProgram;

    PerlinNoise noiseGen;
    ChunkManager chunkManager;
    Water water;
    Skybox skybox;
    Camera camera;

    float deltaTime;
    float lastFrame;

    float lastMouseX;
    float lastMouseY;
    bool firstMouse;

    // helper functions for shader loading and compilation
    std::string loadShaderSource(const char* filePath);
    GLuint compileShader(GLenum type, const std::string& source);
    GLuint createShaderProgram(const char* vertPath, const char* fragPath);

    void init();
    void setupBuffers();
    void processInput();
    void cleanup();

    static void mouseCallback(GLFWwindow* win, double xpos, double ypos);

public:
    Engine(const std::string& windowTitle = "terrain", unsigned int winWidth = 800, unsigned int winHeight = 600);
    ~Engine();

    void run();
};

#endif
