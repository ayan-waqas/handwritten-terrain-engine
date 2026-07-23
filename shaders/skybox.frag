#version 330 core

in vec3 TexCoords;
out vec4 FragColor;

uniform vec3 lightDir;

void main() {
    vec3 dir = normalize(TexCoords);

    // vertical gradient from horizon to sky top
    float height = dir.y;
    vec3 horizonColor = vec3(0.10, 0.12, 0.16);
    vec3 skyTopColor  = vec3(0.04, 0.06, 0.12);

    vec3 skyColor = mix(horizonColor, skyTopColor, clamp(height * 2.0, 0.0, 1.0));

    // glowing sun orb in direction of lightDir
    vec3 sunDir = normalize(-lightDir);
    float sunDot = max(dot(dir, sunDir), 0.0);
    float sunDisk = pow(sunDot, 256.0);
    float sunGlow = pow(sunDot, 16.0) * 0.4;

    vec3 sunColor = vec3(1.0, 0.9, 0.7) * (sunDisk * 2.0 + sunGlow);

    FragColor = vec4(skyColor + sunColor, 1.0);
}
