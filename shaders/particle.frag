#version 330 core

in vec3 particleColor;
in float particleAlpha;
out vec4 FragColor;

void main() {
    // Radial soft circle falloff from point center
    vec2 coord = gl_PointCoord - vec2(0.5);
    float distSq = dot(coord, coord);
    if (distSq > 0.25)
        discard;

    float alpha = (1.0 - sqrt(distSq) * 2.0) * particleAlpha;
    FragColor = vec4(particleColor * 1.5, alpha);
}
