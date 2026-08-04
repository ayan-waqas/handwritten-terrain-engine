#version 330 core

in vec3 FragPos;
in vec3 Normal;
in float Height;
in float Biome;
in vec4 FragPosLightSpace;

out vec4 FragColor;

uniform vec3 lightDir;
uniform vec3 lightColor;
uniform vec3 viewPos;
uniform float time;
uniform float timeOfDay;
uniform sampler2D shadowMap;

float hash(vec2 p) {
    return fract(sin(dot(p, vec2(12.9898, 78.233))) * 43758.5453);
}

float noiseLayer(vec2 p) {
    vec2 i = floor(p);
    vec2 f = fract(p);
    f = f * f * (3.0 - 2.0 * f);
    float a = hash(i);
    float b = hash(i + vec2(1.0, 0.0));
    float c = hash(i + vec2(0.0, 1.0));
    float d = hash(i + vec2(1.0, 1.0));
    return mix(mix(a, b, f.x), mix(c, d, f.x), f.y);
}

float triplanarNoise(vec3 pos, vec3 norm) {
    vec3 blend = abs(norm);
    blend = normalize(max(blend, 0.00001));
    blend /= (blend.x + blend.y + blend.z);
    
    float nx = noiseLayer(pos.yz);
    float ny = noiseLayer(pos.xz);
    float nz = noiseLayer(pos.xy);
    return nx * blend.x + ny * blend.y + nz * blend.z;
}

float calculateShadow(vec4 fragPosLightSpace, vec3 norm, vec3 lightDirection) {
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    if (projCoords.z > 1.0 || projCoords.x < 0.0 || projCoords.x > 1.0 || projCoords.y < 0.0 || projCoords.y > 1.0)
        return 0.0;

    float currentDepth = projCoords.z;
    float bias = max(0.002 * (1.0 - dot(norm, lightDirection)), 0.0005);

    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);

    for (int x = -1; x <= 1; ++x) {
        for (int y = -1; y <= 1; ++y) {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            float weight = (x == 0 && y == 0) ? 0.25 : ((x == 0 || y == 0) ? 0.125 : 0.0625);
            shadow += (currentDepth - bias > pcfDepth) ? weight : 0.0;
        }
    }
    return shadow;
}

void main() {
    vec3 norm = normalize(Normal);
    vec3 lightDirection = normalize(-lightDir);
    vec3 viewDir = normalize(viewPos - FragPos);
    float sunElevation = lightDirection.y;
    float dayFactor = smoothstep(-0.1, 0.2, sunElevation);

    // Neutral natural ambient light (NO blue tinting!)
    vec3 skyAmbient   = vec3(0.58, 0.58, 0.58);
    vec3 groundAmbient = vec3(0.30, 0.28, 0.24);
    float upWeight = norm.y * 0.5 + 0.5;
    vec3 hemiAmbient = mix(groundAmbient, skyAmbient, upWeight) * 0.42 * dayFactor;
    
    float shadow = calculateShadow(FragPosLightSpace, norm, lightDirection);
    float diff = max(dot(norm, lightDirection), 0.0);
    vec3 diffuse = (1.0 - shadow) * diff * lightColor * 1.15;

    float steepness = dot(norm, vec3(0.0, 1.0, 0.0));

    vec3 grass, rock, snow;

    // Natural Earth Palettes (Zero blue tint!)
    vec3 forestGrass = vec3(0.12, 0.50, 0.14);
    vec3 forestRock  = vec3(0.48, 0.44, 0.38); // Earth granite rock
    vec3 forestSnow  = vec3(1.0, 1.0, 0.98);   // Warm pure snow

    vec3 desertGrass = vec3(0.82, 0.68, 0.28);
    vec3 desertRock  = vec3(0.65, 0.38, 0.18);
    vec3 desertSnow  = vec3(0.95, 0.90, 0.78);

    vec3 tundraGrass = vec3(0.18, 0.46, 0.22); // Crisp alpine green
    vec3 tundraRock  = vec3(0.42, 0.40, 0.38); // Dark mountain granite slate
    vec3 tundraSnow  = vec3(1.0, 1.0, 0.98);   // Pure snow

    float b1 = smoothstep(0.3, 0.38, Biome);
    float b2 = smoothstep(0.6, 0.68, Biome);

    vec3 tempGrass = mix(forestGrass, desertGrass, b1);
    vec3 tempRock  = mix(forestRock, desertRock, b1);
    vec3 tempSnow  = mix(forestSnow, desertSnow, b1);

    grass = mix(tempGrass, tundraGrass, b2);
    rock  = mix(tempRock, tundraRock, b2);
    snow  = mix(tempSnow, tundraSnow, b2);

    float microNoise = triplanarNoise(FragPos * 0.5, norm) * 0.5 + triplanarNoise(FragPos * 2.0, norm) * 0.5;
    rock *= mix(0.88, 1.12, microNoise);

    float rockFactor = 1.0 - smoothstep(0.22, 0.48, steepness);
    vec3 baseColor = mix(grass, rock, rockFactor);

    if (norm.z > 0.3 && steepness > 0.4 && Height < 8.0) {
        float mossWeight = clamp((norm.z - 0.3) * 5.0, 0.0, 1.0) * clamp((steepness - 0.4) * 5.0, 0.0, 1.0);
        baseColor = mix(baseColor, vec3(0.08, 0.22, 0.08), mossWeight * 0.8 * rockFactor);
    }

    float snowFactor = 0.0;
    if (Height > 8.5) {
        snowFactor = smoothstep(8.5, 12.0, Height) * smoothstep(0.6, 0.8, steepness);
        vec3 finalSnow = mix(snow * 0.92, snow, diff * (1.0 - shadow));
        baseColor = mix(baseColor, finalSnow, snowFactor);
    }

    float heightDeriv = length(vec2(dFdx(Height), dFdy(Height)));
    float creviceAO = clamp(steepness * 0.5 + (1.0 - heightDeriv * 0.5), 0.15, 1.0);
    vec3 finalAmbient = hemiAmbient * creviceAO;

    float rim = pow(1.0 - max(dot(viewDir, norm), 0.0), 3.5) * max(dot(norm, lightDirection), 0.0);
    vec3 rimLight = vec3(1.0, 0.90, 0.70) * rim * 0.35 * dayFactor * (1.0 - shadow);

    vec3 litColor = (finalAmbient + diffuse) * baseColor + rimLight;

    vec3 halfDir = normalize(lightDirection + viewDir);

    // Direct Sun Specular Highlights on Mountain Rock Ridges (Peak Lighting!)
    float rockSpecular = pow(max(dot(norm, halfDir), 0.0), 32.0) * rockFactor * 0.25 * (1.0 - shadow) * dayFactor;
    litColor += vec3(1.0, 0.95, 0.85) * rockSpecular * lightColor;

    float snowSpecular = pow(max(dot(norm, halfDir), 0.0), 32.0) * snowFactor * 0.7;
    float iceSpecular = 0.0;
    if (steepness < 0.6 && Height > 8.5) {
        iceSpecular = pow(max(dot(norm, halfDir), 0.0), 128.0) * 0.4 * (1.0 - snowFactor);
    }
    litColor += vec3(1.0, 0.98, 0.92) * (snowSpecular + iceSpecular) * lightColor * (1.0 - shadow);

    float wetSpecular = 0.0;
    if (Height < 4.0 && rockFactor > 0.5) {
        wetSpecular = pow(max(dot(norm, halfDir), 0.0), 64.0) * 0.5 * (1.0 - Height/4.0);
    }
    litColor += vec3(1.0) * wetSpecular * lightColor * dayFactor * (1.0 - shadow);

    // Atmospheric Horizon Fog (Warm Natural Haze - ZERO BLUE TINT!)
    float distance = length(viewPos - FragPos);
    float distFog = clamp((distance - 90.0) / 320.0, 0.0, 1.0);
    float heightFog = exp(-max(FragPos.y + 8.0, 0.0) * 0.04);
    float fogFactor = clamp(distFog + heightFog * 0.12, 0.0, 1.0);
    
    vec3 dayFog = vec3(0.72, 0.76, 0.80); // Soft warm natural atmospheric haze
    vec3 duskFog = vec3(0.68, 0.48, 0.35);
    vec3 nightFog = vec3(0.02, 0.03, 0.06);
    
    vec3 fogColor;
    if (sunElevation > 0.2) fogColor = dayFog;
    else if (sunElevation > 0.0) fogColor = mix(duskFog, dayFog, sunElevation / 0.2);
    else if (sunElevation > -0.1) fogColor = mix(nightFog, duskFog, (sunElevation + 0.1) / 0.1);
    else fogColor = nightFog;

    vec3 finalColor = mix(litColor, fogColor, fogFactor);
    FragColor = vec4(finalColor, 1.0);
}
