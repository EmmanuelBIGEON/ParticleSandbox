#version 460 core
out vec4 fragColor;

uniform vec3 lineColor;

void main() {
    // fragColor = vec4(lineColor, 1.0);
    fragColor = vec4(1.0, 1.0, 1.0, 1.0);
}