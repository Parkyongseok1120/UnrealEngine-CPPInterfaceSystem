#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CHealingZone.generated.h"



USTRUCT()
struct FHealingState
{
	GENERATED_BODY()

	// 진입 시간
	float EnteredTime = 0.0f;
    
	// 마지막 힐링 적용 시간
	float LastHealTime = 0.0f;
};

UCLASS()
class INTERFACETEST_API ACHealingZone : public AActor
{
	GENERATED_BODY()
	
public:	
	ACHealingZone();

protected:
	virtual void BeginPlay() override;

public:    
	virtual void Tick(float DeltaTime) override;

	// ==================== 설정값 ====================
    
	// 힐링양
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Healing")
	int32 HealAmount = 7;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Healing")
	bool bContinuousHealing = true;

	// 힐링 적용 간격 (초) - 이 시간마다 힐링 적용
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Healing")
	float HealingInterval = 2.0f;

	// ==================== 컴포넌트 ====================
    
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UBoxComponent* HealingBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UStaticMeshComponent* VisualMesh;

private:
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	// 캐릭터별 힐링 상태
	TMap<class ACPlayerCharacter*, FHealingState> CharacterStates;
    
	// 현재 게임 시간
	float CurrentTime = 0.0f;
	
	// Tick 처리 중인지 플래그 (OnOverlapEnd와 동기화)
	bool bIsProcessingTick = false;
	
	// Tick 중에 제거 요청된 캐릭터 (지연 제거용)
	TArray<ACPlayerCharacter*> PendingRemoval;
};