#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

uniform mat4 transform;
uniform mat4 model;
uniform mat4 lightSpaceMatrix;
uniform float time;

out vec3 vertColor;
out vec3 WorldPos;
out vec4 FragPosLightSpace;

void main() {
    vec3 pos = aPos;

    if (aPos.y > 1.5) {
        float wind = sin(pos.x * 0.5 + time * 2.0) * 0.08;
        pos.x += wind;
        pos.z += wind * 0.7;
    }

    WorldPos = vec3(model * vec4(pos, 1.0));
    vertColor = aColor;
    FragPosLightSpace = lightSpaceMatrix * vec4(WorldPos, 1.0);
    gl_Position = transform * vec4(pos, 1.0);
}
