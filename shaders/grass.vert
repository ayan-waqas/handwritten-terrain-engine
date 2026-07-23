#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in float aWindWeight;

out vec3 vertColor;
out vec3 WorldPos;

uniform mat4 transform;
uniform mat4 model;
uniform float time;

void main() {
    vec3 pos = aPos;
    vec3 worldBase = vec3(model * vec4(0.0, 0.0, 0.0, 1.0));

    // Smooth 2D scrolling wind field ripple across meadow
    float windWave = sin(time * 1.8 + worldBase.x * 0.15 + worldBase.z * 0.15);
    float windGust = cos(time * 2.4 + worldBase.x * 0.22 - worldBase.z * 0.18) * 0.5 + 0.5;

    float displacement = (windWave * 0.7 + windGust * 0.3) * aWindWeight * 0.10;
    pos.x += displacement;
    pos.z += displacement * 0.6;

    WorldPos = vec3(model * vec4(pos, 1.0));
    vertColor = aColor;

    gl_Position = transform * vec4(pos, 1.0);
}
