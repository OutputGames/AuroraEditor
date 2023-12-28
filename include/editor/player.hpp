#if !defined(PLAYER_HPP)
#define PLAYER_HPP

#include "aurora/utils/utils.hpp"
#include "aurora/ecs/component.hpp"
#include "aurora/input/input.hpp"

class PlayerController : public Component
{
	CLASS_DECLARATION(PlayerController)

public:
	PlayerController(std::string&& initialValue) : Component(move(initialValue))
	{
	}

	PlayerController() = default;

	void Init() override;
    void Update() override;

	aim::aimGamepad* Gamepad;

private:

	float xAxis, yAxis, Radius;
	float rXAxis, rYAxis;
	float lookUpAmt;

	Collider* collider;
};


#endif // PLAYER_HPP
