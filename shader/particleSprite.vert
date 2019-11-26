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
layout (location = 10) in int aIsAlive;
layout (location = 11) in vec2 aSpriteOffset1;
layout (location = 12) in vec2 aSpriteOffset2;
layout (location = 13) in float aSpriteBlend;

smooth out vec2 gTexCoords;
smooth out vec3 gNormal;
flat out vec3 gPosition;
flat out  vec3 gVelocity;
flat out  vec4 gColor;
flat out  int gIsAlive;
smooth out vec2 gSpriteOffset1;
smooth out vec2 gSpriteOffset2;
flat out float gSpriteBlend;

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

    if (aIsAlive == 0)
        gl_Position = projection * view *  (vec4(worldSpaceVertexPosition, 1.0));
    else
        gl_Position = projection * view *  vec4(worldSpaceVertexPosition, 1.0);
    gNormal = aNormal;
    gTexCoords = aTexCoords;
    gPosition = aVertexPosition + aPosition;
    gVelocity = aVelocity;
    gColor = aColor;
    gIsAlive = aIsAlive;
    gSpriteOffset1 = aSpriteOffset1;
    gSpriteOffset2 = aSpriteOffset2;
    gSpriteBlend = aSpriteBlend;

    gTexCoords /= numberOfRows;
    gTexCoords = aTexCoords / numberOfRows;
    gSpriteOffset1 = gTexCoords + aSpriteOffset1;
    gSpriteOffset2 = gTexCoords + aSpriteOffset2;
}