#include "Character/PlayerCharacterCore.h"
#include <algorithm>

using namespace std;

/*
 * Unreal 프로젝트의 PlayerCharacter에 의해 관리됨.
 * 메모리 관리에서 Controller는 소유하지 않고 약참조.
 */

namespace CPPCore
{
	PlayerCharacterCore::PlayerCharacterCore(ICharacterController* InController, const CharacterStatsConfig& InConfig)
		: Controller(InController),
		Config(InConfig),
		bIsSprinting(false),
		bIsZooming(false),
		CurrentJumpCount(0),
		CurrentFOV(90.0f),
		bWasGrounded(true),
		SprintStaminaTimer(0.0f)
	{
		Health = make_unique<HealthSystem>(Config.MaxHealth);
		Stamina = make_unique<StaminaSystem>(Config.MaxStamina);

		if (Controller)
		{
			Controller->SetMovementSpeed(Config.WalkSpeed);
			Controller->SetCameraFOV(Config.DefaultFOV);
			LastPosition = Controller->GetPosition();
			CurrentFOV = Config.DefaultFOV;
		}
	}
	
	PlayerCharacterCore::~PlayerCharacterCore() = default;
	

	//=======================Handle========================
	
	void PlayerCharacterCore::HandleMoveInput(const Vector2& InputVector)
	{
		if (Controller)
		{
			ProcessMovementInput(InputVector);
		}
	}

	void PlayerCharacterCore::HandleLookInput(const Vector2& InputVector)
	{
		if (!Controller) return;

		// 개발하다가 회전 버그나서 적어둠
		// Controller->GetRotation()은 (Pitch, Yaw, Roll) 순서임.
		Vector3 current = Controller->GetRotation();
		float pitch = current.X;
		float yaw   = current.Y;
		// roll은 사용 안함.

		const float Sensitivity = 0.5f;

		// 언리얼 마우스 X → Yaw, 마우스 Y → Pitch(보통 반전이라고 함)
		yaw   += InputVector.X * Sensitivity;
		pitch -= InputVector.Y * Sensitivity;

		// 피치 제한(도 단위) -> 마우스 위 아래
		pitch = clamp(pitch, -89.0f, 89.0f);

		// Roll은 0 유지(혹은 기존 값 유지)
		Controller->SetRotation(Vector3(pitch, yaw, 0.0f));
	}

	void PlayerCharacterCore::HandleJumpInput()
	{
		if (!Controller || !CanJump()) return;
		
		Controller->Jump();
		CurrentJumpCount++;

		if (OnJumped)
		{
			OnJumped();
		}
	}

	void PlayerCharacterCore::HandleSprintStart()
	{
		if (!bIsSprinting && !bIsZooming && Stamina->HasEnoughStamina((5.0f)))
		{
			bIsSprinting = true;
			UpdateMovementSpeed();

			if (OnSprintStateChanged)
			{
				OnSprintStateChanged(true);
			}
		}
	}

	void PlayerCharacterCore::HandleSprintEnd()
	{
		if (bIsSprinting)
		{
			bIsSprinting = false;
			UpdateMovementSpeed();

			if (OnSprintStateChanged)
			{
				OnSprintStateChanged(false);
			}
		}
	}

	void PlayerCharacterCore::HandleZoomStart()
	{
		if (!bIsZooming)
		{
			// 달리는 중일떄 못함.
			if (bIsSprinting)
			{
				HandleSprintEnd();
			}

			bIsZooming = true;

			if (OnZoomStateChanged)
			{
				OnZoomStateChanged(true);
			}
		}
	}

	void PlayerCharacterCore::HandleZoomEnd()
	{
		if (bIsZooming)
		{
			bIsZooming = false;

			if (OnZoomStateChanged)
			{
				OnZoomStateChanged(false);
			}
		}
	}

	void PlayerCharacterCore::HandleAttackInput()
	{
		if (OnAttack)
		{
			OnAttack();
		}
	}
	
	bool PlayerCharacterCore::CanJump() const
	{
		if (!Controller)
			return false;

		return Controller->IsGrounded() || CurrentJumpCount < Config.MaxJumpCount;
	}



	//=======================Update========================
	
	void PlayerCharacterCore::Update(float DeltaTime)
	{
		if (!Controller)
			return;

		UpdateCameraFOV(DeltaTime);
		UpdateGroundCheck();
		UpdateSprint(DeltaTime);

		// 스태미나 복구
		Stamina->RegenerateStamina(DeltaTime);

		// 변경된 위치값 체크
		Vector3 CurrentPosition = Controller->GetPosition();
		if ((CurrentPosition - LastPosition).Length() > 0.01f)
		{
			if (OnPositionChanged)
			{
				OnPositionChanged(CurrentPosition);
			}
			LastPosition = CurrentPosition;
		}
	}

	void PlayerCharacterCore::UpdateMovementSpeed()
	{
		if (Controller)
		{
			float TargetSpeed = bIsSprinting ? Config.SprintSpeed : Config.WalkSpeed;
			Controller->SetMovementSpeed(TargetSpeed);
		}
	}

	void PlayerCharacterCore::UpdateCameraFOV(float DeltaTime)
	{
		if (Controller)
		{
			float TargetFOV = bIsZooming ? Config.ZoomedFOV : Config.DefaultFOV;

			//스무스한 FOV 보간
			float Alpha = min(1.0f, DeltaTime * Config.ZoomInterpSpeed);
			CurrentFOV = CurrentFOV + (TargetFOV - CurrentFOV) * Alpha;

			Controller->SetCameraFOV(CurrentFOV);
		}
	}

	void PlayerCharacterCore::UpdateGroundCheck()
	{
		if (!Controller)
			return;

		bool bIsGrounded = Controller->IsGrounded();

		//땅 밟았을 때만
		if (!bIsGrounded && bIsGrounded)
		{
			CurrentJumpCount = 0;

			if (OnLanded)
			{
				OnLanded();
			}
		}

		bWasGrounded = bIsGrounded;
	}

	void PlayerCharacterCore::UpdateSprint(float DeltaTime)
	{
		if (bIsSprinting)
		{
			//달리기중 체력소모
			SprintStaminaTimer += DeltaTime;


			// 0.1초마다 스태미나 사용구현
			if (SprintStaminaTimer >= 0.1f)
			{
				Stamina->UseStamina(Config.SprintStaminaCost * 0.1f);
				SprintStaminaTimer = 0.0f;

				//스태미나가 없다면 달리기 중지
				if (!Stamina->HasEnoughStamina(1.0f))
				{
					HandleSprintEnd();
				}
			}
		}
	}

	void PlayerCharacterCore::ProcessMovementInput(const Vector2& Input)
	{
		if (!Controller)
			return;

		// 입력 벡터 정규화 (대각선 이동 속도 조정)
		Vector2 NormalizedInput = Input;
		if (NormalizedInput.Length() > 1.0f)
			NormalizedInput = NormalizedInput.Normalize();

		// 카메라 방향 가져오기
		Vector3 Forward = Controller->GetCameraForward();
		Forward.Z = 0; // 수평 이동 유지
    
		Vector3 Right = Controller->GetCameraRight();
		Right.Z = 0; // 수평 이동 유지

		// 길이 체크 후 정규화
		if (Forward.Length() > 0.0001f)
			Forward = Forward.Normalize();
    
		if (Right.Length() > 0.0001f)
			Right = Right.Normalize();

		// 이동 방향 계산
		Vector3 MovementDirection = Forward * NormalizedInput.Y + Right * NormalizedInput.X;
		if (MovementDirection.Length() > 0.01f)
		{
			MovementDirection = MovementDirection.Normalize();
			Controller->AddMovementInput(MovementDirection, 1.0f);
		}
	}
}
