#version 460 core
out vec4 fragColor;

uniform vec3 particleColor;

void main() {
    fragColor = vec4(particleColor, 0.4);
}