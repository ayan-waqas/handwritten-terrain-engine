#version 330 core

// Input vertex position from C++ vertex buffer
layout (location = 0) in vec3 aPos;

// uniform 4x4 matrix passed from C++ every frame
uniform mat4 transform;

void main() {
    // multiply the matrix by the 4D position to transform the vertex in 3D space
    gl_Position = transform * vec4(aPos, 1.0);
}
