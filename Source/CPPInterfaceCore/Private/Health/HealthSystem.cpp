#include "Health/HealthSystem.h"
#include <algorithm>

namespace CPPCore
{
	HealthSystem::HealthSystem(int32_t MaxHealthValue) : MaxHealth(MaxHealthValue), CurrentHealth(MaxHealthValue)
	{
	}

	void HealthSystem::TakeDamage(float DamageAmount)
	{
		int32_t OldHealth = CurrentHealth;
		CurrentHealth = max(0,CurrentHealth - static_cast<int32_t>(DamageAmount));

		if (OnHealthChanged)
		{
			OnHealthChanged(OldHealth, CurrentHealth);
		}

		if (CurrentHealth == 0 && OldHealth > 0 && OnDeath)
		{
			OnDeath();
		}
	}

	void HealthSystem::Heal(int32_t HealthAmount)
	{
		int32_t OldHealth = CurrentHealth;
		CurrentHealth = std::min(CurrentHealth + HealthAmount, MaxHealth);

		if (OnHealthChanged && OldHealth != CurrentHealth)
		{
			OnHealthChanged(OldHealth, CurrentHealth);
		}
	}

	bool HealthSystem::IsAlive() const
	{
		return CurrentHealth > 0;
	}
}
