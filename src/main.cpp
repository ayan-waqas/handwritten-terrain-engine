#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

std::string loadShaderSource(const char* filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Shader not found " << filePath << std::endl;
        return "";
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}
int main() {
    if (glfwInit() == GLFW_FALSE) {
        std::cerr << "Failed to initialize GLFW" << std::endl; //cerr used specifically for error handling
        return -1; // same here  , 0 denotes success but we want to denote failure hence -1
    }
    //creating a window and specifies certain context
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window = glfwCreateWindow(800, 600, "terrain", nullptr, nullptr);
    if (window == nullptr) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    // so basically glfw opens a window and our window pointer is actually the one pointing to all opengl functions in the gpu etc, so we pass it to the window creator function
    glfwMakeContextCurrent(window);

    // temp loop to keep the window open so we can verify it works
    while (!glfwWindowShouldClose(window)) {
        glfwSwapBuffers(window); //swaps the current(on screen),  and back(off screen) buffer.
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}