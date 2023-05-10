#version 300 es
precision highp float;

out vec4 fragColor;

uniform vec3 elementColor;

void main() {
    fragColor = vec4(elementColor, 1.0);
}