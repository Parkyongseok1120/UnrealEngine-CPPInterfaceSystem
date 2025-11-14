#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CDamageZone.generated.h"


// 캐릭터별 데미지 상태 추적
USTRUCT()
struct FDamageState
{
	GENERATED_BODY()

	// 진입 시간
	float EnteredTime = 0.0f;
    
	// 마지막 데미지 적용 시간
	float LastDamageTime = 0.0f;
};


UCLASS()
class INTERFACETEST_API ACDamageZone : public AActor
{
	GENERATED_BODY()
	
public:	
	ACDamageZone();

protected:
	virtual void BeginPlay() override;

public:    
	virtual void Tick(float DeltaTime) override;

	// ==================== 설정값 ====================
    
	// 데미지양
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
	float DamageAmount = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
	bool bContinuousDamage = true;

	// 데미지 적용 간격 (초) - 이 시간마다 데미지 적용
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
	float DamageInterval = 2.0f;

	// ==================== 컴포넌트 ====================
    
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UBoxComponent* DamageBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UStaticMeshComponent* VisualMesh;

private:
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	// 캐릭터별 데미지 상태
	TMap<class ACPlayerCharacter*, FDamageState> CharacterStates;
    
	// 현재 게임 시간
	float CurrentTime = 0.0f;
	
	// Tick 처리 중인지 플래그 (OnOverlapEnd와 동기화)
	bool bIsProcessingTick = false;
	
	// Tick 중에 제거 요청된 캐릭터 (지연 제거용)
	TArray<ACPlayerCharacter*> PendingRemoval;
};