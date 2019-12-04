#pragma once
#include <json.hpp>
#include <boost/filesystem.hpp>
#include <NTL.hpp>

class ParticleSystem;
class AParticleEmitter;

class JsonParticleParser {
public:
	JsonParticleParser() = delete;
	JsonParticleParser(JsonParticleParser const &) = delete;
	~JsonParticleParser() = default;

	explicit JsonParticleParser(boost::filesystem::path path);

	void parse();

	static glm::vec3	jsonToVec3(json &json);

private:
	boost::filesystem::path jsonPath_;
	json					json_;
	ParticleSystem			*currentSystem_;
	AParticleEmitter			*currentEmitter_;
};
