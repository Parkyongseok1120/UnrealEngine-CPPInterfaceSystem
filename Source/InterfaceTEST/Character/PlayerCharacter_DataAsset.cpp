// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/PlayerCharacter_DataAsset.h"

CPPCore::CharacterStatsConfig UPlayerCharacter_DataAsset::ToCore() const
{
	CPPCore::CharacterStatsConfig Config;

	// Movement
	Config.WalkSpeed = WalkSpeed;
	Config.SprintSpeed = SprintSpeed;
	Config.JumpForce = JumpForce;
	Config.MaxJumpCount = MaxJumpCount;

	// Camera
	Config.DefaultFOV = DefaultFOV;
	Config.ZoomedFOV = ZoomedFOV;
	Config.ZoomInterpSpeed = ZoomInterpSpeed;

	// Health
	Config.MaxHealth = MaxHealth;
	Config.DefaultHealth = DefaultHealth;

	// Stamina
	Config.MaxStamina = MaxStamina;
	Config.StaminaRegenRate = StaminaRegenRate;
	Config.SprintStaminaCost = SprintStaminaCost;

	return Config;
}