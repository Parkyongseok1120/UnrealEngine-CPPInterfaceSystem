#pragma once
#include <functional>
#include <algorithm>

using namespace std;

namespace Core
{
	class StaminaSystem
	{
	public:
		StaminaSystem(float MaxStaminaValue = 100.0f);

		void UseStamina(float Amount);
		void RegenerateStamina(float DeltaTime);
		bool HasEnoughStamina(float Amount) const;

		float GetCurrentStamina() const {return CurrentStamina;}
		float GetMaxStamina() const {return MaxStamina;}

		// Events
		function<void(float, float)> OnStaminaChanged; // Old, New
		function<void()> OnStaminaDepleted;


	private:
		float CurrentStamina; 
		float MaxStamina;
		float RegenRate; 
	};
}