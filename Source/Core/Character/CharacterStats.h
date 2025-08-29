#pragma once
#include <cstdint>

namespace Core
{
	struct CharacterStats
	{
		//Movement
		float WalkSpeed = 400.0f;
		float SprintSpeed = 600.0f;
		float JumpForce = 500.0f;
		int32_t MaxJumpCount = 2;

		//Camera
		float DefaultFOV = 90.0f;
		float ZoomedFOV = 45.0f;
		float ZoomInterpSpeed = 20.0f;

		//Health
		int32_t MaxHealth = 100;
		int32_t DefaultHealth = 100;

		//Statmina
		float MaxStamina = 100.0f;
		float StminaRegenRate = 10.0f;
		float SprintStaminaCost = 15.0f;
	};
}

