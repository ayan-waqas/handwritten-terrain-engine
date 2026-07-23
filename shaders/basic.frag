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
    vec3 ambient = 0.22 * lightColor;

    // diffuse lighting component
    float diff = max(dot(norm, lightDirection), 0.0);
    vec3 diffuse = diff * lightColor * 1.3;

    // slope calculation for cliff detection
    float steepness = dot(norm, vec3(0.0, 1.0, 0.0));

    // dark fantasy terrain color palette
    vec3 grass = vec3(0.12, 0.20, 0.10); // deep olive green
    vec3 rock  = vec3(0.35, 0.35, 0.38); // ash grey rock
    vec3 snow  = vec3(0.95, 0.95, 1.00); // pure white snow

    // 1. base color starts as grass
    vec3 baseColor = grass;

    // 2. steep slopes become rock cliffs
    if (steepness < 0.75)
        baseColor = mix(rock, grass, clamp(steepness / 0.75, 0.0, 1.0));

    // 3. high elevation mountain peaks get covered in snow
    if (Height > 1.5)
        baseColor = mix(baseColor, snow, clamp((Height - 1.5) / 3.0, 0.0, 1.0));

    // combine lighting with terrain colors
    vec3 litColor = (ambient + diffuse) * baseColor;

    // 4. atmospheric distance fog calculation
    float distance = length(viewPos - FragPos);
    float fogFactor = clamp((distance - 20.0) / 120.0, 0.0, 1.0);
    vec3 fogColor = vec3(0.10, 0.12, 0.16); // dark fantasy slate fog

    // blend terrain into fog
    vec3 finalColor = mix(litColor, fogColor, fogFactor);

    FragColor = vec4(finalColor, 1.0);
}
