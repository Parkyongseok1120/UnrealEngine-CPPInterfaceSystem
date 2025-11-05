#pragma once
#include <functional>
#include <algorithm>

using namespace std;

namespace CPPCore
{
	class CPPINTERFACECORE_API StaminaSystem
	{
	public:
		StaminaSystem(float MaxStaminaValue = 100.0f);

		void UseStamina(float Amount);
		void RegenerateStamina(float DeltaTime);
		bool HasEnoughStamina(float Amount) const;

		float GetCurrentStamina() const {return CurrentStamina;}
		float GetMaxStamina() const {return MaxStamina;}

		void SetRegenRate(float NewRegenRate) {RegenRate = NewRegenRate;}


		// Events
		function<void(float, float)> OnStaminaChanged; // Old, New
		function<void()> OnStaminaDepleted;


	private:
		float CurrentStamina; 
		float MaxStamina;
		float RegenRate; 
	};
}