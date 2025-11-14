#include "CPlayerCharacter.h"
#include "UnrealCharacterController.h"
#include "Character/PlayerCharacterCore.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "PlayerCharacter_DataAsset.h"
#include "GameFramework/CheatManager.h"
#include "Kismet/GameplayStatics.h"
#include "LogTest/CTestCheatManager.h"
#include "Public/Profiling/ProfilerMarkers.h"

ACPlayerCharacter::ACPlayerCharacter()
{
    GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;

    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
    GetCharacterMovement()->JumpZVelocity = 700.f;
    GetCharacterMovement()->AirControl = 0.35f;
    GetCharacterMovement()->MaxWalkSpeed = 600.f;
    GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
    GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);
    CameraBoom->TargetArmLength = 400.0f;
    CameraBoom->bUsePawnControlRotation = true;

    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
    FollowCamera->bUsePawnControlRotation = false;
    
}

ACPlayerCharacter::~ACPlayerCharacter()
{
   //TUniquePtr은 자동으로 해제하므로 따로넣지 않음.
}

void ACPlayerCharacter::BeginPlay()
{
    Super::BeginPlay();

    if (APlayerController* PC = Cast<APlayerController>(GetController()))
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
        {
            Subsystem->AddMappingContext(DefaultMappingContext, 0);
        }
    }

    CPPCore::CharacterStatsConfig Config;
    if (StatsAsset.IsValid())
    {
        Config = StatsAsset.Get()->ToCore();
        UE_LOG(LogTemp, Log, TEXT("캐릭터 스탯 데이터 에셋 로딩 : %s"), *StatsAsset->GetName());
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("캐릭터 스탯 데이터 에셋이 설정되지 않았음. 기본값 사용."));
    }

    if (!CharacterController.IsValid())
    {
        CharacterController = MakeUnique<FUnrealCharacterController>(this, FollowCamera);
    }

    if (!CharacterCore.IsValid())
    {
        CharacterCore = MakeUnique<CPPCore::PlayerCharacterCore>(CharacterController.Get(), Config);

        CharacterCore->OnPositionChanged = [this](CPPCore::Vector3 NewPos) { OnCore_PositionChanged(NewPos); };
        CharacterCore->OnSprintStateChanged = [this](bool bSprint) { OnCore_SprintStateChanged(bSprint); };
        CharacterCore->OnZoomStateChanged = [this](bool bZoom) { OnCore_ZoomStateChanged(bZoom); };
        CharacterCore->OnJumped = [this]() { OnCore_Jumped(); };
        CharacterCore->OnLanded = [this]() { OnCore_Landed(); };
        CharacterCore->OnAttack = [this]() { OnCore_Attack(); };

        CharacterCore->GetHealthSystem()->OnHealthChanged = [this](int32 Old, int32 New) { OnCore_HealthChanged(Old, New); };
        CharacterCore->GetHealthSystem()->OnDeath = [this]() { OnCore_Death(); };

        CharacterCore->GetStaminaSystem()->OnStaminaChanged = [this](float Old, float New) { OnCore_StaminaChanged(Old, New); };
        CharacterCore->GetStaminaSystem()->OnStaminaDepleted = [this]() { OnCore_StaminaDepleted(); };
    }

    LastPerformanceLogTime = 0.0f;

    UE_LOG(LogTemp, Warning, TEXT("========================================"));
    UE_LOG(LogTemp, Warning, TEXT("[Character] 초기화 완료"));
    UE_LOG(LogTemp, Log, TEXT("[Character] 체력: %d/%d"), GetCurrentHealth(), GetMaxHealth());
    UE_LOG(LogTemp, Log, TEXT("[Character] 스태미나: %.1f/%.1f"), GetCurrentStamina(), GetMaxStamina());
    UE_LOG(LogTemp, Warning, TEXT("========================================"));
}

void ACPlayerCharacter::PossessedBy(AController* NewController)
{
    Super::PossessedBy(NewController);
    
    if (APlayerController* PC = Cast<APlayerController>(NewController))
    {
        if (!PC->CheatManager || !PC->CheatManager->IsA(UCTestCheatManager::StaticClass()))
        {
            if (PC->CheatManager)
            {
                UE_LOG(LogTemp, Log, TEXT("기존 CheatManager(%s)를 UCTestCheatManager로 교체합니다."), *PC->CheatManager->GetClass()->GetName());
            }

            PC->CheatManager = NewObject<UCTestCheatManager>(PC, UCTestCheatManager::StaticClass());
            
            if (PC->CheatManager)
            {
                PC->CheatManager->InitCheatManager();
                UE_LOG(LogTemp, Warning, TEXT("CheatManager 생성(교체) 완료!"));
            }
        }
        else
        {
            UE_LOG(LogTemp, Log, TEXT("이미 UCTestCheatManager가 적용되어 있습니다."));
        }
    }
}

void ACPlayerCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    UE_LOG(LogTemp, Verbose, TEXT("ACPlayerChracter::EndPlay 델리게이트 및 코어 오브젝트 해제"));

    if (CharacterCore.IsValid())
    {
        CharacterCore->OnPositionChanged = nullptr;
        CharacterCore->OnSprintStateChanged = nullptr;
        CharacterCore->OnZoomStateChanged = nullptr;
        CharacterCore->OnJumped = nullptr;
        CharacterCore->OnLanded = nullptr;
        CharacterCore->OnAttack = nullptr;

        if (CharacterCore->GetHealthSystem())
        {
            CharacterCore->GetHealthSystem()->OnHealthChanged = nullptr;
            CharacterCore->GetHealthSystem()->OnDeath = nullptr;
        }

        if (CharacterCore->GetStaminaSystem())
        {
            CharacterCore->GetStaminaSystem()->OnStaminaChanged = nullptr;
            CharacterCore->GetStaminaSystem()->OnStaminaDepleted = nullptr;
        }

        CharacterCore.Reset();
        CharacterController.Reset();
    }
    
    if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(InputComponent))
    {
        EIC->ClearActionBindings();
        UE_LOG(LogTemp, Verbose, TEXT("ACPlayerCharacter::EndPlay - 입력 바인딩 해제"));
    }
    
    Super::EndPlay(EndPlayReason);
    UE_LOG(LogTemp, Verbose, TEXT("ACPlayerCharacter::EndPlay - 해제 완료."));
}

void ACPlayerCharacter::Tick(float DeltaTime)
{
    PROFILE_SCOPE_CHARACTER_TICK();
    PROFILE_SCOPE_W2_TICK();
    PROFILE_W2_BOOKMARK_TICK_START();
    
    // ========== 성능 로그 간격 제어 (2초마다) ==========
    static float PerformanceLogInterval = 2.0f;  
    float CurrentTime = GetWorld()->GetTimeSeconds();
    bool bShouldLogPerformance = (CurrentTime - LastPerformanceLogTime) >= PerformanceLogInterval;
    
    double TickStartTime = 0.0;
    if (bShouldLogPerformance)
    {
        TickStartTime = FPlatformTime::Seconds();
    }
    // ==================================================
    
    Super::Tick(DeltaTime);

    if (CharacterCore.IsValid())
    {
        PROFILE_SCOPE_CORE_UPDATE();
        CharacterCore->Update(DeltaTime);
    }

    // ========== 2초마다만 성능 로그 출력 ==========
    if (bShouldLogPerformance)
    {
        double TickEndTime = FPlatformTime::Seconds();
        double TickDuration = (TickEndTime - TickStartTime) * 1000.0;
        
        UE_LOG(LogTemp, Log, TEXT("[W2 Performance] Character Tick: %.3f ms"), TickDuration);
        LastPerformanceLogTime = CurrentTime;
    }
    
    PROFILE_W2_BOOKMARK_TICK_END();
}

void ACPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ACPlayerCharacter::Input_Move);
        EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ACPlayerCharacter::Input_Look);
        EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACPlayerCharacter::Input_Jump);
        EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &ACPlayerCharacter::Input_Sprint_Started);
        EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &ACPlayerCharacter::Input_Sprint_Completed);
        EnhancedInputComponent->BindAction(ZoomAction, ETriggerEvent::Started, this, &ACPlayerCharacter::Input_Zoom_Started);
        EnhancedInputComponent->BindAction(ZoomAction, ETriggerEvent::Completed, this, &ACPlayerCharacter::Input_Zoom_Completed);
        EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started, this, &ACPlayerCharacter::Input_Attack);
    }
}

void ACPlayerCharacter::Input_Move(const FInputActionValue& Value)
{
    PROFILE_SCOPE_MOVEMENT();
    if (!CharacterCore) return;
    
    FVector2D MovementVector = Value.Get<FVector2D>();
    CharacterCore->HandleMoveInput(CPPCore::Vector2(MovementVector.X, MovementVector.Y));
}

void ACPlayerCharacter::Input_Look(const FInputActionValue& Value)
{
    if (!CharacterCore) return;
    
    FVector2D LookAxisVector = Value.Get<FVector2D>();
    CharacterCore->HandleLookInput(CPPCore::Vector2(LookAxisVector.X, LookAxisVector.Y));
}

void ACPlayerCharacter::Input_Jump(const FInputActionValue& Value)
{
    PROFILE_SCOPE_JUMP();
    PROFILE_SCOPE_W2_PHYSICS();
    PROFILE_W2_BOOKMARK_PHYSICS_START();
    
    if (!CharacterCore) return;
    
    UE_LOG(LogTemp, Log, TEXT("[Input] 점프 시도"));
    PROFILE_W2_EVENT_JUMP();
    CharacterCore->HandleJumpInput();

    PROFILE_BOOKMARK("PlayerJumped");
    PROFILE_W2_BOOKMARK_PHYSICS_END();
}

void ACPlayerCharacter::Input_Sprint_Started(const FInputActionValue& Value)
{
    if (!CharacterCore) return;
    
    UE_LOG(LogTemp, Log, TEXT("[Input] 스프린트 시작 입력"));
    PROFILE_W2_EVENT_SPRINT_START();
    CharacterCore->HandleSprintStart();
}

void ACPlayerCharacter::Input_Sprint_Completed(const FInputActionValue& Value)
{
    if (!CharacterCore) return;
    
    UE_LOG(LogTemp, Log, TEXT("[Input] 스프린트 종료 입력"));
    PROFILE_W2_EVENT_SPRINT_END();
    CharacterCore->HandleSprintEnd();
}

void ACPlayerCharacter::Input_Zoom_Started(const FInputActionValue& Value)
{
    if (!CharacterCore) return;
    
    UE_LOG(LogTemp, Log, TEXT("[Input] 줌 시작 입력"));
    PROFILE_W2_EVENT_ZOOM_CHANGED(true);
    CharacterCore->HandleZoomStart();
}

void ACPlayerCharacter::Input_Zoom_Completed(const FInputActionValue& Value)
{
    if (!CharacterCore) return;
    
    UE_LOG(LogTemp, Log, TEXT("[Input] 줌 종료 입력"));
    PROFILE_W2_EVENT_ZOOM_CHANGED(false);
    CharacterCore->HandleZoomEnd();
}

void ACPlayerCharacter::Input_Attack(const FInputActionValue& Value)
{
    if (!CharacterCore) return;
    
    UE_LOG(LogTemp, Log, TEXT("[Input] 공격 입력"));
    PROFILE_W2_EVENT_ATTACK();
    CharacterCore->HandleAttackInput();
}

void ACPlayerCharacter::OnCore_PositionChanged(CPPCore::Vector3 NewPosition)
{
    //발소리 같은 위치기반 이벤트 트리거
}

void ACPlayerCharacter::OnCore_SprintStateChanged(bool bIsSprinting)
{
    CameraBoom->bEnableCameraLag = !bIsSprinting;
    
    UE_LOG(LogTemp, Warning, TEXT("========================================"));
    UE_LOG(LogTemp, Warning, TEXT("[Sprint] 상태 변화: %s"), bIsSprinting ? TEXT("시작") : TEXT("종료"));
    UE_LOG(LogTemp, Log, TEXT("[Sprint] 현재 스태미나: %.1f/%.1f"), GetCurrentStamina(), GetMaxStamina());
    UE_LOG(LogTemp, Warning, TEXT("========================================"));
}

void ACPlayerCharacter::OnCore_ZoomStateChanged(bool bIsZooming)
{
    PROFILE_SCOPE_FOV();
    PROFILE_SCOPE_W2_POSTPROCESS();
    PROFILE_W2_BOOKMARK_POSTPROCESS_START();
    
    CameraBoom->bEnableCameraLag = !bIsZooming;
    
    // FOV 로그 추가
    if (FollowCamera)
    {
        UE_LOG(LogTemp, Warning, TEXT("========================================"));
        UE_LOG(LogTemp, Warning, TEXT("[Zoom] 상태 변화: %s"), bIsZooming ? TEXT("ON") : TEXT("OFF"));
        UE_LOG(LogTemp, Log, TEXT("[Zoom] 현재 FOV: %.1f"), FollowCamera->FieldOfView);
        UE_LOG(LogTemp, Warning, TEXT("========================================"));
    }
    
    PROFILE_W2_BOOKMARK_POSTPROCESS_END();
}

void ACPlayerCharacter::OnCore_Jumped()
{
    PROFILE_BOOKMARK("JumpAnimationStart"); 
    
    UE_LOG(LogTemp, Log, TEXT("[Jump] 점프 실행!"));
}

void ACPlayerCharacter::OnCore_Landed()
{
    UE_LOG(LogTemp, Log, TEXT("[Jump] 착지!"));
}

void ACPlayerCharacter::OnCore_Attack()
{
    PROFILE_SCOPE_ATTACK();
    PROFILE_SCOPE_W2_COMBAT();
    PROFILE_W2_BOOKMARK_COMBAT_START();
    
    PROFILE_BOOKMARK("PlayerAttacked");
    
    UE_LOG(LogTemp, Log, TEXT("[Combat] 공격 실행!"));
    
    PROFILE_W2_BOOKMARK_COMBAT_END();
}

void ACPlayerCharacter::OnCore_HealthChanged(int32 OldHealth, int32 NewHealth)
{
    PROFILE_SCOPE_HEALTH(); 
    PROFILE_W2_EVENT_HEALTH_CHANGED(OldHealth, NewHealth);
    
    // 변화가 없으면 리턴 (로그 스팸 방지)
    if (OldHealth == NewHealth)
    {
        return;
    }
    
    // Stat 업데이트 (실시간 반영)
    PROFILE_UPDATE_CURRENT_HEALTH(NewHealth);
    
    if (NewHealth < OldHealth)
    {
        // 데미지 받음
        PROFILE_SCOPE_DAMAGE();
        PROFILE_BOOKMARK("PlayerDamaged");
        PROFILE_W2_EVENT_DAMAGE_TAKEN();
        
        int32 DamageAmount = OldHealth - NewHealth;
        PROFILE_UPDATE_DAMAGE(DamageAmount);  // ⭐ 누적 데미지 Stat
        
        UE_LOG(LogTemp, Warning, TEXT("========================================"));
        UE_LOG(LogTemp, Warning, TEXT("[Health] 데미지를 받음!"));
        UE_LOG(LogTemp, Log, TEXT("[Health] 체력 변화: %d -> %d (-%d)"), 
            OldHealth, NewHealth, DamageAmount);
        UE_LOG(LogTemp, Warning, TEXT("========================================"));
    }
    else 
    {
        // 힐링
        PROFILE_SCOPE_HEAL();  
        
        int32 HealAmount = NewHealth - OldHealth;
        PROFILE_UPDATE_HEALING(HealAmount); 
        
        UE_LOG(LogTemp, Warning, TEXT("========================================"));
        UE_LOG(LogTemp, Warning, TEXT("[Health] 힐링!"));
        UE_LOG(LogTemp, Log, TEXT("[Health] 체력 변화: %d -> %d (+%d)"), 
            OldHealth, NewHealth, HealAmount);
        UE_LOG(LogTemp, Warning, TEXT("========================================"));
    }
    
    OnHealthChanged.Broadcast(OldHealth, NewHealth);
}

// ========== OnCore_Death 수정 ==========
void ACPlayerCharacter::OnCore_Death()
{
    PROFILE_SCOPE_DEATH();  
    PROFILE_UPDATE_DEATH_COUNT();
    
    // 죽음 관련 핸들
    GetCharacterMovement()->DisableMovement();
    GetMesh()->SetSimulatePhysics(true);
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    
    UE_LOG(LogTemp, Error, TEXT("========================================"));
    UE_LOG(LogTemp, Error, TEXT("[Health] 캐릭터 사망!"));
    UE_LOG(LogTemp, Error, TEXT("[Health] 최종 체력: 0"));
    UE_LOG(LogTemp, Error, TEXT("========================================"));
    
    OnDeath.Broadcast();
}

void ACPlayerCharacter::OnCore_StaminaChanged(float OldStamina, float NewStamina)
{
    float Delta = FMath::Abs(NewStamina - OldStamina);
    
    if (Delta > 0.5f) 
    {
        if (NewStamina < OldStamina)
        {
            UE_LOG(LogTemp, Log, TEXT("[Stamina] 소모: %.1f -> %.1f (-%.1f)"), OldStamina, NewStamina, Delta);
        }
        else
        {
            UE_LOG(LogTemp, Log, TEXT("[Stamina] 회복: %.1f -> %.1f (+%.1f)"), OldStamina, NewStamina, Delta);
        }
    }
    
    OnStaminaChanged.Broadcast(OldStamina, NewStamina);
}

void ACPlayerCharacter::OnCore_StaminaDepleted()
{
    PROFILE_BOOKMARK("StaminaDepleted");
    PROFILE_W2_EVENT_STAMINA_DEPLETED();
    
    UE_LOG(LogTemp, Warning, TEXT("========================================"));
    UE_LOG(LogTemp, Warning, TEXT("[Stamina] 스태미나 고갈!"));
    UE_LOG(LogTemp, Log, TEXT("[Stamina] 현재 스태미나: 0.0/%.1f"), GetMaxStamina());
    UE_LOG(LogTemp, Warning, TEXT("========================================"));
}

bool ACPlayerCharacter::IsSprinting() const
{
    return CharacterCore ? CharacterCore->IsSprinting() : false;
}

bool ACPlayerCharacter::IsZooming() const
{
    return CharacterCore ? CharacterCore->IsZooming() : false;
}

bool ACPlayerCharacter::CanJump() const
{
    return CharacterCore ? CharacterCore->CanJump() : false;
}

int32 ACPlayerCharacter::GetCurrentHealth() const
{
    return CharacterCore ? CharacterCore->GetHealthSystem()->GetCurrentHealth() : 0;
}

int32 ACPlayerCharacter::GetMaxHealth() const
{
    return CharacterCore ? CharacterCore->GetHealthSystem()->GetMaxHealth() : 0;
}

float ACPlayerCharacter::GetCurrentStamina() const
{
    return CharacterCore ? CharacterCore->GetStaminaSystem()->GetCurrentStamina() : 0.0f;
}

float ACPlayerCharacter::GetMaxStamina() const
{
    return CharacterCore ? CharacterCore->GetStaminaSystem()->GetMaxStamina() : 0.0f;
}

void ACPlayerCharacter::ApplyDamage(float Amount)
{
    PROFILE_SCOPE_DAMAGE();  
    
    if (CharacterCore && CharacterCore->GetHealthSystem())
    {
        CharacterCore->GetHealthSystem()->TakeDamage(Amount);
    }
}

void ACPlayerCharacter::Heal(int32 Amount)
{
    PROFILE_SCOPE_HEAL();
    
    if (CharacterCore && CharacterCore->GetHealthSystem())
    {
        CharacterCore->GetHealthSystem()->Heal(Amount);
    }
}