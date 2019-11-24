#version 330 core
out vec4 FragColor;

smooth in   vec2 TexCoords;
smooth in   vec3 Normal;
flat in     vec3 Position;
flat in     vec3 Velocity;
flat in     vec4 Color;
flat in     float Rotate;
flat in     float Size;
flat in     float Age;
flat in     float LifeTime;
smooth in vec2 SpriteOffset1;
smooth in vec2 SpriteOffset2;
flat in float SpriteBlend;

uniform sampler2D sprite;

void main()
{
    vec4 color1 = texture(sprite, SpriteOffset1);
    vec4 color2 = texture(sprite, SpriteOffset2);
        FragColor = mix(color1, color2, SpriteBlend);
}