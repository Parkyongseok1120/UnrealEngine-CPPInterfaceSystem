#pragma once
#include <cstdint>

namespace CPPCore
{
	// 캐릭터 설정을 이한 DTO(Data Transfer Object) 구조체
	// DTO : 로직없이 데이터만 다른계층(시스템, 모듈, 클래스 등)으로 전달하기 위한 순수한 객체(노트정리)
	struct CharacterStatsConfig
	{
		// Movement 
		float WalkSpeed = 400.0f;
		float SprintSpeed = 600.0f;
		float JumpForce = 500.0f;
		int32_t MaxJumpCount = 2;

		// Camera 
		float DefaultFOV = 90.0f;
		float ZoomedFOV = 45.0f;
		float ZoomInterpSpeed = 20.0f;

		// Health 
		int32_t MaxHealth = 100;
		int32_t DefaultHealth = 100;

		// Stamina 
		float MaxStamina = 100.0f;
		float StaminaRegenRate = 10.0f;
		float SprintStaminaCost = 15.0f;
	};
}