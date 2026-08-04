#version 330 core

in vec3 particleColor;
in float particleAlpha;
out vec4 FragColor;

uniform float timeOfDay;

void main() {
    vec2 coord = gl_PointCoord - vec2(0.5);
    float distSq = dot(coord, coord);
    if (distSq > 0.25)
        discard;

    float alpha = (1.0 - sqrt(distSq) * 2.0) * particleAlpha;

    float nightFactor = 0.0;
    if (timeOfDay <= 5.0 || timeOfDay >= 19.0)
        nightFactor = 1.0;
    if (timeOfDay > 5.0 && timeOfDay < 7.0)
        nightFactor = 1.0 - (timeOfDay - 5.0) / 2.0;
    if (timeOfDay > 17.0 && timeOfDay < 19.0)
        nightFactor = (timeOfDay - 17.0) / 2.0;

    vec3 nightColor = vec3(1.0, 0.7, 0.1) * 3.0;
    vec3 dayColor = vec3(1.0, 1.0, 1.0) * 0.8;

    vec3 color = mix(dayColor, nightColor, nightFactor) * particleColor;
    alpha *= mix(0.5, 1.5, nightFactor);

    FragColor = vec4(color, alpha);
}
