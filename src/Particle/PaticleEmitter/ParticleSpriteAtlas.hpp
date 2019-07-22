#pragma once

#include <boost/filesystem.hpp>

class ParticleSpriteAtlas {
public:
	ParticleSpriteAtlas(const char *path, boost::filesystem::path const &directory, unsigned tileSize);

	unsigned int getNumberOfRows() const;
	unsigned int getAtlasId() const;

private:
	unsigned int	tileSize_;
	unsigned int	atlasId_;
};
