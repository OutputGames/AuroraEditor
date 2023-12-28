#include "player.hpp"

#include "agl/agl.hpp"
#include "aurora/gfx/camera.hpp"
#include "aurora/physics/physics.hpp"
#include "glm/gtx/projection.hpp"

CLASS_DEFINITION(Component, PlayerController)

void PlayerController::Init()
{
	collider = Entity->GetComponent<Collider>();
}

void PlayerController::Update()
{

	if (xAxis < -4000)
	{
		xAxis = 0;
	}

	if (yAxis < -4000)
	{
		yAxis = 1;
	}

	if (rXAxis < -4000)
	{
		rXAxis = 0;
	}

	if (rYAxis < -4000)
	{
		rYAxis = 1;
	}

	if (Radius < -4000)
	{
		Radius = 3;
	}

	float scale = 0.01f;


	xAxis += Gamepad->leftStickAxis.x * scale;
	yAxis += Gamepad->leftStickAxis.y * scale;

	rXAxis += Gamepad->rightStickAxis.x * scale;
	rYAxis += Gamepad->rightStickAxis.y * scale;

	//posY = std::clamp(posY, -1.0f, 1.0f);

	float maxRadius = 25.0f;

	Radius = maxRadius;

	float t = SDL_GetTicks64() * 0.001f;

	vec3 camOffset = { 0,2,0 };
	vec3 targetOffset = { 0,1.25,0 };

	float cx = sinf(rXAxis) * Radius;
	float cz = cosf(rXAxis) * Radius;

	float delta = agl::deltaTime;

	lookUpAmt += Gamepad->rightStickAxis.y * scale;

	lookUpAmt = std::clamp(lookUpAmt, -1.0f, 1.0f);

	Camera::Main->Entity->Transform.position = Entity->Transform.position + camOffset + vec3{cx, lookUpAmt * Radius, cz};
	Camera::Main->target = Entity->Transform.position + targetOffset;

	float px = Gamepad->leftStickAxis.x;
	float py = Gamepad->leftStickAxis.y;

	float speed = 60;
	float jumpPower = 15;

	CharacterController* controller = Entity->GetComponent<CharacterController>();

	vec3 velo = controller->GetVelocity();

	velo = vec3{ px * speed, 0, py * speed };

	glm::vec3 forward = glm::normalize(glm::vec3(0.0f, 0.0f, -1.0f) * glm::mat3(inverse(Camera::Main->GetViewMatrix())));

	//forward = glm::proj(forward , Camera::Main->up);

	velo *= forward;

	velo.y = controller->GetVelocity().y;

	if (Gamepad->IsButtonReleased(SDL_CONTROLLER_BUTTON_A))
	{
		velo += vec3{ 0,jumpPower,0 };
	}

	controller->SetVelocity(velo);

	if (controller->Entity->Transform.position.y < -100)
	{
		controller->Reset();
	}
}
