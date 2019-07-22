#include <string>
#include <boost/filesystem.hpp>

struct Texture {
	enum eType { NONE, DIFFUSE, SPECULAR };

	unsigned int	id;
	eType			type;
	std::string		path;

	static unsigned int TextureFromFile(const char *path, boost::filesystem::path const &directory);
};