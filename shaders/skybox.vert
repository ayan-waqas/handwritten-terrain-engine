#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 projection;
uniform mat4 view;

out vec3 TexCoords;

void main() {
    TexCoords = aPos;
    // remove translation from view matrix so skybox stays centered on camera
    mat4 staticView = mat4(mat3(view));
    vec4 pos = projection * staticView * vec4(aPos, 1.0);
    gl_Position = pos.xyww;
}
