#version 400 core

layout (points) in;
layout (points, max_vertices = 1) out;

flat in  vec4 gColor[];
flat in  int gIsAlive[];

flat out     vec4 fColor;

void main() {

    if (gIsAlive[0] == 0) {
        EndPrimitive();
        return ;
    }

    gl_Position = gl_in[0].gl_Position;
    fColor = gColor[0];
    EmitVertex();

    EndPrimitive();
}