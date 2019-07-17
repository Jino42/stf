#include "Texture.hpp"
#include <glad/glad.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <exception>
#include <iostream>

unsigned int Texture::TextureFromFile(const char *path, boost::filesystem::path const &directory)
{
	boost::filesystem::path		filename(directory / path);
	unsigned char      			 *data;
	unsigned int        		textureID;
	GLenum              		format;
	int                 		width, height, nrComponents;


	glGenTextures(1, &textureID);

	data = stbi_load(filename.generic_string().c_str(), &width, &height, &nrComponents, 0);
	if (data && nrComponents == 2)
		throw(std::runtime_error(std::string("Texture cannot be GREY_ALPHA at path: ") + path));
	else if (data) {

		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;
		else
			format = 0;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else
	{
		stbi_image_free(data);
		throw(std::runtime_error(std::string("Texture failed to load at path: ") + filename.generic_string()));
	}
	stbi_image_free(data);
	std::cout << "Texture [" << (directory / path).string() << "]" << std::endl;

	return (textureID);
}