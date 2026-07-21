cmake_minimum_required(VERSION 3.20)
project(handwritten-terrain-engine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Fetch GLFW automatically
include(FetchContent)
FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG 3.3.8
)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(glfw)

# GLAD (manually placed, compiled as part of our sources)
add_library(glad external/glad/src/glad.c)
target_include_directories(glad PUBLIC external/glad/include)

add_executable(handwritten-terrain-engine src/main.cpp
        src/main.cpp)

target_link_libraries(handwritten-terrain-engine PRIVATE glfw glad)

if(WIN32)
    target_link_libraries(handwritten-terrain-engine PRIVATE opengl32)
elseif(APPLE)
    target_link_libraries(handwritten-terrain-engine PRIVATE "-framework OpenGL")
else()
    target_link_libraries(handwritten-terrain-engine PRIVATE GL)
endif()