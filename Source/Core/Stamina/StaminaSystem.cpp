#include "StaminaSystem.h"
#include <algorithm>

using namespace std;

namespace Core
{
	StaminaSystem::StaminaSystem(float MaxStaminaValue)
			: MaxStamina(MaxStaminaValue),
			 CurrentStamina(MaxStaminaValue),
			 RegenRate(10.0f)
	{
	}

	void StaminaSystem::UseStamina(float Amount)
	{
		float OldStamina = CurrentStamina;
		CurrentStamina = max(0.0f, CurrentStamina - Amount);

		if (OnStaminaChanged && OldStamina > CurrentStamina)
		{
			OnStaminaChanged(OldStamina, CurrentStamina);
		}

		if (CurrentStamina == 0.0f && OldStamina > 0.0f && OnStaminaChanged)
		{
			OnStaminaDepleted();
		}
	}

	void StaminaSystem::RegenerateStamina(float DeltaTime)
	{
		if (CurrentStamina < MaxStamina)
		{
			float OldStamina = CurrentStamina;
			CurrentStamina = min(MaxStamina, CurrentStamina + RegenRate * DeltaTime);

			if (OnStaminaChanged && OldStamina != CurrentStamina)
			{
				OnStaminaChanged(OldStamina, CurrentStamina);
			}
		}
	}

	bool StaminaSystem::HasEnoughStamina(float Amount) const
	{
		return CurrentStamina >= Amount;
	}
}


