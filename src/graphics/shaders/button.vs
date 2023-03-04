#version 460 core
layout (location = 0) in vec2 position; 

uniform mat4 model;
    
void main()
{
    gl_Position =  model * vec4(position, 1.0, 1.0);
}