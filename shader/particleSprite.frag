#version 330 core
out vec4 FragColor;


smooth in   vec2 fTexCoords;
smooth in   vec3 fNormal;
flat in     vec3 fPosition;
flat in     vec3 fVelocity;
flat in     vec4 fColor;
flat in     int fIsAlive;
smooth in vec2 fSpriteOffset1;
smooth in vec2 fSpriteOffset2;
flat in float fSpriteBlend;

uniform sampler2D sprite;

void main()
{
    vec4 color1 = texture(sprite, fSpriteOffset1);
    vec4 color2 = texture(sprite, fSpriteOffset2);
        FragColor = mix(color1, color2, fSpriteBlend);
        //FragColor = vec4(1.f);
}