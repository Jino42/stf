#version 330 core
out vec4 FragColor;

smooth in   vec2 TexCoords;
smooth in   vec2 Normal;
flat in     vec3 Position;
flat in     vec3 Velocity;
flat in     float Pressure;
flat in     float Density;
flat in     float Mass;
flat in     int Flag;

void main()
{
    //FragColor = Color;
    vec3 veloNormalized = normalize(Velocity);
    veloNormalized = clamp(veloNormalized, vec3(0.f), vec3(1.f));
    FragColor = vec4(veloNormalized, 1.f);

    FragColor = vec4(vec3(1.f / Density), 1.f);

    FragColor = vec4(vec3(250.f / Pressure), 1.f);

    if (Flag == 1)
        FragColor = vec4(1.0f, 0.0f, 0.0f, 1.f);
    else if (Flag != 0)
        FragColor = vec4(0.0f, 0.0f, 1.0f, 1.f);

    //FragColor = vec4(vec3(Mass), 1.f);
}