#version 330 core

in vec3 WorldPos;
in vec4 ClipSpacePos;

out vec4 FragColor;

uniform float time;
uniform vec3 lightDir;
uniform vec3 viewPos;
uniform sampler2D reflectionTexture;
uniform float timeOfDay;

void main() {
    float wave1 = sin(WorldPos.x * 0.25 + time * 1.8);
    float wave2 = cos(WorldPos.z * 0.35 + time * 2.2);
    float wave3 = sin(WorldPos.x * 0.6 + WorldPos.z * 0.4 + time * 1.2);
    vec3 waveNormal = normalize(vec3(wave1 * 0.07 + wave3 * 0.04, 1.0, wave2 * 0.07));

    vec3 lightDirection = normalize(-lightDir);
    vec3 viewDir = normalize(viewPos - WorldPos);

    float sunElevation = lightDirection.y;
    float dayFactor = smoothstep(-0.1, 0.2, sunElevation);

    float fresnel = pow(1.0 - max(dot(viewDir, waveNormal), 0.0), 3.0);
    fresnel = clamp(fresnel, 0.2, 0.95);

    vec2 ndc = (ClipSpacePos.xy / ClipSpacePos.w) * 0.5 + 0.5;
    vec2 reflectUV = ndc + waveNormal.xz * 0.035;
    reflectUV = clamp(reflectUV, vec2(0.001), vec2(0.999));
    vec3 reflectedScene = texture(reflectionTexture, reflectUV).rgb;

    vec3 halfDir = normalize(lightDirection + viewDir);
    float spec = pow(max(dot(waveNormal, halfDir), 0.0), 128.0);
    vec3 specular = vec3(1.0, 0.96, 0.85) * spec * 1.2 * dayFactor;

    vec3 moonDir = normalize(vec3(-lightDirection.x, max(-lightDirection.y, 0.0), -lightDirection.z));
    vec3 moonHalfDir = normalize(moonDir + viewDir);
    float moonSpec = pow(max(dot(waveNormal, moonHalfDir), 0.0), 128.0);
    vec3 moonSpecular = vec3(0.5, 0.7, 1.0) * moonSpec * 0.5 * (1.0 - dayFactor);

    float depthNoise = (wave1 * 0.5 + 0.5) * 0.4 + (wave2 * 0.5 + 0.5) * 0.4;
    vec3 shallowColor = vec3(0.0, 0.72, 0.88);
    vec3 deepColor    = vec3(0.02, 0.22, 0.62);
    vec3 nightColor   = vec3(0.01, 0.04, 0.10);

    vec3 waterBaseDay = mix(deepColor, shallowColor, clamp(depthNoise + 0.2, 0.0, 1.0));
    vec3 waterBase = mix(nightColor, waterBaseDay, dayFactor);

    vec3 waterColor = mix(waterBase, reflectedScene, fresnel * 0.75 + 0.15) + specular + moonSpecular;

    float distance = length(viewPos - WorldPos);
    float distFog = clamp((distance - 80.0) / 280.0, 0.0, 1.0);
    float heightFog = exp(-max(WorldPos.y + 8.0, 0.0) * 0.04);
    float fogFactor = clamp(distFog + heightFog * 0.15, 0.0, 1.0);

    vec3 fogDay = vec3(0.48, 0.72, 0.92);
    vec3 fogSunset = vec3(0.8, 0.5, 0.3);
    vec3 fogNight = vec3(0.02, 0.05, 0.12);

    vec3 currentFog = mix(fogNight, mix(fogSunset, fogDay, smoothstep(0.0, 0.3, sunElevation)), dayFactor);

    waterColor = mix(waterColor, currentFog, fogFactor);

    FragColor = vec4(waterColor, 0.85);
}
