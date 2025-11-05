#include "Misc/AutomationTest.h"
#include "Character/CPlayerCharacter.h"
#include "Character/UnrealCharacterController.h"
#include "Character/PlayerCharacter_DataAsset.h"
#include "GameFramework/Character.h"
#include "Camera/CameraComponent.h"

// ==================== TEST 1: UnrealCharacterController 통합 ====================
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FUnrealControllerIntegrationTest,
    "InterfaceTEST.Integration.UnrealController",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter
)

bool FUnrealControllerIntegrationTest::RunTest(const FString& Parameters)
{
    UWorld* World = GEngine->GetWorldFromContextObject(GEngine->GameViewport, EGetWorldErrorMode::ReturnNull);
    
    if (!World)
    {
        AddWarning(TEXT("World not available for testing"));
        return false;
    }

    FActorSpawnParameters SpawnParams;
    SpawnParams.Name = FName(TEXT("TestCharacter"));
    
    ACharacter* TestCharacter = World->SpawnActor<ACharacter>(ACharacter::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
    
    if (!TestCharacter)
    {
        AddError(TEXT("Failed to spawn test character"));
        return false;
    }

    UCameraComponent* TestCamera = NewObject<UCameraComponent>(TestCharacter);
    if (!TestCamera)
    {
        AddError(TEXT("Failed to create camera component"));
        TestCharacter->Destroy();
        return false;
    }

    FUnrealCharacterController* Controller = new FUnrealCharacterController(TestCharacter, TestCamera);
    
    TestTrue(TEXT("Controller Created"), Controller != nullptr);

    CPPCore::Vector3 TestPos(100.0f, 200.0f, 300.0f);
    Controller->SetPosition(TestPos);
    
    CPPCore::Vector3 ResultPos = Controller->GetPosition();
    TestTrue(TEXT("Position Set Correctly"), 
        FMath::IsNearlyEqual(ResultPos.X, 100.0f, 1.0f) &&
        FMath::IsNearlyEqual(ResultPos.Y, 200.0f, 1.0f) &&
        FMath::IsNearlyEqual(ResultPos.Z, 300.0f, 1.0f)
    );

    Controller->SetMovementSpeed(600.0f);

    Controller->SetCameraFOV(75.0f);
    float ResultFOV = Controller->GetCameraFOV();
    TestTrue(TEXT("FOV Set Correctly"), FMath::IsNearlyEqual(ResultFOV, 75.0f, 0.1f));

    delete Controller;
    TestCharacter->Destroy();

    return true;
}

// ==================== TEST 2: DataAsset 변환 테스트 ====================
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FDataAssetConversionTest,
    "InterfaceTEST.Integration.DataAssetConversion",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter
)

bool FDataAssetConversionTest::RunTest(const FString& Parameters)
{
    UPlayerCharacter_DataAsset* TestAsset = NewObject<UPlayerCharacter_DataAsset>();
    
    if (!TestAsset)
    {
        AddError(TEXT("Failed to create DataAsset"));
        return false;
    }
    
    TestAsset->WalkSpeed = 450.0f;
    TestAsset->SprintSpeed = 650.0f;
    TestAsset->MaxHealth = 120;
    TestAsset->MaxStamina = 110.0f;
    TestAsset->DefaultFOV = 85.0f;
    
    CPPCore::CharacterStatsConfig CoreConfig = TestAsset->ToCore();

    TestEqual(TEXT("WalkSpeed Converted"), CoreConfig.WalkSpeed, 450.0f);
    TestEqual(TEXT("SprintSpeed Converted"), CoreConfig.SprintSpeed, 650.0f);
    TestEqual(TEXT("MaxHealth Converted"), CoreConfig.MaxHealth, 120);
    TestEqual(TEXT("MaxStamina Converted"), CoreConfig.MaxStamina, 110.0f);
    TestEqual(TEXT("DefaultFOV Converted"), CoreConfig.DefaultFOV, 85.0f);

    return true;
}

IMPLEMENT_COMPLEX_AUTOMATION_TEST(
    FFullIntegrationTest,
    "InterfaceTEST.Integration.FullFlow",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter
)

void FFullIntegrationTest::GetTests(TArray<FString>& OutBeautifiedNames, TArray<FString>& OutTestCommands) const
{
    OutBeautifiedNames.Add(TEXT("Character Creation"));
    OutTestCommands.Add(TEXT("CharacterCreation"));

    OutBeautifiedNames.Add(TEXT("Input Processing"));
    OutTestCommands.Add(TEXT("InputProcessing"));

    OutBeautifiedNames.Add(TEXT("Event Broadcasting"));
    OutTestCommands.Add(TEXT("EventBroadcasting"));
}

bool FFullIntegrationTest::RunTest(const FString& Parameters)
{
    UWorld* World = GEngine->GetWorldFromContextObject(GEngine->GameViewport, EGetWorldErrorMode::ReturnNull);
    
    if (!World)
    {
        AddWarning(TEXT("World not available"));
        return false;
    }

    if (Parameters == TEXT("CharacterCreation"))
    {
        ACPlayerCharacter* TestChar = World->SpawnActor<ACPlayerCharacter>(ACPlayerCharacter::StaticClass());
        
        TestTrue(TEXT("Character Created"), TestChar != nullptr);
        
        if (TestChar)
        {
            TestChar->Destroy();
        }
        
        return true;
    }
    else if (Parameters == TEXT("InputProcessing"))
    {
        AddInfo(TEXT("Input processing requires game runtime"));
        return true;
    }
    else if (Parameters == TEXT("EventBroadcasting"))
    {
        AddInfo(TEXT("Event broadcasting tested via functional tests"));
        return true;
    }

    return false;
}
