#pragma once
#include "../Math/MathTypes.h"

namespace Core
{
	struct ICharacterController
	{
	public:
		virtual ~ICharacterController() = default;

		//Movement
		virtual void AddMovementInput(const Vector3& Direction, float Scale) = 0;
		virtual void Jump() = 0;
		virtual bool IsGrounded() const = 0;
		virtual void SetMovementSpeed(float Speed) = 0;

		//Transforms
		virtual void SetPosition(const Vector3& Position) = 0;
		virtual Vector3 GetPosition() const = 0;
		virtual void SetRotation(const Vector3& Rotation) = 0;
		virtual Vector3 GetRotation() const =0;

		//Camera
		virtual void SetCameraFOV(float FOV) = 0;
		virtual float GetCameraFOV() const = 0 ;
		virtual Vector3 GetCameraLocation() const = 0;
		virtual Vector3 GetCameraForward() const =0;
		virtual Vector3 GetCameraRight() const = 0;
	};
}
