#version 330 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

smooth in vec2 gTexCoords[];
smooth in vec3 gNormal[];
flat in vec3 gPosition[];
flat in  vec3 gVelocity[];
flat in  vec4 gColor[];
flat in  int gIsAlive[];
smooth in vec2 gSpriteOffset1[];
smooth in vec2 gSpriteOffset2[];
flat in float gSpriteBlend[];

smooth out   vec2 fTexCoords;
smooth out   vec3 fNormal;
flat out     vec3 fPosition;
flat out     vec3 fVelocity;
flat out     vec4 fColor;
flat out     int fIsAlive;
smooth out vec2 fSpriteOffset1;
smooth out vec2 fSpriteOffset2;
flat out float fSpriteBlend;

void build_house(vec4 position)
{


    if (gIsAlive[0] == 0) {
        EndPrimitive();
        return ;
    }


    gl_Position = gl_in[0].gl_Position;
    fTexCoords = gTexCoords[0];
    fNormal = gNormal[0];
    fPosition = gPosition[0];
    fVelocity = gVelocity[0];
    fColor = gColor[0];
    fIsAlive = gIsAlive[0];
    fSpriteOffset1 = gSpriteOffset1[0];
    fSpriteOffset2 = gSpriteOffset2[0];
    fSpriteBlend = gSpriteBlend[0];
    EmitVertex();

    gl_Position = gl_in[1].gl_Position;
    fTexCoords = gTexCoords[1];
    fNormal = gNormal[1];
    fPosition = gPosition[1];
    fVelocity = gVelocity[1];
    fColor = gColor[1];
    fIsAlive = gIsAlive[1];
    fSpriteOffset1 = gSpriteOffset1[1];
    fSpriteOffset2 = gSpriteOffset2[1];
    fSpriteBlend = gSpriteBlend[1];
    EmitVertex();

    gl_Position = gl_in[2].gl_Position;
    fTexCoords = gTexCoords[2];
    fNormal = gNormal[2];
    fPosition = gPosition[2];
    fVelocity = gVelocity[2];
    fColor = gColor[2];
    fIsAlive = gIsAlive[2];
    fSpriteOffset1 = gSpriteOffset1[2];
    fSpriteOffset2 = gSpriteOffset2[2];
    fSpriteBlend = gSpriteBlend[2];
    EmitVertex();

    EndPrimitive();
}

void main() {
    build_house(gl_in[0].gl_Position);
}