#include "CPlayerCharacter.h"
#include "UnrealCharacterController.h"
#include "../Core/Character/PlayerCharacterCore.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Kismet/GameplayStatics.h"

ACPlayerCharacter::ACPlayerCharacter()
    : CharacterCore(nullptr)
    , CharacterController(nullptr)
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

    if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
        {
            Subsystem->AddMappingContext(DefaultMappingContext, 0);
        }
    }

    if (CharacterController.IsValid())
    {
        CharacterController = MakeUnique<FUnrealCharacterController>(this, FollowCamera);
    }

    if (CharacterCore.IsValid())
    {
        CharacterCore = MakeUnique<Core::PlayerCharacterCore>(CharacterController.Get());

        CharacterCore->OnPositionChanged = [this](Core::Vector3 NewPos) { OnCore_PositionChanged(NewPos); };
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

    Super::EndPlay(EndPlayReason);

}

void ACPlayerCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (CharacterCore.IsValid())
    {
        CharacterCore->Update(DeltaTime);
    }
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
    if (CharacterCore && Controller)
    {
     
        FVector2D MovementVector = Value.Get<FVector2D>();
        CharacterCore->HandleMoveInput(Core::Vector2(MovementVector.X, MovementVector.Y));
    }
}

void ACPlayerCharacter::Input_Look(const FInputActionValue& Value)
{
    if (CharacterCore && Controller)
    {
        FVector2D LookAxisVector = Value.Get<FVector2D>();
        CharacterCore->HandleLookInput(Core::Vector2(LookAxisVector.X, LookAxisVector.Y));
    }
}

void ACPlayerCharacter::Input_Jump(const FInputActionValue& Value)
{
    if (CharacterCore)
    {
        CharacterCore->HandleJumpInput();
    }
}

void ACPlayerCharacter::Input_Sprint_Started(const FInputActionValue& Value)
{
    if (CharacterCore)
    {
        CharacterCore->HandleSprintStart();
    }
}

void ACPlayerCharacter::Input_Sprint_Completed(const FInputActionValue& Value)
{
    if (CharacterCore)
    {
        CharacterCore->HandleSprintEnd();
    }
}

void ACPlayerCharacter::Input_Zoom_Started(const FInputActionValue& Value)
{
    if (CharacterCore)
    {
        CharacterCore->HandleZoomStart();
    }
}

void ACPlayerCharacter::Input_Zoom_Completed(const FInputActionValue& Value)
{
    if (CharacterCore)
    {
        CharacterCore->HandleZoomEnd();
    }
}

void ACPlayerCharacter::Input_Attack(const FInputActionValue& Value)
{
    if (CharacterCore)
    {
        CharacterCore->HandleAttackInput();
    }
}

void ACPlayerCharacter::OnCore_PositionChanged(Core::Vector3 NewPosition)
{
    //발소리 같은 위치기반 이벤트 트리거
}

void ACPlayerCharacter::OnCore_SprintStateChanged(bool bIsSprinting)
{
    // 카메라 렉 업데이트(여기에서 FOV, 카메라 설정 또는 스프린트 효과 트리거 조정가능함.)
    CameraBoom->bEnableCameraLag = !bIsSprinting;
}

void ACPlayerCharacter::OnCore_ZoomStateChanged(bool bIsZooming)
{
    // 줌용 카메라 업데이트(위에와 마찬가지)
    CameraBoom->bEnableCameraLag = !bIsZooming;
}

void ACPlayerCharacter::OnCore_Jumped()
{
    //점프 애니메이션이나 파티클 여기에
}

void ACPlayerCharacter::OnCore_Landed()
{
    //땅 밟았을때 애니메이션이나 이펙트 여기에.
}

void ACPlayerCharacter::OnCore_Attack()
{
    // 공격시 로직, 애니메이션이나 소리
}

void ACPlayerCharacter::OnCore_HealthChanged(int32 OldHealth, int32 NewHealth)
{
    // 블루프린트에 브로드 캐스트(데미지 받을때 이펙트나 사운드여기다가 추가 가능.)
    OnHealthChanged.Broadcast(OldHealth, NewHealth);
}

void ACPlayerCharacter::OnCore_Death()
{
    // 죽음 관련 핸들
    GetCharacterMovement()->DisableMovement();
    GetMesh()->SetSimulatePhysics(true);
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    
    OnDeath.Broadcast();
}

void ACPlayerCharacter::OnCore_StaminaChanged(float OldStamina, float NewStamina)
{
    OnStaminaChanged.Broadcast(OldStamina, NewStamina);
}

void ACPlayerCharacter::OnCore_StaminaDepleted()
{
    // Handle stamina depletion (e.g., play exhausted animation)
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
    if (CharacterCore && CharacterCore->GetHealthSystem())
    {
        CharacterCore->GetHealthSystem()->TakeDamage(Amount);
    }
}

void ACPlayerCharacter::Heal(int32 Amount)
{
    if (CharacterCore && CharacterCore->GetHealthSystem())
    {
        CharacterCore->GetHealthSystem()->Heal(Amount);
    }
}
