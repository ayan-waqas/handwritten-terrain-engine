#version 330 core

// input vertex position from C++ vertex buffer (location = 0 matches our vertex attribute layout)
layout (location = 0) in vec3 aPos;

void main() {
    // converts the 3D position into a 4D  coordinate (X, Y, Z, W)
    // w is set to 1 for now  so that the coordinates are kept at their original scale.
    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
}
