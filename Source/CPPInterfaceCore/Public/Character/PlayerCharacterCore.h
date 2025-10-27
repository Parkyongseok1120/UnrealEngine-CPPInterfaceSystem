#pragma once
#include "ICharacterController.h"
#include "CharacterStatsConfig.h"
#include "../Math/MathTypes.h"
#include "../Health/HealthSystem.h"
#include "../Stamina/StaminaSystem.h"

#include <functional>
#include <memory>

using namespace std;

/*
 * Unreal 프로젝트의 PlayerCharacter에 의해 관리됨.
 * 메모리 관리에서 Controller는 소유하지 않고 약참조.
 * 생성자는 config를 매개변수로 받게 됨.
 * Controller의 라이프타임은 해당 객체의 생명주기보다는 길어야 함.
 */
namespace CPPCore
{
	class CPPINTERFACECORE_API PlayerCharacterCore
	{
	public:

		explicit PlayerCharacterCore(ICharacterController* InController, const CharacterStatsConfig& InConfig);

		~PlayerCharacterCore();

		//Input Handlers (engine-independent)
		void HandleMoveInput(const Vector2& InputVector);
		void HandleLookInput(const Vector2& InputVector);
		void HandleJumpInput();
		void HandleSprintStart();
		void HandleSprintEnd();
		void HandleZoomStart();
		void HandleZoomEnd();
		void HandleAttackInput();

		//Update(called every frame)
		void Update(float DeltaTime);

		//State queries
		bool IsSprinting() const {return bIsSprinting;}
		bool IsZooming() const {return bIsZooming;}
		bool CanJump() const;

		//SubSystem accessors
		HealthSystem* GetHealthSystem() const {return Health.get();}
		StaminaSystem* GetStaminaSystem() const {return Stamina.get();}

		//Events
		function<void(Vector3)> OnPositionChanged;
		function<void(bool)> OnSprintStateChanged;
		function<void(bool)> OnZoomStateChanged;
		function<void()> OnJumped;
		function<void()> OnLanded;
		function<void()> OnAttack;

	private:
		ICharacterController* Controller;

		unique_ptr<HealthSystem> Health;
		unique_ptr<StaminaSystem> Stamina;

		CharacterStatsConfig Config;

		bool bIsSprinting;
		bool bIsZooming;
		int32_t CurrentJumpCount;
		float CurrentFOV;
		Vector3 LastPosition;
		bool bWasGrounded;
		float SprintStaminaTimer;

		// Internal methods
		void UpdateMovementSpeed();
        void UpdateCameraFOV(float DeltaTime);
		void UpdateGroundCheck();
		void UpdateSprint(float DeltaTime);
		void ProcessMovementInput(const Vector2& Input);
		
	};
}
