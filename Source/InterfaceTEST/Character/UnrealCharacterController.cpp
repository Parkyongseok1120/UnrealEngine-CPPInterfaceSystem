#include "UnrealCharacterController.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"

FUnrealCharacterController::FUnrealCharacterController(ACharacter* InCharacter, UCameraComponent* InCamera)
    : Character(InCharacter)
    , Camera(InCamera)
{
}

void FUnrealCharacterController::AddMovementInput(const Core::Vector3& Direction, float Scale)
{
    if (Character)
    {
        Character->AddMovementInput(ToUnrealVector(Direction), Scale);
    }
}

void FUnrealCharacterController::Jump()
{
    if (Character)
    {
        Character->Jump();
    }
}

bool FUnrealCharacterController::IsGrounded() const
{
    if (Character && Character->GetCharacterMovement())
    {
        return Character->GetCharacterMovement()->IsMovingOnGround();
    }
    return false;
}

void FUnrealCharacterController::SetMovementSpeed(float Speed)
{
    if (Character && Character->GetCharacterMovement())
    {
        Character->GetCharacterMovement()->MaxWalkSpeed = Speed;
    }
}

void FUnrealCharacterController::SetPosition(const Core::Vector3& Position)
{
    if (Character)
    {
        Character->SetActorLocation(ToUnrealVector(Position));
    }
}

Core::Vector3 FUnrealCharacterController::GetPosition() const
{
    if (Character)
    {
        return FromUnrealVector(Character->GetActorLocation());
    }
    return Core::Vector3();
}

void FUnrealCharacterController::SetRotation(const Core::Vector3& Rotation)
{
    if (Character && Character->Controller)
    {
        Character->Controller->SetControlRotation(ToUnrealRotator(Rotation));
    }
}

Core::Vector3 FUnrealCharacterController::GetRotation() const
{
    if (Character && Character->Controller)
    {
        return FromUnrealRotator(Character->Controller->GetControlRotation());
    }
    return Core::Vector3();
}

void FUnrealCharacterController::SetCameraFOV(float FOV)
{
    if (Camera)
    {
        Camera->SetFieldOfView(FOV);
    }
}

float FUnrealCharacterController::GetCameraFOV() const
{
    return Camera ? Camera->FieldOfView : 90.0f;
}

Core::Vector3 FUnrealCharacterController::GetCameraLocation() const
{
    if (Camera)
    {
        return FromUnrealVector(Camera->GetComponentLocation());
    }
    return Core::Vector3();
}

Core::Vector3 FUnrealCharacterController::GetCameraForward() const
{
    if (Camera)
    {
        return FromUnrealVector(Camera->GetForwardVector());
    }
    return Core::Vector3(1, 0, 0);
}

Core::Vector3 FUnrealCharacterController::GetCameraRight() const
{
    if (Camera)
    {
        return FromUnrealVector(Camera->GetRightVector());
    }
    return Core::Vector3(0, 1, 0);
}

FVector FUnrealCharacterController::ToUnrealVector(const Core::Vector3& Vector) const
{
    return FVector(Vector.X, Vector.Y, Vector.Z);
}

Core::Vector3 FUnrealCharacterController::FromUnrealVector(const FVector& Vector) const
{
    return Core::Vector3(Vector.X, Vector.Y, Vector.Z);
}

FRotator FUnrealCharacterController::ToUnrealRotator(const Core::Vector3& PitchYawRoll) const
{
    // Core::Vector3에서 사용. (Pitch, Yaw, Roll) 순서.
    return FRotator(PitchYawRoll.X, PitchYawRoll.Y, PitchYawRoll.Z);
}

Core::Vector3 FUnrealCharacterController::FromUnrealRotator(const FRotator& Rotator) const
{
    return Core::Vector3(Rotator.Pitch, Rotator.Yaw, Rotator.Roll);
}
