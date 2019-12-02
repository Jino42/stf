#pragma once

#include <memory>
#include <boost/filesystem.hpp>

class PathManager {
public:
	PathManager();
	PathManager(PathManager const &) = delete;
	~PathManager() = default;
	static PathManager &Get();

	/// \brief Add Path
	/// \details Store a Path linked to a name
	///				like "resources" linked to the resources path
	/// \param Name path
	/// \param Path
	void addPath(std::string const &name, boost::filesystem::path path);


	/// \brief Get Path linked to a name
	/// \details Shearch in map, the Path associated to this name
	///             If the name are not found, throw an exception
	/// \param Name of path
	/// \return Path
	/// \throw It throws out_of_range if "Name" is not the key of an element in the Path map.
	boost::filesystem::path &getPath(std::string const &name);

private:
	std::map< std::string, boost::filesystem::path > mapPaths_;

	static std::unique_ptr<PathManager> instance_;
};