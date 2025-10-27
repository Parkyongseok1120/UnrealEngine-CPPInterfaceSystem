// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Character/CharacterStatsConfig.h"
#include "PlayerCharacter_DataAsset.generated.h"

/**
 * 
 */

struct CPPCore::CharacterStatsConfig;

UCLASS(Blueprintable, Meta = (DisplayName = "PlayerStats_Config"))
class INTERFACETEST_API UPlayerCharacter_DataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	// ================= Conversion (DTO) =================
	CPPCore::CharacterStatsConfig ToCore() const;
	
	// =============== Movement ==============
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement", Meta = (ClampMin = "0.0", UIMin = "0.0"))
	float WalkSpeed = 400.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement", Meta = (ClampMin = "0.0", UIMin = "0.0"))
	float SprintSpeed = 600.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement", Meta = (ClampMin = "0.0", UIMin = "0.0"))
	float JumpForce = 500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement", Meta = (ClampMin = "1", UIMin = "1"))
	int32 MaxJumpCount = 2;

	// ==================== Camera ====================
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera", Meta = (ClampMin = "30.0", ClampMax = "120.0"))
	float DefaultFOV = 90.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera", Meta = (ClampMin = "30.0", ClampMax = "120.0"))
	float ZoomedFOV = 45.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera", Meta = (ClampMin = "0.1", UIMin = "0.1"))
	float ZoomInterpSpeed = 20.0f;

	// ==================== Health ====================
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Health", Meta = (ClampMin = "1", UIMin = "1"))
	int32 MaxHealth = 100;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Health", Meta = (ClampMin = "1", UIMin = "1"))
	int32 DefaultHealth = 100;

	// ==================== Stamina ====================
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stamina", Meta = (ClampMin = "0.0", UIMin = "0.0"))
	float MaxStamina = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stamina", Meta = (ClampMin = "0.0", UIMin = "0.0"))
	float StaminaRegenRate = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stamina", Meta = (ClampMin = "0.0", UIMin = "0.0"))
	float SprintStaminaCost = 15.0f;
	
	
};
