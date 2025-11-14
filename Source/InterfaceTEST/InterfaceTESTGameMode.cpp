// Copyright Epic Games, Inc. All Rights Reserved.

#include "InterfaceTESTGameMode.h"
#include "InterfaceTESTCharacter.h"
#include "LogTest/CTestCheatManager.h"
#include "UObject/ConstructorHelpers.h"

AInterfaceTESTGameMode::AInterfaceTESTGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
		
	}
}
