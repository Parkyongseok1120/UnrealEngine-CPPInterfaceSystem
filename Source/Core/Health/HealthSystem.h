#pragma once
#include <functional>
#include <algorithm>

using namespace std;

namespace Core
{
	class HealthSystem
	{
	public:
		HealthSystem(int32_t MaxHealthValue = 100);

		void TakeDamage(float DamageAmount);
		void Heal(int32_t HealthAmount);
		bool IsAlive() const;

		int32_t GetCurrentHealth() const {return CurrentHealth;}
		int32_t GetMaxHealth() const{return MaxHealth;}

		//Events
		function<void(int32_t, int32_t)> OnHealthChanged; //Old, New(Callback)
		function<void()> OnDeath; //Callback


	private:
		int32_t CurrentHealth;
		int32_t MaxHealth;
	};
}
