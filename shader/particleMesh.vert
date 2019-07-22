#version 330 core
layout (location = 0) in vec3 aVertexPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

layout (location = 3) in vec3 aPosition;
layout (location = 4) in vec3 aVelocity;
layout (location = 5) in vec4 aColor;
layout (location = 6) in float aRotate;
layout (location = 7) in float aSize;
layout (location = 8) in float aAge;
layout (location = 9) in float aLifeTime;

smooth out vec2 TexCoords;
smooth out vec3 Normal;
flat out vec3 Position;
flat out  vec3 Velocity;
flat out  vec4 Color;
flat out  float Rotate;
flat out  float Size;
flat out  float Age;
flat out  float LifeTime;

uniform mat4 view;
uniform mat4 projection;

void main()
{

    gl_Position = projection * view * vec4(aVertexPosition + aPosition, 1.0);
    Normal = aNormal;
    TexCoords = aTexCoords;
    Position = aVertexPosition + aPosition;
    Velocity = aVelocity;
    Color = aColor;
    Rotate = aRotate;
    Size = aSize;
    Age = aAge;
    LifeTime = aLifeTime;
}