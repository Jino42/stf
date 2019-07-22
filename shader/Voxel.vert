#version 330 core

layout (location = 0) in vec3 aPosition3;
layout (location = 1) in uint aColor;

//out VS_OUT {
//    uint color;
//}   vs_out;

uniform mat4 view;
uniform mat4 projection;
uniform int positionX;
uniform int positionZ;

void main()
{
    vec3 position3 = aPosition3;
    position3.x += positionX;
    position3.z += positionZ;

    gl_Position = projection * view  * vec4(
    position3,
    1.0f);
    //vs_out.color = aColor;
}