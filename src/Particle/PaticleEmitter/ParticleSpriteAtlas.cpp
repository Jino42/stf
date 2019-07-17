#include "ParticleSpriteAtlas.hpp"
#include "Engine/Graphic/Texture.hpp"

ParticleSpriteAtlas::ParticleSpriteAtlas(const char *path, boost::filesystem::path const &directory, unsigned int tileSize) :
tileSize_(tileSize),
atlasId_(Texture::TextureFromFile(path, directory)) {
}

unsigned int ParticleSpriteAtlas::getNumberOfRows() const {
	return tileSize_;
}
unsigned int ParticleSpriteAtlas::getAtlasId() const {
	return atlasId_;
}
