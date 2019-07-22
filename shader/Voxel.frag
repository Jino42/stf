#version 330 core

out vec4 FragColor;

//in  GS_OUT {
//    flat uint color;
//}   fs_in;


uniform int id;

void main()
{
/*
    FragColor = vec4(
    (fs_in.color & 0xFF0000) / 255.0f,
    (fs_in.color & 0x00FF00) / 255.0f,
    (fs_in.color & 0x0000FF) / 255.0f,
    1.0f);*/

    if ((id % 3) == 0)
        FragColor = vec4(0.8f, 0.0f, 0.2f, 1.0f);
    if ((id % 3) == 1)
        FragColor = vec4(0.2f, 0.8f, 0.0f, 1.0f);
    if ((id % 3) == 2)
        FragColor = vec4(0.0f, 0.2f, 0.8f, 1.0f);
}
