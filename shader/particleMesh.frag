#version 330 core
out vec4 FragColor;

smooth in   vec2 TexCoords;
smooth in   vec2 Normal;
flat in     vec3 Position;
flat in     vec3 Velocity;
flat in     vec4 Color;
flat in     float Rotate;
flat in     float Size;
flat in     float Age;
flat in     float LifeTime;

uniform sampler2D texture_diffuse1;

void main()
{
    vec4 color = texture(texture_diffuse1, TexCoords);
    FragColor =  vec4(color.r, color.g, color.b, 1.f);
}