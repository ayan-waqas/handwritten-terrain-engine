#version 330 core

layout (location = 0) in vec3 aPos;

uniform mat4 transform;
uniform mat4 model;
uniform float time;

out vec3 WorldPos;
out vec4 ClipSpacePos;

void main() {
    vec3 pos = aPos;
    WorldPos = vec3(model * vec4(pos, 1.0));
    ClipSpacePos = transform * vec4(pos, 1.0);
    gl_Position = ClipSpacePos;
}
