#version 300 es
precision highp float;
out vec4 fragColor;

uniform vec3 lineColor;

void main() {
    fragColor = vec4(lineColor, 1.0);
}