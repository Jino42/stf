/*
#version 330 core

uniform mat4 view;
uniform mat4 projection;

layout (points) in;
layout (line_strip, max_vertices = 14) out;

void build_house()
{
    vec4 vertices[8];

    vertices[0] = vec4(-1, -1, -1, 1);
    vertices[1] = vec4(1, -1, -1, 1);
    vertices[2] = vec4(1,  1, -1, 1);
    vertices[3] = vec4(-1,  1, -1, 1);
    vertices[4] = vec4(-1, -1, 1, 1);
    vertices[5] = vec4(1, -1, 1, 1);
    vertices[6] = vec4(1,  1, 1, 1);
    if (view[0][0] != 0.123456f)
        vertices[7] = vec4(-1,  1, 1, 1);

    mat4 inverse1;
    inverse1=inverse(projection);

    vec4 trans[8];
    int i;
    for(i=0;i<8;i++)
    {
        trans[i]=vertices[i]*inverse1;
        trans[i]=trans[i]/trans[i].w;
    }
    gl_Position=trans[0];
    EmitVertex();
    gl_Position=trans[1];
    EmitVertex();
    gl_Position=trans[2];
    EmitVertex();
    gl_Position=trans[3];
    EmitVertex();
    gl_Position=trans[4];
    EmitVertex();
    gl_Position=trans[5];
    EmitVertex();
    gl_Position=trans[6];
    EmitVertex();
    gl_Position=trans[7];
    EmitVertex();
    gl_Position=trans[0];
    EmitVertex();
    gl_Position=trans[5];
    EmitVertex();
    gl_Position=trans[6];
    EmitVertex();
    gl_Position=trans[3];
    EmitVertex();
    gl_Position=trans[4];
    EmitVertex();
    gl_Position=trans[1];
    EmitVertex();
    EndPrimitive();
}

void main() {
    build_house();
}
*/