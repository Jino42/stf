#pragma once

#include <memory>
#include <map>
#include <Engine/Camera.hpp>

/// \breid Permit to Manage all Cameras
/// \details You can Add, Remove Cameras, associated with StringName
class CameraManager {
public:

	/// \brief Constructor
	/// \details Create a default Camera, mapped with "Default" name
	CameraManager();

	/// \brief Set Focused Camera
	/// \brief Set Focused Camera for get him with :
	///			CameraManager::Get().getFocus();
	///			Camera::Focus;
	/// \{
	/// \param camera
	void	setFocus(Camera &camera);
	/// \param name : Name mapped to Camera to focus
	/// \throw It throws out_of_range if "Name" is not the key of an element in the Cameras map.
	void	setFocus(std::string const &name);
	/// \}

	/// \brief Get Camera focused
	/// \details Default camera is created in constructor of Cameramanager
	Camera &getFocus();

	void	addCamera(std::string const &name);

	/// \throw It throws out_of_range if "Name" is not the key of an element in the Cameras map.
	Camera	&getCamera(std::string const &name);
	//void	removeCamera(std::string const &name);

	static CameraManager &Get();


private:
	std::map< std::string, Camera > mapCameras_;
	Camera *focus_;

	static std::unique_ptr<CameraManager> instance_;
};