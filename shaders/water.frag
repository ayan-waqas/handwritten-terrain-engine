#version 330 core

in vec3 WorldPos;
in vec4 ClipSpacePos;

out vec4 FragColor;

uniform float time;
uniform vec3 lightDir;
uniform vec3 viewPos;
uniform sampler2D reflectionTexture;

void main() {
    // Layered animated wave normal
    float wave1 = sin(WorldPos.x * 0.25 + time * 1.8);
    float wave2 = cos(WorldPos.z * 0.35 + time * 2.2);
    float wave3 = sin(WorldPos.x * 0.6 + WorldPos.z * 0.4 + time * 1.2);
    vec3 waveNormal = normalize(vec3(wave1 * 0.07 + wave3 * 0.04, 1.0, wave2 * 0.07));

    vec3 lightDirection = normalize(-lightDir);
    vec3 viewDir = normalize(viewPos - WorldPos);

    // Fresnel effect for realistic reflection/refraction blend
    float fresnel = pow(1.0 - max(dot(viewDir, waveNormal), 0.0), 3.0);
    fresnel = clamp(fresnel, 0.2, 0.95);

    // Projected clip space coordinates for planar reflection sampling
    vec2 ndc = (ClipSpacePos.xy / ClipSpacePos.w) * 0.5 + 0.5;

    // Distort reflection UV coordinates with wave normal
    vec2 reflectUV = ndc + waveNormal.xz * 0.035;
    reflectUV = clamp(reflectUV, vec2(0.001), vec2(0.999));

    // Sample dynamic planar reflection texture
    vec3 reflectedScene = texture(reflectionTexture, reflectUV).rgb;

    // Intense specular sun reflection (Blinn-Phong)
    vec3 halfDir = normalize(lightDirection + viewDir);
    float spec = pow(max(dot(waveNormal, halfDir), 0.0), 128.0);
    vec3 specular = vec3(1.0, 0.96, 0.85) * spec * 1.2;

    // Vibrant deep blue water base color
    float depthNoise = (wave1 * 0.5 + 0.5) * 0.4 + (wave2 * 0.5 + 0.5) * 0.4;
    vec3 shallowColor = vec3(0.0, 0.72, 0.88);
    vec3 deepColor    = vec3(0.02, 0.22, 0.62);
    vec3 waterBase = mix(deepColor, shallowColor, clamp(depthNoise + 0.2, 0.0, 1.0));

    // Blend base water color with dynamic scene reflection based on Fresnel
    vec3 waterColor = mix(waterBase, reflectedScene, fresnel * 0.75 + 0.15) + specular;

    // Atmospheric fog matching terrain and sky
    float distance = length(viewPos - WorldPos);
    float distFog = clamp((distance - 80.0) / 280.0, 0.0, 1.0);
    float heightFog = exp(-max(WorldPos.y + 8.0, 0.0) * 0.04);
    float fogFactor = clamp(distFog + heightFog * 0.15, 0.0, 1.0);
    vec3 fogColor = vec3(0.48, 0.72, 0.92);

    waterColor = mix(waterColor, fogColor, fogFactor);

    FragColor = vec4(waterColor, 0.90);
}
