#pragma once

#include "CoreMinimal.h"
#include "Character/ICharacterController.h"

class ACharacter;
class UCameraComponent;

// 어댑터 역할, 언리얼 엔진 -> CPPCore 인터페이스로 연결
// 라이프타임은 댕글링 포인터 방지를 위해서 약참조(TWeakObjectPtr) 사용.
// 모든 UObject 접근(참조)은 사용 전에 유효성 검사(IsValid)
// 일부 변환 유틸은 인라인 함수로 개편

class FUnrealCharacterController : public CPPCore::ICharacterController
{
public:
	FUnrealCharacterController(ACharacter* InCharacter, UCameraComponent * InCamera);

private:
	/* ============= Inline 변환 유틸 ================ */
	FORCEINLINE	FVector ToUnrealVector(const CPPCore::Vector3& Vector) const
	{
		return FVector(Vector.X, Vector.Y, Vector.Z);
	}
	
	FORCEINLINE CPPCore::Vector3 FromUnrealVector(const FVector& Vector)const
	{
		return CPPCore::Vector3(Vector.X, Vector.Y, Vector.Z);
	}
	
	FORCEINLINE FRotator ToUnrealRotator(const CPPCore::Vector3& PitchYawRoll) const
	{
		return FRotator(PitchYawRoll.X, PitchYawRoll.Y, PitchYawRoll.Z);
	}
	
	FORCEINLINE CPPCore::Vector3 FromUnrealRotator(const FRotator& Rotator)const
	{
		return CPPCore::Vector3(Rotator.Pitch, Rotator.Yaw, Rotator.Roll);
	}	

public:
	virtual void AddMovementInput(const CPPCore::Vector3& Direction, float Scale) override;
	virtual void Jump() override;
	virtual bool IsGrounded() const override;
	virtual void SetMovementSpeed(float Speed) override;

	virtual void SetPosition(const CPPCore::Vector3& Position) override;
	virtual CPPCore::Vector3 GetPosition() const override;
	virtual void SetRotation(const CPPCore::Vector3& Rotation) override;
	virtual CPPCore::Vector3 GetRotation() const override;


	virtual void SetCameraFOV(float FOV) override;
	virtual float GetCameraFOV() const override;
	virtual CPPCore::Vector3 GetCameraLocation() const override;
    virtual CPPCore::Vector3 GetCameraForward() const override;
	virtual CPPCore::Vector3 GetCameraRight() const override;

private:
	TWeakObjectPtr<ACharacter> Character;
	TWeakObjectPtr<UCameraComponent> Camera;
};
