#include "Misc/AutomationTest.h"
#include "Character/PlayerCharacterCore.h"
#include "Character/CharacterStatsConfig.h"
#include "Health/HealthSystem.h"
#include "Stamina/StaminaSystem.h"

using namespace CPPCore;

// ==================== Mock Controller ====================
class FMockCharacterController : public ICharacterController
{
public:
    Vector3 Position;
    Vector3 Rotation;
    float MovementSpeed = 400.0f;
    float CameraFOV = 90.0f;
    bool bIsGrounded = true;
    int32 JumpCallCount = 0;

    virtual void AddMovementInput(const Vector3& Direction, float Scale) override {}
    virtual void Jump() override { JumpCallCount++; }
    virtual bool IsGrounded() const override { return bIsGrounded; }
    virtual void SetMovementSpeed(float Speed) override { MovementSpeed = Speed; }
    
    virtual void SetPosition(const Vector3& InPosition) override { Position = InPosition; }
    virtual Vector3 GetPosition() const override { return Position; }
    virtual void SetRotation(const Vector3& InRotation) override { Rotation = InRotation; }
    virtual Vector3 GetRotation() const override { return Rotation; }
    
    virtual void SetCameraFOV(float FOV) override { CameraFOV = FOV; }
    virtual float GetCameraFOV() const override { return CameraFOV; }
    virtual Vector3 GetCameraLocation() const override { return Position; }
    virtual Vector3 GetCameraForward() const override { return Vector3(1, 0, 0); }
    virtual Vector3 GetCameraRight() const override { return Vector3(0, 1, 0); }
};
