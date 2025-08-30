#pragma once

#include "CoreMinimal.h"
#include "../Core/Character/ICharacterController.h"

class ACharacter;
class UCameraComponent;

class FUnrealCharacterController : public Core::ICharacterController
{
public:
	FUnrealCharacterController(ACharacter* InCharacter, UCameraComponent * InCamera);
	
	virtual void AddMovementInput(const Core::Vector3& Direction, float Scale) override;
	virtual void Jump() override;
	virtual bool IsGrounded() const override;
	virtual void SetMovementSpeed(float Speed) override;

	virtual void SetPosition(const Core::Vector3& Position) override;
	virtual Core::Vector3 GetPosition() const override;
	virtual void SetRotation(const Core::Vector3& Rotation) override;
	virtual Core::Vector3 GetRotation() const override;


	virtual void SetCameraFOV(float FOV) override;
	virtual float GetCameraFOV() const override;
	virtual Core::Vector3 GetCameraLocation() const override;
    virtual Core::Vector3 GetCameraForward() const override;
	virtual Core::Vector3 GetCameraRight() const override;

private:
	ACharacter* Character;
	UCameraComponent* Camera;

	FVector ToUnrealVector(const Core::Vector3& Vector) const;
	Core::Vector3 FromUnrealVector(const FVector& Vector)const;
	FRotator ToUnrealRotator(const Core::Vector3& PitchYawRoll) const;
	Core::Vector3 FromUnrealRotator(const FRotator& Rotator)const;
};
