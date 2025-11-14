#include "Health/HealthSystem.h"
#include <algorithm>
#include <cmath>  // round 함수 사용

namespace CPPCore
{
	HealthSystem::HealthSystem(int32_t MaxHealthValue) 
		: MaxHealth(MaxHealthValue), CurrentHealth(MaxHealthValue)
	{
	}

	void HealthSystem::TakeDamage(float DamageAmount)
	{
		int32 OldHealth = CurrentHealth;
		
		// 수정: 반올림 적용으로 작은 데미지도 처리
		int32 DamageInt = static_cast<int32>(std::round(DamageAmount));
		
		// 최소 1 데미지 보장 (DamageAmount > 0이면)
		if (DamageAmount > 0.0f && DamageInt == 0)
		{
			DamageInt = 1;
		}
		
		CurrentHealth = std::max(0, CurrentHealth - DamageInt);

		// 실제로 변화가 있을 때만 콜백 호출
		if (OnHealthChanged && OldHealth != CurrentHealth)
		{
			OnHealthChanged(OldHealth, CurrentHealth);
		}

		if (CurrentHealth == 0 && OldHealth > 0 && OnDeath)
		{
			OnDeath();
		}
	}

	void HealthSystem::Heal(int32 HealthAmount)
	{
		int32 OldHealth = CurrentHealth;
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