#version 400 core
out vec4 FragColor;

flat in     vec4 fColor;

void main()
{
        FragColor = fColor;
    /*
    vec3 veloNormalized = normalize(Velocity);
    veloNormalized = clamp(veloNormalized, vec3(0.f), vec3(1.f));
    FragColor = vec4(veloNormalized, 1.f);
    */
}