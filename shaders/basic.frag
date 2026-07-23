#version 330 core

// inputs from vertex shader
in vec3 FragPos;
in vec3 Normal;
in float Height;

// output color
out vec4 FragColor;

// lighting uniforms
uniform vec3 lightDir;
uniform vec3 lightColor;

void main() {
    // normalize surface normal vector
    vec3 norm = normalize(Normal);
    vec3 lightDirection = normalize(-lightDir);

    // ambient lighting component
    vec3 ambient = 0.25 * lightColor;

    // diffuse lighting component
    float diff = max(dot(norm, lightDirection), 0.0);
    vec3 diffuse = diff * lightColor * 1.3;

    // slope calculation for cliff detection
    float steepness = dot(norm, vec3(0.0, 1.0, 0.0));

    // dark fantasy terrain color palette
    vec3 grass = vec3(0.12, 0.20, 0.10); // olive green
    vec3 rock  = vec3(0.35, 0.35, 0.38); // ash grey rock
    vec3 snow  = vec3(0.95, 0.95, 1.00); // pure white snow

    // 1. base color starts as grass
    vec3 baseColor = grass;

    // 2. steep slopes become rock cliffs
    if (steepness < 0.75)
        baseColor = mix(rock, grass, clamp(steepness / 0.75, 0.0, 1.0));

    // 3. high elevation mountain peaks get covered in snow (overrides rock/grass on peaks)
    if (Height > 1.5)
        baseColor = mix(baseColor, snow, clamp((Height - 1.5) / 3.0, 0.0, 1.0));

    // combine lighting with terrain colors
    vec3 result = (ambient + diffuse) * baseColor;
    FragColor = vec4(result, 1.0);
}
