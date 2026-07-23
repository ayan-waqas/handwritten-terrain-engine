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
    : window(nullptr), width(winWidth), height(winHeight), title(windowTitle),
      shaderProgram(0), waterShaderProgram(0), skyboxShaderProgram(0),
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

    // enable 3d depth testing and alpha blending
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // compile and link our shader from files
    shaderProgram = createShaderProgram("shaders/basic.vert", "shaders/basic.frag");
    waterShaderProgram = createShaderProgram("shaders/water.vert", "shaders/water.frag");
    skyboxShaderProgram = createShaderProgram("shaders/skybox.vert", "shaders/skybox.frag");

    if (shaderProgram == false || waterShaderProgram == false || skyboxShaderProgram == false) {
        std::cerr << "Failed to create shader program!" << std::endl;
        glfwTerminate();
        exit(-1);
    }

    setupBuffers();
}

void Engine::setupBuffers() {
    // generate 128x128 heightmap using fbm noise
    heightmap.generate(noiseGen, 0.05f, 4, 0.5f, 2.0f, 15.0f);

    // terrain mesh drawing lesgoo
    heightmap.buildMesh(terrainMesh);

    // setup water plane mesh
    water.setup(200.0f);

    // setup skybox mesh
    skybox.setup();
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

        glClearColor(0.10f, 0.12f, 0.16f, 1.0f); //set clear color to dark fantasy slate fog
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear screen and depth buffer

        // calculate 3d transforms using our handwritten math library
        Mat4 model = Mat4::translate(Vec3(0.0f, -10.0f, 0.0f)); // terrain position at origin
        Mat4 view = camera.getViewMatrix(); // dynamic view matrix from free fly camera
        Mat4 projection = Mat4::perspective(45.0f, (float)width / (float)height, 0.1f, 1000.0f); // 3d to 2d perspective

        // 1. render skybox with sky gradient and glowing sun orb
        glUseProgram(skyboxShaderProgram);
        GLint skyProjLoc = glGetUniformLocation(skyboxShaderProgram, "projection");
        glUniformMatrix4fv(skyProjLoc, 1, GL_FALSE, projection.m);

        GLint skyViewLoc = glGetUniformLocation(skyboxShaderProgram, "view");
        glUniformMatrix4fv(skyViewLoc, 1, GL_FALSE, view.m);

        GLint skyLightDirLoc = glGetUniformLocation(skyboxShaderProgram, "lightDir");
        glUniform3f(skyLightDirLoc, -0.5f, -1.0f, -0.3f);

        skybox.draw();

        // 2. render terrain mesh
        glUseProgram(shaderProgram);

        // combine MVP = projection * view * model
        Mat4 mvp = projection * view * model;

        // pass our matrix uniforms to vertex shader
        GLint transformLoc = glGetUniformLocation(shaderProgram, "transform");
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, mvp.m);

        GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, model.m);

        // pass directional sunlight uniforms to fragment shader
        GLint lightDirLoc = glGetUniformLocation(shaderProgram, "lightDir");
        glUniform3f(lightDirLoc, -0.5f, -1.0f, -0.3f);

        GLint lightColorLoc = glGetUniformLocation(shaderProgram, "lightColor");
        glUniform3f(lightColorLoc, 1.0f, 0.95f, 0.9f);

        // pass camera position for distance fog calculation
        GLint viewPosLoc = glGetUniformLocation(shaderProgram, "viewPos");
        glUniform3f(viewPosLoc, camera.position.x, camera.position.y, camera.position.z);

        terrainMesh.draw();

        // 3. render water plane
        glUseProgram(waterShaderProgram);
        GLint waterTransformLoc = glGetUniformLocation(waterShaderProgram, "transform");
        Mat4 waterMvp = projection * view * Mat4::translate(Vec3(0.0f, -10.0f, 0.0f));
        glUniformMatrix4fv(waterTransformLoc, 1, GL_FALSE, waterMvp.m);
        water.draw();

        glfwSwapBuffers(window); //swaps the on screen and off screen buffer
        glfwPollEvents();
    }
}
//to clean up gpu resources
void Engine::cleanup() {
    terrainMesh.cleanup();
    water.cleanup();
    skybox.cleanup();
    if (shaderProgram != 0)
        glDeleteProgram(shaderProgram);
    if (waterShaderProgram != 0)
        glDeleteProgram(waterShaderProgram);
    if (skyboxShaderProgram != 0)
        glDeleteProgram(skyboxShaderProgram);
    glfwTerminate();
}
