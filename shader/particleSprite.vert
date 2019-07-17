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
layout (location = 10) in vec2 aSpriteOffset1;
layout (location = 11) in vec2 aSpriteOffset2;
layout (location = 12) in float aSpriteBlend;

smooth out vec2 TexCoords;
smooth out vec3 Normal;
flat out vec3 Position;


flat out  vec3 Velocity;
flat out  vec4 Color;
flat out  float Rotate;
flat out  float Size;
flat out  float Age;
flat out  float LifeTime;
smooth out vec2 SpriteOffset1;
smooth out vec2 SpriteOffset2;
flat out float SpriteBlend;

uniform mat4 view;
uniform mat4 projection;

uniform uint numberOfRows;

void main()
{

    vec3 worldSpaceCameraRight = vec3(view[0][0], view[1][0], view[2][0]);
    vec3 worldSpaceCameraUp = vec3(view[0][1], view[1][1], view[2][1]);

    vec3 worldSpaceVertexPosition= aPosition
    + worldSpaceCameraRight * aVertexPosition.x * aSize
    + worldSpaceCameraUp * aVertexPosition.y * aSize;

    gl_Position = projection * view *  vec4(worldSpaceVertexPosition, 1.0);
    Normal = aNormal;
    TexCoords = aTexCoords;
    Position = aVertexPosition + aPosition;
    Velocity = aVelocity;
    Color = aColor;
    Rotate = aRotate;
    Size = aSize;
    Age = aAge;
    LifeTime = aLifeTime;
    SpriteOffset1 = aSpriteOffset1;
    SpriteOffset2 = aSpriteOffset2;
    SpriteBlend = aSpriteBlend;

    TexCoords /= numberOfRows;
    TexCoords = aTexCoords / numberOfRows;
    SpriteOffset1 = TexCoords + aSpriteOffset1;
    SpriteOffset2 = TexCoords + aSpriteOffset2;
}