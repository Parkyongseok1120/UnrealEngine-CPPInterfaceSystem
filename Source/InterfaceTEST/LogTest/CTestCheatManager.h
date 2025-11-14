#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CheatManager.h"
#include "CTestCheatManager.generated.h"

UCLASS()
class INTERFACETEST_API UCTestCheatManager : public UCheatManager
{
	GENERATED_BODY()

public:
	// Health 관련
	UFUNCTION(Exec, Category = "Test|Health")
	void TestDamage(float Amount);

	UFUNCTION(Exec, Category = "Test|Health")
	void TestHeal(int32 Amount);

	UFUNCTION(Exec, Category = "Test|Health")
	void TestKill();

	UFUNCTION(Exec, Category = "Test|Health")
	void TestRevive();

	// Stamina 관련
	UFUNCTION(Exec, Category = "Test|Stamina")
	void TestStaminaDrain(float Amount);

	UFUNCTION(Exec, Category = "Test|Stamina")
	void TestStaminaRestore();

	// FOV 관련
	UFUNCTION(Exec, Category = "Test|FOV")
	void TestPrintFOV();

	// 통합 상태 출력
	UFUNCTION(Exec, Category = "Test|Status")
	void TestPrintStatus();

	// 자동 테스트 시작
	UFUNCTION(Exec, Category = "Test|Auto")
	void TestAutoRun();

private:
	class ACPlayerCharacter* GetPlayerCharacter();
};