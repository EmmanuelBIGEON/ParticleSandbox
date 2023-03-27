#version 460 core
out vec4 fragColor;

uniform vec3 elementColor;

void main() {
    fragColor = vec4(elementColor, 1.0);
}