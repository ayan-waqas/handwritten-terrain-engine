#version 330 core

// output color variable representing the final RGBA color of the pixel
out vec4 FragColor;

void main() {
    // outputs a solid orange color: Red (100%), Green (50%), Blue (20%), Alpha (100% opaque)
    FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
}
