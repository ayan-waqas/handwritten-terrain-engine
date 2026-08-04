#version 330 core

in vec3 vertColor;
in vec3 WorldPos;
in vec4 FragPosLightSpace;

out vec4 FragColor;

uniform vec3 lightDir;
uniform vec3 lightColor;
uniform vec3 viewPos;
uniform float time;
uniform float timeOfDay;
uniform sampler2D shadowMap;

float calculateShadow(vec4 fragPosLightSpace, vec3 norm, vec3 lightDirection) {
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    if (projCoords.z > 1.0 || projCoords.x < 0.0 || projCoords.x > 1.0 || projCoords.y < 0.0 || projCoords.y > 1.0)
        return 0.0;

    float currentDepth = projCoords.z;
    float bias = max(0.003 * (1.0 - dot(norm, lightDirection)), 0.0008);

    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);

    for (int x = -1; x <= 1; ++x) {
        for (int y = -1; y <= 1; ++y) {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;
    return shadow;
}

void main() {
    vec3 norm = normalize(cross(dFdx(WorldPos), dFdy(WorldPos)));
    vec3 lightDirection = normalize(-lightDir);
    vec3 viewDir = normalize(viewPos - WorldPos);

    float sunElevation = lightDirection.y;
    float dayFactor = smoothstep(-0.1, 0.2, sunElevation);

    float shadow = calculateShadow(FragPosLightSpace, norm, lightDirection);

    float wrapDiff = max(dot(norm, lightDirection) * 0.6 + 0.4, 0.0);
    vec3 diffuse = (1.0 - shadow) * wrapDiff * lightColor * 1.05 * dayFactor;

    vec3 skyAmbient   = vec3(0.45, 0.68, 0.95);
    vec3 groundAmbient = vec3(0.20, 0.26, 0.18);
    float upWeight = norm.y * 0.5 + 0.5;
    vec3 ambient = mix(groundAmbient, skyAmbient, upWeight) * 0.45 * dayFactor;
    ambient += vec3(0.02, 0.04, 0.08) * (1.0 - dayFactor);

    float rim = pow(1.0 - max(dot(viewDir, norm), 0.0), 3.0) * max(dot(norm, lightDirection), 0.0);
    vec3 rimLight = vec3(1.0, 0.90, 0.65) * rim * 0.35 * dayFactor * (1.0 - shadow);

    vec3 litColor = (ambient + diffuse) * vertColor + rimLight;

    float distance = length(viewPos - WorldPos);
    float distFog = clamp((distance - 80.0) / 280.0, 0.0, 1.0);
    float heightFog = exp(-max(WorldPos.y + 8.0, 0.0) * 0.04);
    float fogFactor = clamp(distFog + heightFog * 0.15, 0.0, 1.0);
    
    vec3 dayFog = vec3(0.48, 0.72, 0.92);
    vec3 duskFog = vec3(0.6, 0.4, 0.3);
    vec3 nightFog = vec3(0.02, 0.03, 0.06);
    
    vec3 fogColor;
    if (sunElevation > 0.2) fogColor = dayFog;
    else if (sunElevation > 0.0) fogColor = mix(duskFog, dayFog, sunElevation / 0.2);
    else if (sunElevation > -0.1) fogColor = mix(nightFog, duskFog, (sunElevation + 0.1) / 0.1);
    else fogColor = nightFog;

    vec3 finalColor = mix(litColor, fogColor, fogFactor);
    FragColor = vec4(finalColor, 1.0);
}
