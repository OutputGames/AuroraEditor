#include "cam.hpp"

CLASS_DEFINITION(Component, OrbitalCamera)

void OrbitalCamera::Update()
{

	if (xRotation < -4000)
	{
		xRotation = 0;
	}

	if (yRotation < -4000)
	{
		yRotation = 1;
	}

	if (Radius < -4000)
	{
		Radius = 3;
	}


	xRotation += Gamepad->leftStickAxis.x * 0.05f;
	yRotation += Gamepad->leftStickAxis.y * 0.05f;

	//posY = std::clamp(posY, -1.0f, 1.0f);

	Radius += Gamepad->rightStickAxis.y * 0.5f;

	float maxRadius = 25.0f;

	Radius = std::clamp(Radius, -maxRadius, maxRadius);

	float t = SDL_GetTicks64() * 0.001f;

	Entity->Transform.position = vec3{ sinf(xRotation) * Radius, yRotation * Radius, cosf(xRotation) * Radius };
	//Entity->Transform.position = vec3{ sinf(t)*10.0f,0,cosf(t)*10.0f };


}