#version 460 core
layout (location = 0) in vec3 aPos; // the position variable has attribute position 0
layout (location = 1) in vec4 aColor; // the color variable has attribute position 1
  
uniform mat4 model;
uniform mat4 projection;
uniform mat4 view;

uniform mat4 shiftPos; // Reduce the need for computation on CPU (PanickedCircle)

out vec4 vertexColor; // specify a color output to the fragment shader

void main()
{
    gl_Position = projection * view * model * shiftPos * vec4(aPos, 1.0); // set the output position to the transformed vertex position
    vertexColor = aColor; // set the output variable to the input color we got from the vertex data
}