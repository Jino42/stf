#version 330 core
out vec4 FragColor;
  
in vec2 TexCoord;

uniform sampler2D texture1;

void main()
{
    if (TexCoord.x != 4561) 
        FragColor = texture(texture1, TexCoord);
    else
        FragColor = vec4(1.f, 0.f, 0.f, 1.f);
}