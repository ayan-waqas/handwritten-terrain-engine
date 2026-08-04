#version 330 core

in vec3 TexCoords;
out vec4 FragColor;

uniform vec3 lightDir;
uniform vec3 lightColor;
uniform float time;
uniform float timeOfDay;
uniform vec3 viewPos;

float random(vec2 st) {
    return fract(sin(dot(st.xy, vec2(12.9898,78.233))) * 43758.5453123);
}

void main() {
    vec3 dir = normalize(TexCoords);
    float height = dir.y;
    vec3 sunDir = normalize(-lightDir);
    float sunElevation = sunDir.y;
    
    vec3 skyZenith, skyHorizon, groundHaze, sunCol, sunHaloCol;
    
    if (sunElevation > 0.2) {
        skyZenith = vec3(0.12, 0.35, 0.82);
        skyHorizon = vec3(0.72, 0.76, 0.80);
        groundHaze = vec3(0.60, 0.64, 0.68);
        sunCol = vec3(1.0, 1.0, 1.0);
        sunHaloCol = vec3(1.0, 0.9, 0.8);
    } else if (sunElevation > 0.0) {
        float t = sunElevation / 0.2;
        skyZenith = mix(vec3(0.4, 0.6, 0.8), vec3(0.12, 0.35, 0.82), t);
        skyHorizon = mix(vec3(0.9, 0.6, 0.4), vec3(0.48, 0.72, 0.92), t);
        groundHaze = mix(vec3(0.6, 0.5, 0.4), vec3(0.40, 0.55, 0.65), t);
        sunCol = mix(vec3(1.0, 0.8, 0.4), vec3(1.0, 1.0, 1.0), t);
        sunHaloCol = mix(vec3(1.0, 0.6, 0.2), vec3(1.0, 0.9, 0.8), t);
    } else if (sunElevation > -0.1) {
        float t = (sunElevation + 0.1) / 0.1;
        skyZenith = mix(vec3(0.01, 0.02, 0.05), vec3(0.4, 0.6, 0.8), t);
        skyHorizon = mix(vec3(0.4, 0.1, 0.3), vec3(0.9, 0.6, 0.4), t);
        groundHaze = mix(vec3(0.01, 0.01, 0.02), vec3(0.6, 0.5, 0.4), t);
        sunCol = mix(vec3(0.8, 0.3, 0.1), vec3(1.0, 0.8, 0.4), t);
        sunHaloCol = mix(vec3(0.6, 0.1, 0.0), vec3(1.0, 0.6, 0.2), t);
    } else {
        skyZenith = vec3(0.01, 0.02, 0.05);
        skyHorizon = vec3(0.02, 0.03, 0.06);
        groundHaze = vec3(0.01, 0.01, 0.02);
        sunCol = vec3(0.0);
        sunHaloCol = vec3(0.0);
    }

    vec3 skyColor = mix(skyHorizon, skyZenith, clamp(pow(max(height, 0.0), 0.6), 0.0, 1.0));
    if (height < 0.0)
        skyColor = mix(skyHorizon, groundHaze, clamp(-height * 3.0, 0.0, 1.0));

    float sunDot = max(dot(dir, sunDir), 0.0);
    float moonDot = max(dot(dir, -sunDir), 0.0);

    float sunDisk = 0.0;
    float sunGlow = 0.0;
    float sunAtmosphere = 0.0;
    if (sunElevation > -0.1) {
        sunDisk = pow(sunDot, 512.0) * 3.0;
        sunGlow = pow(sunDot, 16.0) * 0.45;
        sunAtmosphere = pow(sunDot, 4.0) * 0.25;
    }
    
    vec3 finalSun = sunCol * sunDisk;
    vec3 finalHalo = sunHaloCol * (sunGlow + sunAtmosphere);
    
    float moonDisk = pow(moonDot, 1024.0) * 2.0;
    float moonGlow = pow(moonDot, 32.0) * 0.2;
    vec3 moonColor = vec3(0.8, 0.9, 1.0) * (moonDisk + moonGlow);
    if (sunElevation > -0.1) {
        float moonFade = smoothstep(-0.1, 0.2, sunElevation);
        moonColor *= (1.0 - moonFade);
    }
    
    vec3 starColor = vec3(0.0);
    if (sunElevation < 0.0) {
        float starVal = random(dir.xy * 100.0);
        if (starVal > 0.995) {
            float fade = smoothstep(0.0, -0.1, sunElevation);
            starColor = vec3(1.0) * (starVal - 0.995) * 200.0 * fade * clamp(height, 0.0, 1.0);
        }
    }

    skyColor += finalHalo + finalSun + moonColor + starColor;

    float cloudTime = time * 0.015;
    float cx = dir.x / (max(height, 0.05) + 0.25) + cloudTime;
    float cz = dir.z / (max(height, 0.05) + 0.25) + cloudTime * 0.7;

    float layer1 = sin(cx * 3.5) * cos(cz * 3.0);
    float layer2 = sin(cx * 8.0 + 1.5) * cos(cz * 7.0 + 0.5) * 0.5;
    float layer3 = sin(cx * 16.0) * cos(cz * 14.0) * 0.25;
    
    float cloudNoise = layer1 + layer2 + layer3;
    float cloudMask = smoothstep(0.05, 0.75, cloudNoise) * clamp(height * 5.0, 0.0, 1.0);
    
    float cloudFade = smoothstep(-0.1, 0.2, sunElevation);
    cloudMask *= cloudFade;
    
    vec3 cloudBase = vec3(0.70, 0.78, 0.88) * sunHaloCol;
    vec3 cloudSunLit = vec3(1.0, 0.95, 0.88) * sunCol;
    float cloudSunDot = max(dot(normalize(vec3(layer1, 0.5, layer2)), sunDir), 0.0);
    vec3 finalCloudColor = mix(cloudBase, cloudSunLit, cloudSunDot * 0.7 + 0.3);

    skyColor = mix(skyColor, finalCloudColor, cloudMask * 0.65);

    FragColor = vec4(skyColor, 1.0);
}
