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
layout (location = 9) in vec3 aForcePressure;
layout (location = 10) in vec3 aForceViscosity;

smooth out vec2 gTexCoords;
smooth out vec3 gNormal;
flat out vec3 gPosition;
flat out vec3 gVelocity;
flat out float gPressure;
flat out float gDensity;
flat out float gMass;
flat out int   gFlag;
flat out vec3   gForcePressure;
flat out vec3   gForceViscosity;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{

    gl_Position = projection * view * (model * vec4(aVertexPosition, 1.0) + vec4(aPosition, 0.f));

    gNormal = aNormal;
    gTexCoords = aTexCoords;

    gPosition = vec3(model * vec4(aVertexPosition + aPosition, 1.f));
    gVelocity = aVelocity;
    gPressure = aPressure;
    gDensity = aDensity;
    gMass = aMass;
    gFlag = aFlag;
    gForcePressure = aForcePressure;
    gForceViscosity = aForceViscosity;
}