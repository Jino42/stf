#version 330 core

layout (points) in;
layout (line_strip, max_vertices = 6) out;



smooth  in  vec2    gTexCoords[];
smooth  in  vec3    gNormal[];
flat    in  vec3    gPosition[];
flat    in  vec3    gVelocity[];
flat    in  float   gPressure[];
flat    in  float   gDensity[];
flat    in  float   gMass[];
flat    in  int     gFlag[];
flat    in  vec3    gForcePressure[];
flat    in  vec3    gForceViscosity[];

flat out vec3 fColor;

uniform int flag;

void main() {

    if ((flag & (1 << 2)) != 0)
    {
        fColor = vec3(1.0f, 0.0f, 0.0f);
        gl_Position = gl_in[0].gl_Position;
        EmitVertex();
        float q = clamp(length(gVelocity[0]) / 10.f, 0.f, 1.f);
        float p = 3.f;
        gl_Position = gl_in[0].gl_Position + vec4(normalize(gVelocity[0]) * q * p, 0.0f);
        EmitVertex();
    }
    
    if ((flag & (1 << 4)) != 0)
    {
        fColor = vec3(0.0f, 1.0f, 0.0f);
        gl_Position = gl_in[0].gl_Position;
        EmitVertex();
        float q = clamp(length(gForcePressure[0]) / 250.f, 0.f, 1.f);
        float p = 3.f;
        gl_Position = gl_in[0].gl_Position + vec4(normalize(gForcePressure[0]) * q * p, 0.0f);
        EmitVertex();
    }
    
    if ((flag & (1 << 3)) != 0)
    {
        fColor = vec3(0.0f, 0.0f, 1.0f);
        gl_Position = gl_in[0].gl_Position;
        EmitVertex();
        gl_Position = gl_in[0].gl_Position + vec4(gForceViscosity[0], 0.0f);
        EmitVertex();
    }

    EndPrimitive();
}