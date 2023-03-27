#version 460 core
layout (location = 0) in vec2 position; 

uniform mat4 model;
uniform vec2 shiftPos; 
uniform float shiftZ; // Allow us save some data and to configure overlapping of ui elements easily.

void main()
{
    gl_Position =  model * vec4(position + shiftPos, shiftZ, 1.0);
}