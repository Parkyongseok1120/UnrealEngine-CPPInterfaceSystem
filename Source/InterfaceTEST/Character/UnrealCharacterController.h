#pragma once

#include "CoreMinimal.h"
#include "Character/ICharacterController.h"

class ACharacter;
class UCameraComponent;

class FUnrealCharacterController : public CPPCore::ICharacterController
{
public:
	FUnrealCharacterController(ACharacter* InCharacter, UCameraComponent * InCamera);
	
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
	ACharacter* Character;
	UCameraComponent* Camera;

	FVector ToUnrealVector(const CPPCore::Vector3& Vector) const;
	CPPCore::Vector3 FromUnrealVector(const FVector& Vector)const;
	FRotator ToUnrealRotator(const CPPCore::Vector3& PitchYawRoll) const;
	CPPCore::Vector3 FromUnrealRotator(const FRotator& Rotator)const;
};
