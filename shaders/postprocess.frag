#version 330 core

in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D screenTexture;
uniform vec2 sunScreenPos;
uniform float sunInView;

// ACES Filmic Tone Mapping Curve (Cinematic color grading)
vec3 acesFilm(vec3 x) {
    float a = 2.51;
    float b = 0.03;
    float c = 2.43;
    float d = 0.59;
    float e = 0.14;
    return clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0, 1.0);
}

void main() {
    vec3 color = texture(screenTexture, TexCoords).rgb;

    // 1. Bloom: extract and blur bright pixels
    vec3 bloom = vec3(0.0);
    vec2 texelSize = 1.0 / vec2(textureSize(screenTexture, 0));

    for (int x = -2; x <= 2; ++x) {
        for (int y = -2; y <= 2; ++y) {
            vec2 offset = vec2(float(x), float(y)) * texelSize * 2.5;
            vec3 s = texture(screenTexture, TexCoords + offset).rgb;
            float brightness = dot(s, vec3(0.2126, 0.7152, 0.0722));
            if (brightness > 0.65)
                bloom += s * (brightness - 0.65);
        }
    }
    bloom /= 25.0;
    color += bloom * 0.5;

    // 2. Volumetric God Rays (radial blur sun shafts)
    if (sunInView > 0.5) {
        const int NUM_SAMPLES = 24;
        float density = 0.85;
        float weight = 0.045;
        float decay = 0.95;

        vec2 deltaTexCoord = (TexCoords - sunScreenPos);
        deltaTexCoord *= 1.0 / float(NUM_SAMPLES) * density;

        vec2 sampleTexCoord = TexCoords;
        float illuminationDecay = 1.0;
        vec3 godRays = vec3(0.0);

        for (int i = 0; i < NUM_SAMPLES; ++i) {
            sampleTexCoord -= deltaTexCoord;
            vec3 s = texture(screenTexture, clamp(sampleTexCoord, vec2(0.0), vec2(1.0))).rgb;
            float luminance = dot(s, vec3(0.299, 0.587, 0.114));

            if (luminance > 0.58)
                s *= (luminance - 0.58) * 2.2;
            else
                s = vec3(0.0);

            godRays += s * illuminationDecay * weight;
            illuminationDecay *= decay;
        }

        vec3 godRayColor = vec3(1.0, 0.88, 0.62) * godRays * 1.6;
        color += godRayColor;
    }

    // 3. ACES Filmic Tone Mapping (Cinematic contrast)
    color = acesFilm(color * 1.1);

    // 4. Vignette: subtle darkening at screen edges
    float dist = length(TexCoords - vec2(0.5));
    float vignette = 1.0 - smoothstep(0.55, 1.15, dist);
    color *= vignette;

    FragColor = vec4(color, 1.0);
}
