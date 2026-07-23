#version 330 core

in vec3 TexCoords;
out vec4 FragColor;

uniform vec3 lightDir;
uniform float time;

void main() {
    vec3 dir = normalize(TexCoords);
    float height = dir.y;

    // rich atmospheric sky gradient (deep sapphire zenith -> vibrant cyan-gold horizon)
    vec3 skyZenith  = vec3(0.12, 0.35, 0.82);
    vec3 skyHorizon = vec3(0.48, 0.72, 0.92);
    vec3 groundHaze = vec3(0.40, 0.55, 0.65);

    // base sky gradient
    vec3 skyColor = mix(skyHorizon, skyZenith, clamp(pow(max(height, 0.0), 0.6), 0.0, 1.0));

    // below horizon ground haze
    if (height < 0.0)
        skyColor = mix(skyHorizon, groundHaze, clamp(-height * 3.0, 0.0, 1.0));

    // Sun direction and glow
    vec3 sunDir = normalize(-lightDir);
    float sunDot = max(dot(dir, sunDir), 0.0);

    // Sun disc and intense atmospheric sun halo (Rayleigh/Mie glow)
    float sunDisk = pow(sunDot, 512.0);
    float sunGlow = pow(sunDot, 16.0) * 0.45;
    float sunAtmosphere = pow(sunDot, 4.0) * 0.25;

    vec3 sunColor = vec3(1.0, 0.92, 0.75) * sunDisk * 3.0;
    vec3 sunHalo  = vec3(1.0, 0.75, 0.45) * (sunGlow + sunAtmosphere);

    // Multi-layered procedural volumetric clouds
    float cloudTime = time * 0.015;
    float cx = dir.x / (max(height, 0.05) + 0.25) + cloudTime;
    float cz = dir.z / (max(height, 0.05) + 0.25) + cloudTime * 0.7;

    float layer1 = sin(cx * 3.5) * cos(cz * 3.0);
    float layer2 = sin(cx * 8.0 + 1.5) * cos(cz * 7.0 + 0.5) * 0.5;
    float layer3 = sin(cx * 16.0) * cos(cz * 14.0) * 0.25;

    float cloudNoise = layer1 + layer2 + layer3;
    float cloudMask = smoothstep(0.05, 0.75, cloudNoise) * clamp(height * 5.0, 0.0, 1.0);

    // Clouds illuminated by sun with rim lighting and shading
    vec3 cloudBase  = vec3(0.70, 0.78, 0.88);
    vec3 cloudSunLit = vec3(1.0, 0.95, 0.88);
    float cloudSunDot = max(dot(vec3(layer1, 0.5, layer2), sunDir), 0.0);
    vec3 finalCloudColor = mix(cloudBase, cloudSunLit, cloudSunDot * 0.7 + 0.3);

    // Blend sky, halo, clouds, and sun
    skyColor += sunHalo;
    skyColor = mix(skyColor, finalCloudColor, cloudMask * 0.65);
    skyColor += sunColor;

    FragColor = vec4(skyColor, 1.0);
}
