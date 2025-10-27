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

void FUnrealCharacterController::AddMovementInput(const CPPCore::Vector3& Direction, float Scale)
{
    // 정리 : TWeakObjectPtr 사용 시, 유효한 포인터인지 확인하기 위해 Get()으로 실제 객체에 접근.
    // Get()은 새로운 강한 참조를 만들지 않으며, nullptr일 수 있으므로 반드시 체크 필요.
    if (ACharacter *C = Character.Get()) 
    {
        Character->AddMovementInput(ToUnrealVector(Direction), Scale);
    }
}

void FUnrealCharacterController::Jump()
{
    if (ACharacter *C = Character.Get())
    {
        C->Jump();
    }
}

bool FUnrealCharacterController::IsGrounded() const
{
    if (ACharacter *C = Character.Get())
    {
        if (UCharacterMovementComponent* Movement = C->GetCharacterMovement())
        {
            return Movement -> IsMovingOnGround();
        }
    }
    return false;
}

void FUnrealCharacterController::SetMovementSpeed(float Speed)
{
    if (ACharacter *C = Character.Get())
    {
        if (UCharacterMovementComponent* Movement = C->GetCharacterMovement())
        {
            Movement ->MaxWalkSpeed = Speed;
        }
    }
}

void FUnrealCharacterController::SetPosition(const CPPCore::Vector3& Position)
{
    if (ACharacter *C = Character.Get())
    {
        C->SetActorLocation(ToUnrealVector(Position));
    }
}

CPPCore::Vector3 FUnrealCharacterController::GetPosition() const
{
    if (ACharacter *C = Character.Get())
    {
        return FromUnrealVector(C->GetActorLocation());
    }
    return CPPCore::Vector3();
}

void FUnrealCharacterController::SetRotation(const CPPCore::Vector3& Rotation)
{
    if (ACharacter *C = Character.Get())
    {
        if (AController* Controller = C->GetController())
        Controller->SetControlRotation(ToUnrealRotator(Rotation));
    }
}

CPPCore::Vector3 FUnrealCharacterController::GetRotation() const
{
    if (ACharacter *C = Character.Get())
    {
        if (AController* Controller = C->Controller)
        {
            return FromUnrealRotator(Controller->GetControlRotation());
        }
    }
    
    return CPPCore::Vector3();
}

void FUnrealCharacterController::SetCameraFOV(float FOV)
{
    if (UCameraComponent * Cam = Camera.Get())
    {
        Cam->SetFieldOfView(FOV);
    }
}

float FUnrealCharacterController::GetCameraFOV() const
{
    if (UCameraComponent * Cam = Camera.Get())
    {
        return Cam ->FieldOfView;
    }
    return 90.0f; // 기본 FOV 값 반환.
}

CPPCore::Vector3 FUnrealCharacterController::GetCameraLocation() const
{
    if (UCameraComponent * Cam = Camera.Get())
    {
        return FromUnrealVector(Cam->GetComponentLocation());
    }
    return CPPCore::Vector3();
}

CPPCore::Vector3 FUnrealCharacterController::GetCameraForward() const
{
    if (UCameraComponent * Cam = Camera.Get())
    {
        return FromUnrealVector(Cam->GetForwardVector());
    }
    return CPPCore::Vector3(1, 0, 0);
}

CPPCore::Vector3 FUnrealCharacterController::GetCameraRight() const
{
    if (UCameraComponent * Cam = Camera.Get())
    {
        return FromUnrealVector(Cam->GetRightVector());
    }
    return CPPCore::Vector3(0, 1, 0);
}