#version 330 core

in vec3 TexCoords;
out vec4 FragColor;

uniform vec3 lightDir;

void main() {
    vec3 dir = normalize(TexCoords);

    // vertical gradient from horizon to sky top (dark fantasy twilight)
    float height = dir.y;
    vec3 horizonColor = vec3(0.08, 0.07, 0.11);
    vec3 skyTopColor  = vec3(0.03, 0.04, 0.08);

    vec3 skyColor = mix(horizonColor, skyTopColor, clamp(height * 2.0, 0.0, 1.0));

    // glowing low sunset sun orb in direction of lightDir
    vec3 sunDir = normalize(-lightDir);
    float sunDot = max(dot(dir, sunDir), 0.0);
    float sunDisk = pow(sunDot, 256.0);
    float sunGlow = pow(sunDot, 16.0) * 0.4;

    vec3 sunColor = vec3(0.95, 0.80, 0.65) * (sunDisk * 2.0 + sunGlow);

    FragColor = vec4(skyColor + sunColor, 1.0);
}
