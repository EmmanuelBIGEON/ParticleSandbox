#version 460 core
out vec4 fragColor;

uniform vec3 buttonColor;

void main() {
    fragColor = vec4(buttonColor, 1.0);
}