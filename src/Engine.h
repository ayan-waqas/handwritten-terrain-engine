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
#include "terrain/Tree.h"
#include "terrain/Particles.h"
#include "terrain/Grass.h"
#include "terrain/Rock.h"
#include "camera/Camera.h"
#include "audio/AudioEngine.h"

class Engine {
private:
    GLFWwindow* window;
    unsigned int width;
    unsigned int height;
    std::string title;

    GLuint shaderProgram;
    GLuint waterShaderProgram;
    GLuint skyboxShaderProgram;
    GLuint treeShaderProgram;
    GLuint grassShaderProgram;
    GLuint particleShaderProgram;
    GLuint postprocessShaderProgram;
    GLuint shadowShaderProgram;

    // post-processing framebuffer
    GLuint framebuffer;
    GLuint colorTexture;
    GLuint depthRenderbuffer;
    GLuint screenQuadVAO;

    // planar water reflection framebuffer
    GLuint reflectionFramebuffer;
    GLuint reflectionColorTexture;
    GLuint reflectionDepthBuffer;

    // directional shadow depth map framebuffer
    GLuint shadowFramebuffer;
    GLuint shadowDepthMap;
    const unsigned int SHADOW_WIDTH = 1024;
    const unsigned int SHADOW_HEIGHT = 1024;
    
    // Day/night cycle
    float timeOfDay;
    bool dayNightAutoAdvance;
    bool tPressedLastFrame;

    // minimap FBO
    GLuint minimapFramebuffer;
    GLuint minimapColorTexture;
    GLuint minimapDepthBuffer;

    PerlinNoise noiseGen;
    ChunkManager chunkManager;
    Water water;
    Skybox skybox;
    Tree tree;
    Rock rock;
    Grass grass;
    Particles particles;
    AudioEngine audioEngine;
    Camera camera;

    float deltaTime;
    float lastFrame;

    float lastMouseX;
    float lastMouseY;
    bool firstMouse;
    bool isFullscreen;
    int windowedX;
    int windowedY;
    int windowedWidth;
    int windowedHeight;
    bool f11PressedLastFrame;

    // helper functions for shader loading and compilation
    std::string loadShaderSource(const char* filePath);
    GLuint compileShader(GLenum type, const std::string& source);
    GLuint createShaderProgram(const char* vertPath, const char* fragPath);

    void init();
    void setupBuffers();
    void setupFramebuffer();
    void setupReflectionFramebuffer();
    void setupShadowFramebuffer();
    void resizeFramebuffer(int w, int h);
    void resizeReflectionFramebuffer(int w, int h);
    void setupMinimapFramebuffer();
    void resizeMinimapFramebuffer(int w, int h);
    void processInput();
    void cleanup();
    void toggleFullscreen();

    static void mouseCallback(GLFWwindow* win, double xpos, double ypos);
    static void framebufferSizeCallback(GLFWwindow* win, int w, int h);

public:
    Engine(const std::string& windowTitle = "terrain", unsigned int winWidth = 800, unsigned int winHeight = 600);
    ~Engine();

    void run();
};

#endif
