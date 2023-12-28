#if !defined(CAM_HPP)
#define CAM_HPP

#include "aurora/utils/utils.hpp"
#include "aurora/ecs/component.hpp"
#include "aurora/gfx/camera.hpp"
#include "aurora/input/input.hpp"

class OrbitalCamera : public Component
{
	CLASS_DECLARATION(OrbitalCamera)

public:
	OrbitalCamera(std::string&& initialValue) : Component(move(initialValue))
	{
	}

	OrbitalCamera() = default;
	void Update() override;

	aim::aimGamepad* Gamepad;

private:

	float xRotation = 0.0f, yRotation = 0.0f, Radius = 0.0f;

};

#endif // CAM_HPP
