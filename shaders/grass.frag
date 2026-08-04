#version 330 core

in vec3 vertColor;
in vec3 WorldPos;
out vec4 FragColor;

uniform vec3 lightDir;
uniform vec3 lightColor;
uniform vec3 viewPos;
uniform float timeOfDay;

void main() {
    vec3 lightDirection = normalize(-lightDir);
    float sunElevation = lightDirection.y;
    float dayFactor = smoothstep(-0.1, 0.2, sunElevation);

    vec3 ambient = 0.42 * vec3(0.85, 0.92, 1.0) * dayFactor;
    ambient += (1.0 - dayFactor) * vec3(0.02, 0.04, 0.1);
    
    float diff = max(dot(vec3(0.0, 1.0, 0.0), lightDirection), 0.0) * 0.6 + 0.4;
    vec3 diffuse = diff * lightColor * dayFactor;

    vec3 litColor = (ambient + diffuse) * vertColor;

    float distance = length(viewPos - WorldPos);
    float distFog = clamp((distance - 60.0) / 240.0, 0.0, 1.0);
    float heightFog = exp(-max(WorldPos.y + 8.0, 0.0) * 0.04);
    float fogFactor = clamp(distFog + heightFog * 0.15, 0.0, 1.0);
    
    vec3 fogDay = vec3(0.48, 0.72, 0.92);
    vec3 fogSunset = vec3(0.8, 0.5, 0.3);
    vec3 fogNight = vec3(0.02, 0.05, 0.12);

    vec3 fogColor = mix(fogNight, mix(fogSunset, fogDay, smoothstep(0.0, 0.3, sunElevation)), dayFactor);

    vec3 finalColor = mix(litColor, fogColor, fogFactor);
    FragColor = vec4(finalColor, 1.0);
}
