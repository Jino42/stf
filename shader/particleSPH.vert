#version 330 core
layout (location = 0) in vec3 aVertexPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

layout (location = 3) in vec3 aPosition;
layout (location = 4) in vec3 aVelocity;
layout (location = 5) in float aPressure;
layout (location = 6) in float aDensity;
layout (location = 7) in float aMass;
layout (location = 8) in int aFlag;

smooth out vec2 TexCoords;
smooth out vec3 Normal;
flat out vec3 Position;
flat out vec3 Velocity;
flat out float Pressure;
flat out float Density;
flat out float Mass;
flat out int Flag;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{

    gl_Position = projection * view * (model * vec4(aVertexPosition, 1.0) + vec4(aPosition, 0.f));
    Normal = aNormal;
    TexCoords = aTexCoords;

    Position = vec3(model * vec4(aVertexPosition + aPosition, 1.f));
    Velocity = aVelocity;
    Pressure = aPressure;
    Density = aDensity;
    Mass = aMass;
    Flag = aFlag;
}