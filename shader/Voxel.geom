#version 330
layout(points) in;
layout(triangle_strip, max_vertices = 12) out;

//in  VS_OUT {
//    uint color;
//}   gs_in[];
//
//out GS_OUT {
//    flat uint color;
//}   gs_out;

uniform float voxSize = 0.25f;
uniform mat4 projection;
uniform mat4 view;

void AddQuad(vec4 center, vec4 dy, vec4 dx) {
    //gs_out.color = gs_in[0].color;
    gl_Position = center + (dx - dy);
    EmitVertex();
    //gs_out.color = gs_in[0].color;
    gl_Position = center + (-dx - dy);
    EmitVertex();
    //gs_out.color = gs_in[0].color;
    gl_Position = center + (dx + dy);
    EmitVertex();
    //gs_out.color = gs_in[0].color;
    gl_Position = center + (-dx + dy);
    EmitVertex();
    EndPrimitive();
}

bool IsCulled(vec4 normal) {
    return normal.z > 0;
}

void main() {
    vec4 center = gl_in[0].gl_Position;

    vec4 dx = (projection * view)[0] * voxSize;
    vec4 dy = (projection * view)[1] * voxSize;
    vec4 dz = (projection * view)[2] * voxSize;

    //vec4 dx = vec4(0.8f, 0.0f, 0.0f, 1.0f);
    //vec4 dy = vec4(0.0f, 0.8f, 0.0f, 1.0f);
    //vec4 dz = vec4(0.0f, 0.0f, 0.8f, 1.0f);

    if (/*(gEnabledFaces[0] & 0x01) != 0 && */!IsCulled(dx))
        AddQuad(center + dx, dy, dz);
    if (/*(gEnabledFaces[0] & 0x02) != 0 && */!IsCulled(-dx))
        AddQuad(center - dx, dz, dy);
    if (/*(gEnabledFaces[0] & 0x04) != 0 && */!IsCulled(dy))
        AddQuad(center + dy, dz, dx);
    if (/*(gEnabledFaces[0] & 0x08) != 0 && */!IsCulled(-dy))
        AddQuad(center - dy, dx, dz);
    if (/*(gEnabledFaces[0] & 0x10) != 0 && */!IsCulled(dz))
        AddQuad(center + dz, dx, dy);
    if (/*(gEnabledFaces[0] & 0x20) != 0 && */!IsCulled(-dz))
        AddQuad(center - dz, dy, dx);
}