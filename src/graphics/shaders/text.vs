#version 460 core
layout (location = 0) in vec4 vertex;
  
uniform mat4 model;
uniform mat4 view;

out vec2 TexCoords;

void main()
{
    gl_Position = view * model * vec4(vertex.xy, 0.0, 1.0);
    TexCoords = vertex.zw;
}