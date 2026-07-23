#version 330 core

// input vertex data
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in float aBiome;

// uniform transformation matrices
uniform mat4 transform;
uniform mat4 model;
uniform float time;

// outputs to fragment shader
out vec3 FragPos;
out vec3 Normal;
out float Height;
out float Biome;

void main() {
    vec3 pos = aPos;

    // wind sway animation on low vegetation ground
    if (aPos.y < 2.0) {
        float wind = sin(aPos.x * 0.5 + time * 2.0) * cos(aPos.z * 0.5 + time * 1.5) * 0.15;
        pos.x += wind;
        pos.z += wind;
    }

    // calculate world space position and surface normal
    FragPos = vec3(model * vec4(pos, 1.0));
    Normal = aNormal;
    Height = aPos.y;
    Biome = aBiome;

    gl_Position = transform * vec4(pos, 1.0);
}
