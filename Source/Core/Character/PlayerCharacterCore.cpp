#include "PlayerCharacterCore.h"
#include <algorithm>


using namespace std;

namespace Core
{
	PlayerCharacterCore::PlayerCharacterCore(ICharacterController* InController)
		: Controller(InController),
		bIsSprinting(false),
		bIsZooming(false),
		CurrentJumpCount(0),
		CurrentFOV(90.0f),
		bWasGrounded(true),
		SprintStaminaTimer(0.0f)
	{
		Health = make_unique<HealthSystem>(Stats.MaxHealth);
		Stamina = make_unique<StaminaSystem>(Stats.MaxStamina);

		if (Controller)
		{
			Controller->SetMovementSpeed(Stats.WalkSpeed);
			Controller->SetCameraFOV(Stats.DefaultFOV);
			LastPosition = Controller->GetPosition();
			CurrentFOV = Stats.DefaultFOV;
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
		if (Controller)
		{
			// 현재 회전값 가지고 오기
			Vector3 CurrentRotationVec = Controller->GetRotation();

			//입력을 회전값에 적용(감도계수 포함)
			float Sensitivity = 0.5f;

			Rotator CurrentRotation;
			CurrentRotation.Roll = CurrentRotationVec.X; 
			CurrentRotation.Pitch = CurrentRotationVec.Y; 
			CurrentRotation.Yaw = CurrentRotationVec.Z;

			//감도적용
			CurrentRotation.Yaw += InputVector.X * Sensitivity;
			CurrentRotation.Pitch += InputVector.Y * Sensitivity;
			CurrentRotation.Pitch = clamp(CurrentRotation.Pitch, -90.0f, 90.0f); // 카메라 수직각도 제한

			Controller->SetRotation(Vector3(
				CurrentRotation.Roll,
				CurrentRotation.Pitch,
				CurrentRotation.Yaw));
		}
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

		return Controller->IsGrounded() || CurrentJumpCount < Stats.MaxJumpCount;
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
			float TargetSpeed = bIsSprinting ? Stats.SprintSpeed : Stats.WalkSpeed;
			Controller->SetMovementSpeed(TargetSpeed);
		}
	}

	void PlayerCharacterCore::UpdateCameraFOV(float DeltaTime)
	{
		if (Controller)
		{
			float TargetFOV = bIsZooming ? Stats.ZoomedFOV : Stats.DefaultFOV;

			//스무스한 FOV 보간
			float Alpha = min(1.0f, DeltaTime * Stats.ZoomInterpSpeed);
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
				Stamina->UseStamina(Stats.SprintStaminaCost * 0.1f);
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
