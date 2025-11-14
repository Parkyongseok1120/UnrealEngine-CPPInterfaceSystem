#include "CHealingZone.h"
#include "Character/CPlayerCharacter.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"

ACHealingZone::ACHealingZone()
{
    PrimaryActorTick.bCanEverTick = true;

    // Box Component 생성
    HealingBox = CreateDefaultSubobject<UBoxComponent>(TEXT("HealingBox"));
    RootComponent = HealingBox;
    HealingBox->SetBoxExtent(FVector(100.0f, 100.0f, 100.0f));
    HealingBox->SetCollisionProfileName(TEXT("OverlapAll"));

    // Visual Mesh 생성
    VisualMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VisualMesh"));
    VisualMesh->SetupAttachment(RootComponent);
    VisualMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    // Overlap 이벤트 바인딩
    HealingBox->OnComponentBeginOverlap.AddDynamic(this, &ACHealingZone::OnOverlapBegin);
    HealingBox->OnComponentEndOverlap.AddDynamic(this, &ACHealingZone::OnOverlapEnd);
}

void ACHealingZone::BeginPlay()
{
    Super::BeginPlay();
    
    CurrentTime = 0.0f;
    
    UE_LOG(LogTemp, Warning, TEXT("========================================"));
    UE_LOG(LogTemp, Warning, TEXT("[HealingZone] 생성 완료"));
    UE_LOG(LogTemp, Log, TEXT("[HealingZone] 힐링: %d, 간격: %.1f초마다"), 
        HealAmount, HealingInterval);
    UE_LOG(LogTemp, Warning, TEXT("========================================"));
}

void ACHealingZone::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    
    CurrentTime += DeltaTime;

    if (!bContinuousHealing || CharacterStates.Num() == 0)
    {
        return;
    }

    // ========== Tick 시작 플래그 ==========
    bIsProcessingTick = true;
    // ======================================

    // 삭제할 캐릭터 목록
    TArray<ACPlayerCharacter*> ToRemove;

    for (auto& Pair : CharacterStates)
    {
        ACPlayerCharacter* Character = Pair.Key;
        FHealingState& State = Pair.Value;

        // 유효성 체크
        if (!Character || !Character->IsValidLowLevel())
        {
            ToRemove.Add(Character);
            continue;
        }

        // 체력이 최대치면 힐링 건너뛰기
        if (Character->GetCurrentHealth() >= Character->GetMaxHealth())
        {
            continue;
        }

        // 2초마다 힐링 적용
        float TimeSinceLastHeal = CurrentTime - State.LastHealTime;
        if (TimeSinceLastHeal >= HealingInterval)
        {
            State.LastHealTime = CurrentTime;
            
            // 힐링 적용
            int32 OldHealth = Character->GetCurrentHealth();
            Character->Heal(HealAmount);
            int32 NewHealth = Character->GetCurrentHealth();

            // 로그 (실제 변화가 있을 때만)
            if (OldHealth != NewHealth)
            {
                UE_LOG(LogTemp, Log, TEXT("[HealingZone] %s - +%d 힐링 (체력: %d/%d)"),
                    *Character->GetName(),
                    NewHealth - OldHealth,
                    Character->GetCurrentHealth(),
                    Character->GetMaxHealth());
            }
        }
    }

    // 무효화된 캐릭터 제거
    for (ACPlayerCharacter* Char : ToRemove)
    {
        CharacterStates.Remove(Char);
    }
    
    // ========== Tick 종료 플래그 ==========
    bIsProcessingTick = false;
    
    // Tick 중에 제거 요청된 캐릭터들 처리
    for (ACPlayerCharacter* Char : PendingRemoval)
    {
        CharacterStates.Remove(Char);
    }
    PendingRemoval.Empty();
    // ======================================
}

void ACHealingZone::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (ACPlayerCharacter* Character = Cast<ACPlayerCharacter>(OtherActor))
    {
        // 새로운 상태 생성
        FHealingState NewState;
        NewState.EnteredTime = CurrentTime;
        NewState.LastHealTime = CurrentTime;  // 진입 직후부터 카운트 시작

        CharacterStates.Add(Character, NewState);
        
        UE_LOG(LogTemp, Warning, TEXT("========================================"));
        UE_LOG(LogTemp, Warning, TEXT("[HealingZone] %s 진입!"), *Character->GetName());
        UE_LOG(LogTemp, Log, TEXT("[HealingZone] 현재 체력: %d/%d"), 
            Character->GetCurrentHealth(), 
            Character->GetMaxHealth());
        UE_LOG(LogTemp, Log, TEXT("[HealingZone] %.1f초마다 %d 힐링 예정"), 
            HealingInterval, HealAmount);
        UE_LOG(LogTemp, Warning, TEXT("========================================"));
        
        // 즉시 힐링 (bContinuousHealing이 false일 경우)
        if (!bContinuousHealing)
        {
            Character->Heal(HealAmount);
            UE_LOG(LogTemp, Log, TEXT("[HealingZone] 즉시 %d 힐링 적용"), HealAmount);
        }
    }
}

void ACHealingZone::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    if (ACPlayerCharacter* Character = Cast<ACPlayerCharacter>(OtherActor))
    {
        if (FHealingState* State = CharacterStates.Find(Character))
        {
            float TimeInZone = CurrentTime - State->EnteredTime;
            
            UE_LOG(LogTemp, Warning, TEXT("========================================"));
            UE_LOG(LogTemp, Warning, TEXT("[HealingZone] %s 이탈!"), *Character->GetName());
            UE_LOG(LogTemp, Log, TEXT("[HealingZone] 체류 시간: %.1f초"), TimeInZone);
            UE_LOG(LogTemp, Log, TEXT("[HealingZone] 최종 체력: %d/%d"), 
                Character->GetCurrentHealth(), 
                Character->GetMaxHealth());
            UE_LOG(LogTemp, Warning, TEXT("========================================"));
        }
        
        // ========== 안전한 제거 처리 ==========
        // Tick 진행 중이면 PendingRemoval에 추가 (지연 제거)
        // Tick이 아니면 즉시 제거
        if (bIsProcessingTick)
        {
            PendingRemoval.AddUnique(Character);
            UE_LOG(LogTemp, Verbose, TEXT("[HealingZone] Tick 중이므로 지연 제거 예약: %s"), *Character->GetName());
        }
        else
        {
            CharacterStates.Remove(Character);
        }
        // ======================================
    }
}