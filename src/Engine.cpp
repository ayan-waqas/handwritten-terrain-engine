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
    : window(nullptr), width(winWidth), height(winHeight), title(windowTitle), shaderProgram(0),
      camera(Vec3(0.0f, 20.0f, 50.0f)), deltaTime(0.0f), lastFrame(0.0f),
      lastMouseX(winWidth / 2.0f), lastMouseY(winHeight / 2.0f), firstMouse(true) {
    init();
}

Engine::~Engine() {
    cleanup();
}

void Engine::mouseCallback(GLFWwindow* win, double xpos, double ypos) {
    Engine* engine = static_cast<Engine*>(glfwGetWindowUserPointer(win));
    if (engine == nullptr)
        return;

    float xPosF = (float)xpos;
    float yPosF = (float)ypos;

    if (engine->firstMouse) {
        engine->lastMouseX = xPosF;
        engine->lastMouseY = yPosF;
        engine->firstMouse = false;
    }

    float xOffset = xPosF - engine->lastMouseX;
    float yOffset = engine->lastMouseY - yPosF;

    engine->lastMouseX = xPosF;
    engine->lastMouseY = yPosF;

    engine->camera.processMouseMovement(xOffset, yOffset);
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

    // capture mouse cursor inside window for fps camera controls
    glfwSetWindowUserPointer(window, this);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouseCallback);

    if (gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) == false) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        glfwTerminate();
        exit(-1);
    }

    //  viewport for current size (might change later)//////////////////////////////////////////
    glViewport(0, 0, width, height);

    // enable 3d depth testing
    glEnable(GL_DEPTH_TEST);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // compile and link our shader from files
    shaderProgram = createShaderProgram("shaders/basic.vert", "shaders/basic.frag");
    if (shaderProgram == false) {
        std::cerr << "Failed to create shader program!" << std::endl;
        glfwTerminate();
        exit(-1);
    }

    setupBuffers();
}

void Engine::setupBuffers() {
    // generate 128x128 heightmap using fbm noise
    heightmap.generate(noiseGen, 0.05f, 4, 0.5f, 2.0f, 5.0f);

    // terrain mesh drawing lesgoo
    heightmap.buildMesh(terrainMesh);
}

void Engine::processInput() {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.processKeyboard("FORWARD", deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.processKeyboard("BACKWARD", deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.processKeyboard("LEFT", deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.processKeyboard("RIGHT", deltaTime);
}

void Engine::run() {
    // temp loop to keep the window open so we can verify it works
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = (float)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput();

        glClearColor(0.1f, 0.1f, 0.15f, 1.0f); //set clear color
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear screen and depth buffer

        glUseProgram(shaderProgram);

        // calculate 3d transforms using our handwritten math library
        Mat4 model = Mat4::translate(Vec3(0.0f, -10.0f, 0.0f)); // terrain position at origin
        Mat4 view = camera.getViewMatrix(); // dynamic view matrix from free fly camera
        Mat4 projection = Mat4::perspective(45.0f, (float)width / (float)height, 0.1f, 1000.0f); // 3d to 2d perspective

        // combine MVP = projection * view * model
        Mat4 mvp = projection * view * model;

        // pass our matrix to the vertex shader uniform
        GLint transformLoc = glGetUniformLocation(shaderProgram, "transform");
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, mvp.m);

        terrainMesh.draw();

        glfwSwapBuffers(window); //swaps the on screen and off screen buffer
        glfwPollEvents();
    }
}

void Engine::cleanup() {
    //cleaning up gpu resources
    terrainMesh.cleanup();
    if (shaderProgram != 0)
        glDeleteProgram(shaderProgram);
    glfwTerminate();
}
