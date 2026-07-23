#version 330 core

// inputs from vertex shader
in vec3 FragPos;
in vec3 Normal;
in float Height;

// output color
out vec4 FragColor;

// uniforms
uniform vec3 lightDir;
uniform vec3 lightColor;
uniform vec3 viewPos;

void main() {
    // normalize surface normal vector
    vec3 norm = normalize(Normal);
    vec3 lightDirection = normalize(-lightDir);

    // ambient lighting component
    vec3 ambient = 0.20 * lightColor;

    // diffuse lighting component with sunset contrast
    float diff = max(dot(norm, lightDirection), 0.0);
    vec3 diffuse = diff * lightColor * 1.4;

    // slope calculation for cliff detection
    float steepness = dot(norm, vec3(0.0, 1.0, 0.0));

    // dark fantasy desaturated color palette
    vec3 grass = vec3(0.10, 0.15, 0.08); // dark dead olive green
    vec3 rock  = vec3(0.28, 0.28, 0.30); // ash grey rock
    vec3 snow  = vec3(0.80, 0.78, 0.75); // pale bone white peaks

    // 1. base color starts as dead vegetation
    vec3 baseColor = grass;

    // 2. steep slopes become ash grey rock cliffs
    if (steepness < 0.70)
        baseColor = mix(rock, grass, clamp(steepness / 0.70, 0.0, 1.0));

    // 3. high elevation ridged peaks get pale bone white snow
    if (Height > 3.0)
        baseColor = mix(baseColor, snow, clamp((Height - 3.0) / 4.0, 0.0, 1.0));

    // combine lighting with terrain colors
    vec3 litColor = (ambient + diffuse) * baseColor;

    // 4. thick muted purple-grey fog calculation
    float distance = length(viewPos - FragPos);
    float fogFactor = clamp((distance - 15.0) / 90.0, 0.0, 1.0);
    vec3 fogColor = vec3(0.08, 0.07, 0.11); // muted purple-grey fog

    // blend terrain into fog
    vec3 finalColor = mix(litColor, fogColor, fogFactor);

    FragColor = vec4(finalColor, 1.0);
}
