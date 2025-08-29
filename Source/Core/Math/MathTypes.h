#pragma once
#include <algorithm>
#include <cmath>
#include <iostream>
using namespace std;

namespace Core
{
	struct Vector2
	{
		float X, Y;

		Vector2() : X(0), Y(0) {}
		Vector2(float x, float y) : X(x), Y(y) {}

		Vector2 operator+ (const Vector2& Other) const {return Vector2(X+Other.X, Y+Other.Y);}
		Vector2 operator- (const Vector2& Other) const {return Vector2(X-Other.X, Y-Other.Y);}
		Vector2 operator* (float Scale) const {return Vector2(X*Scale, Y*Scale);}

	

		float Length() const {return sqrt(X*X + Y*Y);}
		Vector2 Normalize() const
		{
			//정규화 체크
			float L = Length();
			if (L>0.0001f) return Vector2(X/L, Y/L);
			return Vector2();
		}
	};

	struct Vector3
	{
		float X, Y, Z;

		Vector3() : X(0), Y(0), Z(0) {}
		Vector3(float x, float y, float z) : X(x), Y(y), Z(z) {}
		
		Vector3 operator+ (const Vector3& Other) const {return Vector3(X + Other.X, Y + Other.Y, Z + Other.Z);}
		Vector3 operator- (const Vector3& Other) const {return Vector3(X - Other.X, Y - Other.Y, Z - Other.Z);}
		Vector3 operator* (float Scale) const {return Vector3(X*Scale, Y*Scale, Z*Scale);}
		

		float Length() const {return sqrt(X*X + Y*Y + Z*Z);}

		Vector3 Normalize() const
		{
			float L = Length();
			if (L > 0.0001f)return Vector3 (X / L, Y / L, Z / L);
			return Vector3();
		}

		static float Dot(const Vector3& A, const Vector3& B)
		{
			return (A.X * B.X) + (A.Y * B.Y) + (A.Z * B.Z);
		}

		static Vector3 Cross(const Vector3& A, const Vector3& B)
		{
			return Vector3(
				(A.Y * B.Z) - (A.Z * B.Y),
				(A.Z * B.X) - (A.X * B.Z),
				(A.X * B.Y) - (A.Y * B.X)
			);
		}
	};

	struct Rotator
	{
		float Pitch, Yaw, Roll;

		Rotator() : Pitch(0), Yaw(0), Roll(0) {}
		Rotator(float Pitch, float Yaw, float Roll) : Pitch(Pitch), Yaw(Yaw), Roll(Roll) {}

		Vector3 ToForwardVector() const
		{
			float CP = cos(Pitch * 3.14159f / 180.0f);
			float SP = sin(Pitch * 3.14159f / 180.0f);
			float CY = cos(Yaw * 3.14159f / 180.0f);
			float SY = sin(Yaw * 3.14159f / 180.0f);

			return Vector3(CP * CY, CP * SY, SP);
		}


		Vector3 ToRightVector() const
		{
			float CY = cos(Yaw * 3.14159f / 180.0f);
			float SY = sin(Yaw * 3.14159f / 180.0f);

			return Vector3(SY, -CY, 0.0f);
		}
	};
}
