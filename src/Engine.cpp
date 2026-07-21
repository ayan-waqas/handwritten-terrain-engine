#include "Engine.h"
#include <fstream>
#include <sstream>

//shader - point , fragment - color

std::string Engine::loadShaderSource(const char* filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Shader not found " << filePath << std::endl;
        return "";
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

// compiles a single shader (vertex or fragment)
GLuint Engine::compileShader(GLenum type, const std::string& source) {
    GLuint shader = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "Shader compile error (" << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << "):\n" << infoLog << std::endl;
    }
    return shader;
}

// compiles vertex and fragment shaders and links them into a program.
GLuint Engine::createShaderProgram(const char* vertPath, const char* fragPath) {
    std::string vertSrc = loadShaderSource(vertPath);
    std::string fragSrc = loadShaderSource(fragPath);
    GLuint vertShader = compileShader(GL_VERTEX_SHADER, vertSrc);
    GLuint fragShader = compileShader(GL_FRAGMENT_SHADER, fragSrc);
    GLuint program = glCreateProgram();
    glAttachShader(program, vertShader);
    glAttachShader(program, fragShader);
    glLinkProgram(program);
    int success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (success == 0) {
        char infoLog[512];
        glGetProgramInfoLog(program, 512, nullptr, infoLog);
        std::cerr << "Shader link error:\n" << infoLog << std::endl;
    }

    // individual shader aint needed anymore cuz already linked
    glDeleteShader(vertShader);
    glDeleteShader(fragShader);
    return program;
}

Engine::Engine(const std::string& windowTitle, unsigned int winWidth, unsigned int winHeight)
    : window(nullptr), width(winWidth), height(winHeight), title(windowTitle), shaderProgram(0), vao(0), vbo(0) {
    init();
}

Engine::~Engine() {
    cleanup();
}

void Engine::init() {
    if (glfwInit() == false) {
        std::cerr << "Failed to initialize GLFW" << std::endl; //cerr used specifically for error handling
        exit(-1); // same here , 0 denotes success but we want to denote failure hence -1
    }
    //creating a window and specifies certain context
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (window == nullptr) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        exit(-1);
    }

    // so basically glfw opens a window and our window pointer is actually the one pointing to all opengl functions in the gpu etc, so we pass it to the window creator function
    glfwMakeContextCurrent(window);

    if (gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) == false) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        glfwTerminate();
        exit(-1);
    }

    //  viewport for current size (might change later)//////////////////////////////////////////
    glViewport(0, 0, width, height);

    // compile and link our shader from files
    shaderProgram = createShaderProgram("shaders/basic.vert", "shaders/basic.frag");
    if (shaderProgram == false) {
        std::cerr << "Failed to create shader program!" << std::endl;
        glfwTerminate();
        exit(-1);
    }

    setupBuffers();

    // generate 128x128 heightmap using fbm noise
    heightmap.generate(noiseGen, 0.05f, 4, 0.5f, 2.0f, 3.0f);
}

void Engine::setupBuffers() {
    //triangle drawing lesgoo
    float vertices[] = {
        -0.5f, -0.5f, 0.0f, // bottom-left corner
         0.5f, -0.5f, 0.0f, // bottom-right corner
         0.0f,  0.5f, 0.0f  // top-center
    };

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Engine::run() {
    // temp loop to keep the window open so we can verify it works
    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.1f, 0.1f, 0.15f, 1.0f); //set clear color
        glClear(GL_COLOR_BUFFER_BIT); // clear screen

        glUseProgram(shaderProgram);

        // calculate 3d transforms using our handwritten math library
        Mat4 model = Mat4::translate(Vec3(0.0f, 0.0f, -2.0f)); // push triangle back into screen
        Mat4 view = Mat4::lookAt(Vec3(0.0f, 0.0f, 0.0f), Vec3(0.0f, 0.0f, -1.0f), Vec3(0.0f, 1.0f, 0.0f)); // camera at origin looking forward
        Mat4 projection = Mat4::perspective(45.0f, (float)width / (float)height, 0.1f, 100.0f); // 3d to 2d perspective

        // combine MVP = projection * view * model
        Mat4 mvp = projection * view * model;

        // pass our matrix to the vertex shader uniform
        GLint transformLoc = glGetUniformLocation(shaderProgram, "transform");
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, mvp.m);

        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window); //swaps the on screen and off screen buffer
        glfwPollEvents();
    }
}

void Engine::cleanup() {
    //cleaning up gpu resources
    if (vao != 0)
        glDeleteVertexArrays(1, &vao);
    if (vbo != 0)
        glDeleteBuffers(1, &vbo);
    if (shaderProgram != 0)
        glDeleteProgram(shaderProgram);
    glfwTerminate();
}
