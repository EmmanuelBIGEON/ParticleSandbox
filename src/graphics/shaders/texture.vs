#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;

out vec3 ourColor;
out vec2 TexCoord;

uniform mat4 model;

void main()
{
    gl_Position = model * vec4(aPos, 1.0); // set the output position to the transformed vertex position
    // gl_Position = vec4(aPos, 1.0); // set the output position to the transformed vertex position
    ourColor = aColor;
    TexCoord = vec2(aTexCoord.x, aTexCoord.y);
}