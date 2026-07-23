#include "Engine.h"
#include <fstream>
#include <sstream>

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

    glDeleteShader(vertShader);
    glDeleteShader(fragShader);
    return program;
}

Engine::Engine(const std::string& windowTitle, unsigned int winWidth, unsigned int winHeight)
    : window(nullptr), width(winWidth), height(winHeight), title(windowTitle),
      shaderProgram(0), waterShaderProgram(0), skyboxShaderProgram(0), treeShaderProgram(0),
      grassShaderProgram(0), particleShaderProgram(0), postprocessShaderProgram(0),
      framebuffer(0), colorTexture(0), depthRenderbuffer(0), screenQuadVAO(0),
      reflectionFramebuffer(0), reflectionColorTexture(0), reflectionDepthBuffer(0),
      chunkManager(32, 4), camera(Vec3(0.0f, 20.0f, 50.0f)), deltaTime(0.0f), lastFrame(0.0f),
      lastMouseX(winWidth / 2.0f), lastMouseY(winHeight / 2.0f), firstMouse(true),
      isFullscreen(false), windowedX(100), windowedY(100), windowedWidth(winWidth),
      windowedHeight(winHeight), f11PressedLastFrame(false) {
    init();
}

Engine::~Engine() {
    cleanup();
}

void Engine::framebufferSizeCallback(GLFWwindow* win, int w, int h) {
    Engine* engine = static_cast<Engine*>(glfwGetWindowUserPointer(win));
    if (engine == nullptr || w == 0 || h == 0)
        return;
    engine->width = (unsigned int)w;
    engine->height = (unsigned int)h;
    glViewport(0, 0, w, h);
    engine->resizeFramebuffer(w, h);
    engine->resizeReflectionFramebuffer(w, h);
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

void Engine::toggleFullscreen() {
    if (!isFullscreen) {
        glfwGetWindowPos(window, &windowedX, &windowedY);
        glfwGetWindowSize(window, &windowedWidth, &windowedHeight);
        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);
        glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
        isFullscreen = true;
    }
    else {
        glfwSetWindowMonitor(window, nullptr, windowedX, windowedY, windowedWidth, windowedHeight, 0);
        isFullscreen = false;
    }
}

void Engine::setupFramebuffer() {
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    glGenTextures(1, &colorTexture);
    glBindTexture(GL_TEXTURE_2D, colorTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTexture, 0);

    glGenRenderbuffers(1, &depthRenderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthRenderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depthRenderbuffer);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cerr << "Main framebuffer incomplete!" << std::endl;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glGenVertexArrays(1, &screenQuadVAO);
}

void Engine::setupReflectionFramebuffer() {
    glGenFramebuffers(1, &reflectionFramebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, reflectionFramebuffer);

    glGenTextures(1, &reflectionColorTexture);
    glBindTexture(GL_TEXTURE_2D, reflectionColorTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width / 4, height / 4, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, reflectionColorTexture, 0);

    glGenRenderbuffers(1, &reflectionDepthBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, reflectionDepthBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width / 4, height / 4);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, reflectionDepthBuffer);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cerr << "Reflection framebuffer incomplete!" << std::endl;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Engine::resizeFramebuffer(int w, int h) {
    if (colorTexture != 0) {
        glBindTexture(GL_TEXTURE_2D, colorTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    }
    if (depthRenderbuffer != 0) {
        glBindRenderbuffer(GL_RENDERBUFFER, depthRenderbuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, w, h);
    }
}

void Engine::resizeReflectionFramebuffer(int w, int h) {
    if (reflectionColorTexture != 0) {
        glBindTexture(GL_TEXTURE_2D, reflectionColorTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w / 4, h / 4, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    }
    if (reflectionDepthBuffer != 0) {
        glBindRenderbuffer(GL_RENDERBUFFER, reflectionDepthBuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, w / 4, h / 4);
    }
}

void Engine::init() {
    if (glfwInit() == false) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        exit(-1);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (window == nullptr) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        exit(-1);
    }

    glfwMaximizeWindow(window);
    glfwMakeContextCurrent(window);

    glfwSetWindowUserPointer(window, this);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouseCallback);

    if (gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) == false) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        glfwTerminate();
        exit(-1);
    }

    int fbWidth, fbHeight;
    glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
    width = (unsigned int)fbWidth;
    height = (unsigned int)fbHeight;
    glViewport(0, 0, fbWidth, fbHeight);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_PROGRAM_POINT_SIZE);

    shaderProgram = createShaderProgram("shaders/basic.vert", "shaders/basic.frag");
    waterShaderProgram = createShaderProgram("shaders/water.vert", "shaders/water.frag");
    skyboxShaderProgram = createShaderProgram("shaders/skybox.vert", "shaders/skybox.frag");
    treeShaderProgram = createShaderProgram("shaders/tree.vert", "shaders/tree.frag");
    grassShaderProgram = createShaderProgram("shaders/grass.vert", "shaders/grass.frag");
    particleShaderProgram = createShaderProgram("shaders/particle.vert", "shaders/particle.frag");
    postprocessShaderProgram = createShaderProgram("shaders/postprocess.vert", "shaders/postprocess.frag");

    if (shaderProgram == false || waterShaderProgram == false || skyboxShaderProgram == false || treeShaderProgram == false || grassShaderProgram == false || particleShaderProgram == false || postprocessShaderProgram == false) {
        std::cerr << "Failed to create shader program!" << std::endl;
        glfwTerminate();
        exit(-1);
    }

    setupFramebuffer();
    setupReflectionFramebuffer();
    setupBuffers();
}

void Engine::setupBuffers() {
    chunkManager.update(camera.position, noiseGen);
    water.setup(1500.0f);
    skybox.setup();
    tree.setup();
    rock.setup();
    grass.setup();
    particles.setup(600);
}

void Engine::processInput() {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_F11) == GLFW_PRESS) {
        if (!f11PressedLastFrame) {
            toggleFullscreen();
            f11PressedLastFrame = true;
        }
    }
    else {
        f11PressedLastFrame = false;
    }

    float speedMultiplier = 1.0f;
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        speedMultiplier = 2.5f;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.processKeyboard("FORWARD", deltaTime * speedMultiplier);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.processKeyboard("BACKWARD", deltaTime * speedMultiplier);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.processKeyboard("LEFT", deltaTime * speedMultiplier);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.processKeyboard("RIGHT", deltaTime * speedMultiplier);
}

void Engine::run() {
    float waterHeight = -10.0f;

    while (!glfwWindowShouldClose(window)) {
        float currentFrame = (float)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput();
        chunkManager.update(camera.position, noiseGen);

        Mat4 projection = Mat4::perspective(45.0f, (float)width / (float)height, 0.1f, 1000.0f);

        // ===== PASS 1a: Render Planar Reflection into reflectionFramebuffer =====
        glBindFramebuffer(GL_FRAMEBUFFER, reflectionFramebuffer);
        glViewport(0, 0, width / 4, height / 4);
        glClearColor(0.48f, 0.72f, 0.92f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        Mat4 reflectView = camera.getReflectionViewMatrix(waterHeight);
        Vec3 reflectCamPos = Vec3(camera.position.x, 2.0f * waterHeight - camera.position.y, camera.position.z);

        // 1. Skybox in reflection
        glUseProgram(skyboxShaderProgram);
        glUniformMatrix4fv(glGetUniformLocation(skyboxShaderProgram, "projection"), 1, false, projection.m);
        glUniformMatrix4fv(glGetUniformLocation(skyboxShaderProgram, "view"), 1, false, reflectView.m);
        glUniform3f(glGetUniformLocation(skyboxShaderProgram, "lightDir"), -0.5f, -0.8f, -0.3f);
        glUniform1f(glGetUniformLocation(skyboxShaderProgram, "time"), currentFrame);
        skybox.draw();

        // 2. Terrain in reflection
        glUseProgram(shaderProgram);
        glUniform3f(glGetUniformLocation(shaderProgram, "lightDir"), -0.5f, -0.8f, -0.3f);
        glUniform3f(glGetUniformLocation(shaderProgram, "lightColor"), 1.0f, 0.95f, 0.9f);
        glUniform3f(glGetUniformLocation(shaderProgram, "viewPos"), reflectCamPos.x, reflectCamPos.y, reflectCamPos.z);
        glUniform1f(glGetUniformLocation(shaderProgram, "time"), currentFrame);
        chunkManager.draw(shaderProgram, projection, reflectView);

        // 3. Trees in reflection
        chunkManager.drawTrees(treeShaderProgram, projection, reflectView, currentFrame, tree, reflectCamPos);

        // 4. Rocks in reflection
        chunkManager.drawRocks(treeShaderProgram, projection, reflectView, currentFrame, rock, reflectCamPos);


        // ===== PASS 1b: Render Scene to Main Framebuffer =====
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        glViewport(0, 0, width, height);
        glClearColor(0.48f, 0.72f, 0.92f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        Mat4 view = camera.getViewMatrix();

        // 1. Skybox
        glUseProgram(skyboxShaderProgram);
        glUniformMatrix4fv(glGetUniformLocation(skyboxShaderProgram, "projection"), 1, false, projection.m);
        glUniformMatrix4fv(glGetUniformLocation(skyboxShaderProgram, "view"), 1, false, view.m);
        glUniform3f(glGetUniformLocation(skyboxShaderProgram, "lightDir"), -0.5f, -0.8f, -0.3f);
        glUniform1f(glGetUniformLocation(skyboxShaderProgram, "time"), currentFrame);
        skybox.draw();

        // 2. Terrain chunks
        glUseProgram(shaderProgram);
        glUniform3f(glGetUniformLocation(shaderProgram, "lightDir"), -0.5f, -0.8f, -0.3f);
        glUniform3f(glGetUniformLocation(shaderProgram, "lightColor"), 1.0f, 0.95f, 0.9f);
        glUniform3f(glGetUniformLocation(shaderProgram, "viewPos"), camera.position.x, camera.position.y, camera.position.z);
        glUniform1f(glGetUniformLocation(shaderProgram, "time"), currentFrame);
        chunkManager.draw(shaderProgram, projection, view);

        // 3. Trees
        chunkManager.drawTrees(treeShaderProgram, projection, view, currentFrame, tree, camera.position);

        // 4. Rocks
        chunkManager.drawRocks(treeShaderProgram, projection, view, currentFrame, rock, camera.position);

        // 5. Dense Grass Carpet
        chunkManager.drawGrass(grassShaderProgram, projection, view, currentFrame, grass, camera.position);

        // 6. Floating Ambient Particles (Fireflies / Glowing Pollen Motes)
        glUseProgram(particleShaderProgram);
        glUniformMatrix4fv(glGetUniformLocation(particleShaderProgram, "projection"), 1, false, projection.m);
        glUniformMatrix4fv(glGetUniformLocation(particleShaderProgram, "view"), 1, false, view.m);
        glUniform1f(glGetUniformLocation(particleShaderProgram, "time"), currentFrame);
        glUniform3f(glGetUniformLocation(particleShaderProgram, "camPos"), camera.position.x, camera.position.y, camera.position.z);
        particles.draw();

        // 7. Water with Planar Reflection texture
        glUseProgram(waterShaderProgram);
        Mat4 waterModel = Mat4::translate(Vec3(camera.position.x, waterHeight, camera.position.z));
        Mat4 waterMvp = projection * view * waterModel;
        glUniformMatrix4fv(glGetUniformLocation(waterShaderProgram, "transform"), 1, false, waterMvp.m);
        glUniformMatrix4fv(glGetUniformLocation(waterShaderProgram, "model"), 1, false, waterModel.m);
        glUniform1f(glGetUniformLocation(waterShaderProgram, "time"), currentFrame);
        glUniform3f(glGetUniformLocation(waterShaderProgram, "lightDir"), -0.5f, -0.8f, -0.3f);
        glUniform3f(glGetUniformLocation(waterShaderProgram, "viewPos"), camera.position.x, camera.position.y, camera.position.z);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, reflectionColorTexture);
        glUniform1i(glGetUniformLocation(waterShaderProgram, "reflectionTexture"), 1);

        water.draw();

        // ===== PASS 2: Post-Processing (Bloom + Vignette + Volumetric God Rays) =====
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT);
        glDisable(GL_DEPTH_TEST);

        // Compute sun screen-space position for God Rays
        Vec3 lightDir = Vec3(-0.5f, -0.8f, -0.3f).normalize();
        Vec3 sunWorldPos = camera.position - lightDir * 500.0f;
        Mat4 sunMvp = projection * view;

        float clipX = sunMvp.m[0] * sunWorldPos.x + sunMvp.m[4] * sunWorldPos.y + sunMvp.m[8]  * sunWorldPos.z + sunMvp.m[12];
        float clipY = sunMvp.m[1] * sunWorldPos.x + sunMvp.m[5] * sunWorldPos.y + sunMvp.m[9]  * sunWorldPos.z + sunMvp.m[13];
        float clipW = sunMvp.m[3] * sunWorldPos.x + sunMvp.m[7] * sunWorldPos.y + sunMvp.m[11] * sunWorldPos.z + sunMvp.m[15];

        float sunInView = 0.0f;
        float sunScreenX = 0.5f;
        float sunScreenY = 0.5f;

        if (clipW > 0.001f) {
            float ndcX = clipX / clipW;
            float ndcY = clipY / clipW;

            if (ndcX >= -1.5f && ndcX <= 1.5f && ndcY >= -1.5f && ndcY <= 1.5f) {
                sunScreenX = ndcX * 0.5f + 0.5f;
                sunScreenY = ndcY * 0.5f + 0.5f;
                sunInView = 1.0f;
            }
        }

        glUseProgram(postprocessShaderProgram);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, colorTexture);
        glUniform1i(glGetUniformLocation(postprocessShaderProgram, "screenTexture"), 0);

        glUniform2f(glGetUniformLocation(postprocessShaderProgram, "sunScreenPos"), sunScreenX, sunScreenY);
        glUniform1f(glGetUniformLocation(postprocessShaderProgram, "sunInView"), sunInView);

        glBindVertexArray(screenQuadVAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindVertexArray(0);

        glEnable(GL_DEPTH_TEST);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

void Engine::cleanup() {
    chunkManager.cleanup();
    water.cleanup();
    skybox.cleanup();
    tree.cleanup();
    rock.cleanup();
    grass.cleanup();
    particles.cleanup();

    if (framebuffer != 0)
        glDeleteFramebuffers(1, &framebuffer);
    if (colorTexture != 0)
        glDeleteTextures(1, &colorTexture);
    if (depthRenderbuffer != 0)
        glDeleteRenderbuffers(1, &depthRenderbuffer);

    if (reflectionFramebuffer != 0)
        glDeleteFramebuffers(1, &reflectionFramebuffer);
    if (reflectionColorTexture != 0)
        glDeleteTextures(1, &reflectionColorTexture);
    if (reflectionDepthBuffer != 0)
        glDeleteRenderbuffers(1, &reflectionDepthBuffer);

    if (screenQuadVAO != 0)
        glDeleteVertexArrays(1, &screenQuadVAO);

    if (shaderProgram != 0)
        glDeleteProgram(shaderProgram);
    if (waterShaderProgram != 0)
        glDeleteProgram(waterShaderProgram);
    if (skyboxShaderProgram != 0)
        glDeleteProgram(skyboxShaderProgram);
    if (treeShaderProgram != 0)
        glDeleteProgram(treeShaderProgram);
    if (grassShaderProgram != 0)
        glDeleteProgram(grassShaderProgram);
    if (particleShaderProgram != 0)
        glDeleteProgram(particleShaderProgram);
    if (postprocessShaderProgram != 0)
        glDeleteProgram(postprocessShaderProgram);

    glfwTerminate();
}
