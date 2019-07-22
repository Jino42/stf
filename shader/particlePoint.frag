#version 330 core
out vec4 FragColor;

flat in     vec3 Position;
flat in     vec3 Velocity;
flat in     vec4 Color;
flat in     float Rotate;
flat in     float Size;
flat in     float Age;
flat in     float LifeTime;

void main()
{
    FragColor = Color;
}