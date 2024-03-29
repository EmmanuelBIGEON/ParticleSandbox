#version 300 es
layout (location = 0) in vec2 position; 

uniform mat4 model;
uniform vec2 shiftPos; 
    
void main()
{
    gl_Position =  model * vec4(position + shiftPos, -0.1, 1.0);
}