#include "CTestCheatManager.h"
#include "Character/CPlayerCharacter.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"

ACPlayerCharacter* UCTestCheatManager::GetPlayerCharacter()
{
	if (APlayerController* PC = GetOuterAPlayerController())
	{
		return Cast<ACPlayerCharacter>(PC->GetPawn());
	}
	return nullptr;
}

void UCTestCheatManager::TestDamage(float Amount)
{
	if (ACPlayerCharacter* Character = GetPlayerCharacter())
	{
		int32 OldHealth = Character->GetCurrentHealth();
		Character->ApplyDamage(Amount);
		int32 NewHealth = Character->GetCurrentHealth();
		
		UE_LOG(LogTemp, Warning, TEXT("========================================"));
		UE_LOG(LogTemp, Warning, TEXT("[Console Test] 데미지 적용: %.1f"), Amount);
		UE_LOG(LogTemp, Warning, TEXT("[Console Test] 체력 변화: %d -> %d"), OldHealth, NewHealth);
		UE_LOG(LogTemp, Warning, TEXT("========================================"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[Console Test] 플레이어 캐릭터를 찾을 수 없습니다!"));
	}
}

void UCTestCheatManager::TestHeal(int32 Amount)
{
	if (ACPlayerCharacter* Character = GetPlayerCharacter())
	{
		int32 OldHealth = Character->GetCurrentHealth();
		Character->Heal(Amount);
		int32 NewHealth = Character->GetCurrentHealth();
		
		UE_LOG(LogTemp, Warning, TEXT("========================================"));
		UE_LOG(LogTemp, Warning, TEXT("[Console Test] 힐링 적용: %d"), Amount);
		UE_LOG(LogTemp, Warning, TEXT("[Console Test] 체력 변화: %d -> %d"), OldHealth, NewHealth);
		UE_LOG(LogTemp, Warning, TEXT("========================================"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[Console Test] 플레이어 캐릭터를 찾을 수 없습니다!"));
	}
}

void UCTestCheatManager::TestKill()
{
	if (ACPlayerCharacter* Character = GetPlayerCharacter())
	{
		Character->ApplyDamage(9999.0f);
		
		UE_LOG(LogTemp, Warning, TEXT("========================================"));
		UE_LOG(LogTemp, Warning, TEXT("[Console Test] 캐릭터 사망 테스트"));
		UE_LOG(LogTemp, Warning, TEXT("[Console Test] 최종 체력: %d"), Character->GetCurrentHealth());
		UE_LOG(LogTemp, Warning, TEXT("========================================"));
	}
}

void UCTestCheatManager::TestRevive()
{
	if (ACPlayerCharacter* Character = GetPlayerCharacter())
	{
		Character->Heal(Character->GetMaxHealth());
		
		UE_LOG(LogTemp, Warning, TEXT("========================================"));
		UE_LOG(LogTemp, Warning, TEXT("[Console Test] 캐릭터 부활"));
		UE_LOG(LogTemp, Warning, TEXT("[Console Test] 현재 체력: %d/%d"), 
			Character->GetCurrentHealth(), Character->GetMaxHealth());
		UE_LOG(LogTemp, Warning, TEXT("========================================"));
	}
}

void UCTestCheatManager::TestStaminaDrain(float Amount)
{
	if (ACPlayerCharacter* Character = GetPlayerCharacter())
	{
		float OldStamina = Character->GetCurrentStamina();
		
		UE_LOG(LogTemp, Warning, TEXT("========================================"));
		UE_LOG(LogTemp, Warning, TEXT("[Console Test] 스태미나 소모 테스트"));
		UE_LOG(LogTemp, Warning, TEXT("[Console Test] 현재 스태미나: %.1f/%.1f"), 
			Character->GetCurrentStamina(), Character->GetMaxStamina());
		UE_LOG(LogTemp, Warning, TEXT("[Console Test] 스프린트를 사용해서 스태미나를 소모하세요!"));
		UE_LOG(LogTemp, Warning, TEXT("========================================"));
	}
}

void UCTestCheatManager::TestStaminaRestore()
{
	if (ACPlayerCharacter* Character = GetPlayerCharacter())
	{
		UE_LOG(LogTemp, Warning, TEXT("========================================"));
		UE_LOG(LogTemp, Warning, TEXT("[Console Test] 스태미나 회복 모니터링"));
		UE_LOG(LogTemp, Warning, TEXT("[Console Test] 현재 스태미나: %.1f/%.1f"), 
			Character->GetCurrentStamina(), Character->GetMaxStamina());
		UE_LOG(LogTemp, Warning, TEXT("[Console Test] 스프린트를 멈추고 회복을 확인하세요!"));
		UE_LOG(LogTemp, Warning, TEXT("========================================"));
	}
}

void UCTestCheatManager::TestPrintFOV()
{
	if (ACPlayerCharacter* Character = GetPlayerCharacter())
	{
		UE_LOG(LogTemp, Warning, TEXT("========================================"));
		UE_LOG(LogTemp, Warning, TEXT("[Console Test] FOV 상태"));
		UE_LOG(LogTemp, Warning, TEXT("[Console Test] 줌 중: %s"), Character->IsZooming() ? TEXT("예") : TEXT("아니오"));
		UE_LOG(LogTemp, Warning, TEXT("[Console Test] 줌 키를 눌러서 FOV 변화를 확인하세요!"));
		UE_LOG(LogTemp, Warning, TEXT("========================================"));
	}
}

void UCTestCheatManager::TestPrintStatus()
{
	if (ACPlayerCharacter* Character = GetPlayerCharacter())
	{
		UE_LOG(LogTemp, Warning, TEXT("========================================"));
		UE_LOG(LogTemp, Warning, TEXT("[Console Test] === 캐릭터 전체 상태 ==="));
		UE_LOG(LogTemp, Warning, TEXT("[Console Test] 체력: %d/%d"), 
			Character->GetCurrentHealth(), Character->GetMaxHealth());
		UE_LOG(LogTemp, Warning, TEXT("[Console Test] 스태미나: %.1f/%.1f"), 
			Character->GetCurrentStamina(), Character->GetMaxStamina());
		UE_LOG(LogTemp, Warning, TEXT("[Console Test] 스프린트 중: %s"), 
			Character->IsSprinting() ? TEXT("예") : TEXT("아니오"));
		UE_LOG(LogTemp, Warning, TEXT("[Console Test] 줌 중: %s"), 
			Character->IsZooming() ? TEXT("예") : TEXT("아니오"));
		UE_LOG(LogTemp, Warning, TEXT("[Console Test] 점프 가능: %s"), 
			Character->CanJump() ? TEXT("예") : TEXT("아니오"));
		UE_LOG(LogTemp, Warning, TEXT("========================================"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[Console Test] 플레이어 캐릭터를 찾을 수 없습니다!"));
	}
}

void UCTestCheatManager::TestAutoRun()
{
	UE_LOG(LogTemp, Warning, TEXT("========================================"));
	UE_LOG(LogTemp, Warning, TEXT("[Console Test] 자동 테스트 시작"));
	UE_LOG(LogTemp, Warning, TEXT("========================================"));

	if (ACPlayerCharacter* Character = GetPlayerCharacter())
	{
		// 초기 상태 출력
		TestPrintStatus();

		// Health 테스트
		UE_LOG(LogTemp, Warning, TEXT("\n[Test 1] Health 테스트 시작..."));
		TestDamage(20.0f);
		TestHeal(10);

		// Stamina 테스트
		UE_LOG(LogTemp, Warning, TEXT("\n[Test 2] Stamina 테스트 시작..."));
		UE_LOG(LogTemp, Warning, TEXT("스프린트 키를 눌러서 스태미나 소모를 확인하세요!"));

		// FOV 테스트
		UE_LOG(LogTemp, Warning, TEXT("\n[Test 3] FOV 테스트 시작..."));
		UE_LOG(LogTemp, Warning, TEXT("줌 키를 눌러서 FOV 변화를 확인하세요!"));

		// 최종 상태 출력
		TestPrintStatus();
	}

	UE_LOG(LogTemp, Warning, TEXT("========================================"));
	UE_LOG(LogTemp, Warning, TEXT("[Console Test] 자동 테스트 완료"));
	UE_LOG(LogTemp, Warning, TEXT("========================================"));
}