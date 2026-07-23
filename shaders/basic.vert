#version 330 core

// input vertex position and normal vector
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

// uniform transformation matrices
uniform mat4 transform;
uniform mat4 model;

// outputs to fragment shader
out vec3 FragPos;
out vec3 Normal;
out float Height;

void main() {
    // calculate world space position and surface normal
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = aNormal;
    Height = aPos.y;

    gl_Position = transform * vec4(aPos, 1.0);
}
