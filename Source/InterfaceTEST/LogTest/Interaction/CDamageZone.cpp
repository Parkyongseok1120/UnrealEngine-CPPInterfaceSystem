#include "CDamageZone.h"
#include "Character/CPlayerCharacter.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "TimerManager.h"

ACDamageZone::ACDamageZone()
{
    PrimaryActorTick.bCanEverTick = true;

    // Box Component 생성
    DamageBox = CreateDefaultSubobject<UBoxComponent>(TEXT("DamageBox"));
    RootComponent = DamageBox;
    DamageBox->SetBoxExtent(FVector(100.0f, 100.0f, 100.0f));
    DamageBox->SetCollisionProfileName(TEXT("OverlapAll"));

    // Visual Mesh 생성
    VisualMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VisualMesh"));
    VisualMesh->SetupAttachment(RootComponent);
    VisualMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    // Overlap 이벤트 바인딩
    DamageBox->OnComponentBeginOverlap.AddDynamic(this, &ACDamageZone::OnOverlapBegin);
    DamageBox->OnComponentEndOverlap.AddDynamic(this, &ACDamageZone::OnOverlapEnd);
}

void ACDamageZone::BeginPlay()
{
    Super::BeginPlay();
    
    CurrentTime = 0.0f;
    
    UE_LOG(LogTemp, Warning, TEXT("========================================"));
    UE_LOG(LogTemp, Warning, TEXT("[DamageZone] 생성 완료"));
    UE_LOG(LogTemp, Log, TEXT("[DamageZone] 데미지: %.1f, 간격: %.1f초마다"), 
        DamageAmount, DamageInterval);
    UE_LOG(LogTemp, Warning, TEXT("========================================"));
}

void ACDamageZone::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    
    CurrentTime += DeltaTime;

    if (!bContinuousDamage || CharacterStates.Num() == 0)
    {
        return;
    }

    // ========== Tick 시작 플래그 ==========
    bIsProcessingTick = true;
    // ======================================

    // 삭제할 캐릭터 목록 (nullptr 체크용)
    TArray<ACPlayerCharacter*> ToRemove;

    for (auto& Pair : CharacterStates)
    {
        ACPlayerCharacter* Character = Pair.Key;
        FDamageState& State = Pair.Value;

        // 유효성 체크
        if (!Character || !Character->IsValidLowLevel())
        {
            ToRemove.Add(Character);
            continue;
        }

        // 2초마다 데미지 적용
        float TimeSinceLastDamage = CurrentTime - State.LastDamageTime;
        if (TimeSinceLastDamage >= DamageInterval)
        {
            State.LastDamageTime = CurrentTime;
            
            // 데미지 적용
            Character->ApplyDamage(DamageAmount);
            
            // 로그 (데미지 적용할 때만)
            UE_LOG(LogTemp, Log, TEXT("[DamageZone] %s - %.1f 데미지 (체력: %d/%d)"),
                *Character->GetName(),
                DamageAmount,
                Character->GetCurrentHealth(),
                Character->GetMaxHealth());
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

void ACDamageZone::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (ACPlayerCharacter* Character = Cast<ACPlayerCharacter>(OtherActor))
    {
        // 새로운 상태 생성
        FDamageState NewState;
        NewState.EnteredTime = CurrentTime;
        NewState.LastDamageTime = CurrentTime;  // 진입 직후부터 카운트 시작

        CharacterStates.Add(Character, NewState);
        
        UE_LOG(LogTemp, Warning, TEXT("========================================"));
        UE_LOG(LogTemp, Warning, TEXT("[DamageZone] %s 진입!"), *Character->GetName());
        UE_LOG(LogTemp, Log, TEXT("[DamageZone] %.1f초마다 %.1f 데미지 예정"), 
            DamageInterval, DamageAmount);
        UE_LOG(LogTemp, Warning, TEXT("========================================"));
        
        // 즉시 데미지 (bContinuousDamage가 false일 경우)
        if (!bContinuousDamage)
        {
            Character->ApplyDamage(DamageAmount);
            UE_LOG(LogTemp, Log, TEXT("[DamageZone] 즉시 %.1f 데미지 적용"), DamageAmount);
        }
    }
}

void ACDamageZone::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    if (ACPlayerCharacter* Character = Cast<ACPlayerCharacter>(OtherActor))
    {
        if (FDamageState* State = CharacterStates.Find(Character))
        {
            float TimeInZone = CurrentTime - State->EnteredTime;
            
            UE_LOG(LogTemp, Warning, TEXT("========================================"));
            UE_LOG(LogTemp, Warning, TEXT("[DamageZone] %s 이탈!"), *Character->GetName());
            UE_LOG(LogTemp, Log, TEXT("[DamageZone] 체류 시간: %.1f초"), TimeInZone);
            UE_LOG(LogTemp, Log, TEXT("[DamageZone] 최종 체력: %d/%d"), 
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
            UE_LOG(LogTemp, Verbose, TEXT("[DamageZone] Tick 중이므로 지연 제거 예약: %s"), *Character->GetName());
        }
        else
        {
            CharacterStates.Remove(Character);
        }
        // ======================================
    }
}