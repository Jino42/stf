#version 400 core
layout (location = 0) in int aIsAlive;
layout (location = 1) in vec3 aPosition;
layout (location = 2) in vec4 aColor;

flat out  vec4 gColor;
flat out  int gIsAlive;

uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * (vec4(aPosition, 1.f));

        gColor = aColor;
    gIsAlive = aIsAlive;
}