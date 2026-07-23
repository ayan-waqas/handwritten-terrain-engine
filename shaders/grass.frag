#version 330 core

in vec3 vertColor;
in vec3 WorldPos;
out vec4 FragColor;

uniform vec3 lightDir;
uniform vec3 lightColor;
uniform vec3 viewPos;

void main() {
    vec3 lightDirection = normalize(-lightDir);

    // Soft ambient + directional lighting
    vec3 ambient = 0.42 * vec3(0.85, 0.92, 1.0);
    float diff = max(dot(vec3(0.0, 1.0, 0.0), lightDirection), 0.0) * 0.6 + 0.4;
    vec3 diffuse = diff * lightColor;

    vec3 litColor = (ambient + diffuse) * vertColor;

    // Atmospheric distance fog matching terrain and sky
    float distance = length(viewPos - WorldPos);
    float distFog = clamp((distance - 60.0) / 240.0, 0.0, 1.0);
    float heightFog = exp(-max(WorldPos.y + 8.0, 0.0) * 0.04);
    float fogFactor = clamp(distFog + heightFog * 0.15, 0.0, 1.0);
    vec3 fogColor = vec3(0.48, 0.72, 0.92);

    vec3 finalColor = mix(litColor, fogColor, fogFactor);
    FragColor = vec4(finalColor, 1.0);
}
