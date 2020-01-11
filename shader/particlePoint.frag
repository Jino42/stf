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

void main()
{
    FragColor = Color;
    /*
    vec3 veloNormalized = normalize(Velocity);
    veloNormalized = clamp(veloNormalized, vec3(0.f), vec3(1.f));
    FragColor = vec4(veloNormalized, 1.f);
    */
}