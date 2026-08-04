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
    vec3 viewDir = normalize(viewPos - WorldPos);
    float sunElevation = lightDirection.y;
    float dayFactor = smoothstep(-0.1, 0.2, sunElevation);

    vec3 ambient = 0.45 * vec3(0.85, 0.92, 1.0) * dayFactor;
    ambient += (1.0 - dayFactor) * vec3(0.02, 0.04, 0.1);
    
    float diff = max(dot(vec3(0.0, 1.0, 0.0), lightDirection), 0.0) * 0.6 + 0.4;
    vec3 diffuse = diff * lightColor * dayFactor;

    // Subsurface Backlight Transmission
    float backlight = max(dot(-viewDir, lightDirection), 0.0);
    float sss = pow(backlight, 3.0) * 0.45 * dayFactor;
    vec3 sssColor = vec3(0.4, 0.85, 0.25) * sss * lightColor;

    vec3 litColor = (ambient + diffuse) * vertColor + sssColor;

    float distance = length(viewPos - WorldPos);
    float distFog = clamp((distance - 60.0) / 240.0, 0.0, 1.0);
    float heightFog = exp(-max(WorldPos.y + 8.0, 0.0) * 0.04);
    float fogFactor = clamp(distFog + heightFog * 0.15, 0.0, 1.0);
    
    vec3 dayFog = vec3(0.72, 0.76, 0.80);
    vec3 duskFog = vec3(0.65, 0.45, 0.32);
    vec3 nightFog = vec3(0.02, 0.03, 0.06);
    
    vec3 fogColor;
    if (sunElevation > 0.2) fogColor = dayFog;
    else if (sunElevation > 0.0) fogColor = mix(duskFog, dayFog, sunElevation / 0.2);
    else if (sunElevation > -0.1) fogColor = mix(nightFog, duskFog, (sunElevation + 0.1) / 0.1);
    else fogColor = nightFog;

    vec3 finalColor = mix(litColor, fogColor, fogFactor);
    FragColor = vec4(finalColor, 1.0);
}
