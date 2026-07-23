#version 330 core

out vec2 TexCoords;

void main() {
    // generate fullscreen triangle from vertex ID (no VBO needed)
    float x = -1.0;
    float y = -1.0;

    if (gl_VertexID == 1) {
        x = -1.0;
        y = 3.0;
    }
    else if (gl_VertexID == 2) {
        x = 3.0;
        y = -1.0;
    }

    TexCoords = vec2((x + 1.0) * 0.5, (y + 1.0) * 0.5);
    gl_Position = vec4(x, y, 0.0, 1.0);
}
