#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Character/PlayerCharacterCore.h"
#include "UnrealCharacterController.h" 
#include "Public/Profiling/ProfilerMarkers.h"
#include "CPlayerCharacter.generated.h"

//전방 선언
class UCameraComponent;
class USpringArmComponent;
class UInputMappingContext;
class UInputAction;
class UCharacterStatsDataAsset;
class CPPCore::PlayerCharacterCore;
class UPlayerCharacter_DataAsset;
class FUnrealCharacterController;

UCLASS()
class INTERFACETEST_API ACPlayerCharacter : public ACharacter
{
public:
    GENERATED_BODY()

public:
    ACPlayerCharacter();
    virtual ~ACPlayerCharacter() override;

    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    virtual void PossessedBy(AController* NewController) override;

    // ===========Input============
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    UInputMappingContext* DefaultMappingContext;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    UInputAction* MoveAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    UInputAction* LookAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    UInputAction* JumpAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    UInputAction* SprintAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    UInputAction* ZoomAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    UInputAction* AttackAction;

    //==================Config(DataAsset)========================

    UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "Config|Stats")
    TWeakObjectPtr<UPlayerCharacter_DataAsset> StatsAsset;

    // ==================Components======================
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    USpringArmComponent* CameraBoom;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    UCameraComponent* FollowCamera;

    // ===============Core behavior handlers====================
    void Input_Move(const FInputActionValue& Value);
    void Input_Look(const FInputActionValue& Value);
    void Input_Jump(const FInputActionValue& Value);
    void Input_Sprint_Started(const FInputActionValue& Value);
    void Input_Sprint_Completed(const FInputActionValue& Value);
    void Input_Zoom_Started(const FInputActionValue& Value);
    void Input_Zoom_Completed(const FInputActionValue& Value);
    void Input_Attack(const FInputActionValue& Value);

    // ===============Core event handlers=======================
    void OnCore_PositionChanged(CPPCore::Vector3 NewPosition);
    void OnCore_SprintStateChanged(bool bIsSprinting);
    void OnCore_ZoomStateChanged(bool bIsZooming);
    void OnCore_Jumped();
    void OnCore_Landed();
    void OnCore_Attack();

    // ===============Health system callbacks===================
    void OnCore_HealthChanged(int32 OldHealth, int32 NewHealth);
    void OnCore_Death();

    // ==============Stamina system callbacks===================
    void OnCore_StaminaChanged(float OldStamina, float NewStamina);
    void OnCore_StaminaDepleted();

public:
    // ==============Blueprint-exposed functions=================
    UFUNCTION(BlueprintPure, Category = "Character")
    bool IsSprinting() const;

    UFUNCTION(BlueprintPure, Category = "Character")
    bool IsZooming() const;

    bool CanJump() const;

    UFUNCTION(BlueprintPure, Category = "Character|Health")
    int32 GetCurrentHealth() const;

    UFUNCTION(BlueprintPure, Category = "Character|Health")
    int32 GetMaxHealth() const;

    UFUNCTION(BlueprintPure, Category = "Character|Stamina")
    float GetCurrentStamina() const;

    UFUNCTION(BlueprintPure, Category = "Character|Stamina")
    float GetMaxStamina() const;

    UFUNCTION(BlueprintCallable, Category = "Character|Health")
    void ApplyDamage(float Amount);

    UFUNCTION(BlueprintCallable, Category = "Character|Health")
    void Heal(int32 Amount);

    // ==================블루프린트 이벤트 델리게이트===================
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHealthChangedSignature, int32, OldHealth, int32, NewHealth);
    UPROPERTY(BlueprintAssignable, Category = "Character|Events")
    FOnHealthChangedSignature OnHealthChanged;

    DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeathSignature);
    UPROPERTY(BlueprintAssignable, Category = "Character|Events")
    FOnDeathSignature OnDeath;

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnStaminaChangedSignature, float, OldStamina, float, NewStamina);
    UPROPERTY(BlueprintAssignable, Category = "Character|Events")
    FOnStaminaChangedSignature OnStaminaChanged;

private:
    // ============== Core 기능 (약참조)====================
    TUniquePtr<CPPCore::PlayerCharacterCore> CharacterCore;
    TUniquePtr<FUnrealCharacterController> CharacterController;

    // ============== 성능 로그 타이머 (2초 간격) ====================
    float LastPerformanceLogTime;
};