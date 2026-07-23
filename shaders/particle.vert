#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in float aPhase;
layout (location = 3) in float aSize;

out vec3 particleColor;
out float particleAlpha;

uniform mat4 projection;
uniform mat4 view;
uniform float time;
uniform vec3 camPos;

void main() {
    vec3 pos = aPos;

    // Wrap particle positions around camera so particles follow player
    pos.x = camPos.x + mod(pos.x - camPos.x + 100.0, 200.0) - 100.0;
    pos.z = camPos.z + mod(pos.z - camPos.z + 100.0, 200.0) - 100.0;

    // Gentle 3D floating & drifting motion
    pos.y += sin(time * 1.5 + aPhase) * 1.2;
    pos.x += cos(time * 0.8 + aPhase) * 0.8;
    pos.z += sin(time * 0.6 + aPhase) * 0.8;

    vec4 viewPos = view * vec4(pos, 1.0);
    float dist = length(viewPos.xyz);

    // Distance attenuation for particle point size
    gl_PointSize = clamp(aSize * 30.0 / max(dist, 1.0), 2.0, 36.0);

    // Pulsing brightness alpha
    float pulse = sin(time * 2.5 + aPhase) * 0.3 + 0.7;
    particleAlpha = pulse * clamp((80.0 - dist) / 70.0, 0.0, 1.0);
    particleColor = aColor;

    gl_Position = projection * viewPos;
}
