#version 330 core

in vec3 vertColor;
in vec3 WorldPos;
out vec4 FragColor;

uniform vec3 lightDir;
uniform vec3 lightColor;
uniform vec3 viewPos;

void main() {
    // flat face normal from screen-space derivatives
    vec3 norm = normalize(cross(dFdx(WorldPos), dFdy(WorldPos)));
    vec3 lightDirection = normalize(-lightDir);
    vec3 viewDir = normalize(viewPos - WorldPos);

    // Half-Lambert Subsurface Wrap Lighting for Foliage
    float wrapDiff = max(dot(norm, lightDirection) * 0.6 + 0.4, 0.0);
    vec3 diffuse = wrapDiff * lightColor * 1.05;

    // Hemispheric Sky/Ground Ambient
    vec3 skyAmbient   = vec3(0.45, 0.68, 0.95);
    vec3 groundAmbient = vec3(0.20, 0.26, 0.18);
    float upWeight = norm.y * 0.5 + 0.5;
    vec3 ambient = mix(groundAmbient, skyAmbient, upWeight) * 0.45;

    // Back-lit Rim Highlight on Leaves/Branches
    float rim = pow(1.0 - max(dot(viewDir, norm), 0.0), 3.0) * max(dot(norm, lightDirection), 0.0);
    vec3 rimLight = vec3(1.0, 0.90, 0.65) * rim * 0.35;

    vec3 litColor = (ambient + diffuse) * vertColor + rimLight;

    // atmospheric fog matching terrain and sky
    float distance = length(viewPos - WorldPos);
    float distFog = clamp((distance - 80.0) / 280.0, 0.0, 1.0);
    float heightFog = exp(-max(WorldPos.y + 8.0, 0.0) * 0.04);
    float fogFactor = clamp(distFog + heightFog * 0.15, 0.0, 1.0);
    vec3 fogColor = vec3(0.48, 0.72, 0.92);

    vec3 finalColor = mix(litColor, fogColor, fogFactor);
    FragColor = vec4(finalColor, 1.0);
}
