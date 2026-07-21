#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

int main() {
    // initialzing gflw and requesting opengl context
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(800, 600, "Terrain", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create window"<<std::endl; //cerr used to output errors specifcally
        glfwTerminate();
        return 0;
    }
    glfwMakeContextCurrent(window);

    // loading opengl function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD"<<std::endl;
        return 0;
    }

    glViewport(0, 0, 800, 600);

    // main render loop
    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}