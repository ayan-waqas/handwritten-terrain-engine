#version 330 core

in vec3 FragPos;
in vec3 Normal;
in float Height;
in float Biome;

out vec4 FragColor;

uniform vec3 lightDir;
uniform vec3 lightColor;
uniform vec3 viewPos;

void main() {
    vec3 norm = normalize(Normal);
    vec3 lightDirection = normalize(-lightDir);
    vec3 viewDir = normalize(viewPos - FragPos);

    // Hemispheric Sky/Ground Ambient Lighting (cool azure sky above, warm earth below)
    vec3 skyAmbient   = vec3(0.45, 0.68, 0.95);
    vec3 groundAmbient = vec3(0.22, 0.28, 0.20);
    float upWeight = norm.y * 0.5 + 0.5;
    vec3 hemiAmbient = mix(groundAmbient, skyAmbient, upWeight) * 0.45;

    // Direct Sunlight
    float diff = max(dot(norm, lightDirection), 0.0);
    vec3 diffuse = diff * lightColor * 1.1;

    // Slope calculation: steepness is dot product with world UP (0,1,0)
    float steepness = dot(norm, vec3(0.0, 1.0, 0.0));

    // === Biome Palettes with Vibrant Fantasy Colors ===
    vec3 grass, rock, snow;

    // 1. Forest Biome: Vibrant emerald green, rich dark earth rock
    vec3 forestGrass = vec3(0.12, 0.58, 0.15);
    vec3 forestRock  = vec3(0.35, 0.28, 0.22);
    vec3 forestSnow  = vec3(0.95, 0.96, 1.00);

    // 2. Savannah / Desert Biome: Warm golden amber grass, terracotta rock
    vec3 desertGrass = vec3(0.75, 0.62, 0.20);
    vec3 desertRock  = vec3(0.58, 0.35, 0.20);
    vec3 desertSnow  = vec3(0.90, 0.85, 0.75);

    // 3. Tundra / Alpine Biome: Mint-teal alpine moss, slate grey rock
    vec3 tundraGrass = vec3(0.20, 0.52, 0.42);
    vec3 tundraRock  = vec3(0.38, 0.42, 0.50);
    vec3 tundraSnow  = vec3(0.96, 0.98, 1.00);

    if (Biome < 0.33) {
        float t = clamp(Biome / 0.33, 0.0, 1.0);
        grass = mix(forestGrass, desertGrass, t);
        rock  = mix(forestRock,  desertRock,  t);
        snow  = mix(forestSnow,  desertSnow,  t);
    }
    else if (Biome < 0.67) {
        float t = clamp((Biome - 0.33) / 0.34, 0.0, 1.0);
        grass = mix(desertGrass, tundraGrass, t);
        rock  = mix(desertRock,  tundraRock,  t);
        snow  = mix(desertSnow,  tundraSnow,  t);
    }
    else {
        grass = tundraGrass;
        rock  = tundraRock;
        snow  = tundraSnow;
    }

    // === Realistic Geological Rock Strata & Micro-Detail ===
    float strataPattern = sin(FragPos.y * 1.6 + sin(FragPos.x * 0.15 + FragPos.z * 0.15) * 2.0) * 0.5 + 0.5;
    rock *= 0.82 + strataPattern * 0.35;

    float erosionPattern = sin(FragPos.x * 2.5 + FragPos.z * 2.5) * 0.5 + 0.5;
    rock *= 0.88 + erosionPattern * 0.24;

    float rockFactor = 1.0 - smoothstep(0.22, 0.48, steepness);
    vec3 baseColor = mix(grass, rock, rockFactor);

    float snowFactor = 0.0;
    if (Height > 8.5) {
        snowFactor = smoothstep(8.5, 12.0, Height) * clamp((steepness - 0.15) / 0.3, 0.0, 1.0);
        baseColor = mix(baseColor, snow, snowFactor);
    }

    float detailNoise = fract(sin(dot(FragPos.xz, vec2(12.9898, 78.233))) * 43758.5453);
    baseColor *= 0.94 + detailNoise * 0.12;

    // Ambient Occlusion / Crevice Shadowing
    float creviceAO = clamp(steepness * 0.55 + 0.45, 0.30, 1.0);
    vec3 finalAmbient = hemiAmbient * creviceAO;

    // Back-lit Golden Rim Highlight on Mountain Ridges
    float rim = pow(1.0 - max(dot(viewDir, norm), 0.0), 3.5) * max(dot(norm, lightDirection), 0.0);
    vec3 rimLight = vec3(1.0, 0.88, 0.65) * rim * 0.40;

    vec3 litColor = (finalAmbient + diffuse) * baseColor + rimLight;

    // Sun Specular Glint on Snow Peaks
    vec3 halfDir = normalize(lightDirection + viewDir);
    float snowSpecular = pow(max(dot(norm, halfDir), 0.0), 32.0) * snowFactor * 0.7;
    litColor += vec3(1.0, 0.98, 0.92) * snowSpecular * lightColor;

    // Soft atmospheric distance fog matching sky horizon tint
    float distance = length(viewPos - FragPos);
    float distFog = clamp((distance - 80.0) / 280.0, 0.0, 1.0);
    float heightFog = exp(-max(FragPos.y + 8.0, 0.0) * 0.04);
    float fogFactor = clamp(distFog + heightFog * 0.15, 0.0, 1.0);
    vec3 fogColor = vec3(0.48, 0.72, 0.92);

    vec3 finalColor = mix(litColor, fogColor, fogFactor);
    FragColor = vec4(finalColor, 1.0);
}
