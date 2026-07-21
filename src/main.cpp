#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

int main() {
    int init = glfwInit();
    if (init == false) {
        std::cerr << "Failed to initialize GLFW" << std::endl; //cerr used specifically for error handling
        return -1; // same here  , 0 denotes success but we want to denote failure hence -1
    }
    //creating a window and specifies certain context
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window = glfwCreateWindow(800, 600, "terrain", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    // so basically glfw opens a window and our window pointer is actually the one pointing to all opengl functions etc, so we pass it to the window creator function
    glfwMakeContextCurrent(window);

    // temp loop to keep the window open so we can verify it works
    while (!glfwWindowShouldClose(window)) {
        glfwSwapBuffers(window); //swaps the current(on screen),  and back(off screen) buffer.
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}