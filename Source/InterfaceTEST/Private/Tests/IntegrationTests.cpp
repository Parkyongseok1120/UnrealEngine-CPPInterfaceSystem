#include "Misc/AutomationTest.h"
#include "Character/CPlayerCharacter.h"
#include "Character/UnrealCharacterController.h"
#include "Character/PlayerCharacter_DataAsset.h"
#include "Character/PlayerCharacterCore.h"
#include "Health/HealthSystem.h"
#include "Stamina/StaminaSystem.h"
#include "GameFramework/Character.h"
#include "Camera/CameraComponent.h"
#include "Public/Profiling/ProfilerMarkers.h"

// ==================== 기존 TEST 1: UnrealCharacterController 통합 ====================
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

// ==================== 기존 TEST 2: DataAsset 변환 테스트 ====================
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

// ==================== 기존 TEST 3: Full Integration ====================
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

// ==================== W2 NEW TEST 1: Health System 유닛테스트 ====================
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FHealthSystemUnitTest,
    "InterfaceTEST.Unit.W2.HealthSystem",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter
)

bool FHealthSystemUnitTest::RunTest(const FString& Parameters)
{
    PROFILE_SCOPE_HEALTH();
    
    UE_LOG(LogTemp, Warning, TEXT("========================================"));
    UE_LOG(LogTemp, Warning, TEXT("[W2 Unit Test] Health System 시작"));
    UE_LOG(LogTemp, Warning, TEXT("========================================"));

    // Health System 생성
    CPPCore::HealthSystem HealthSystem(100);

    // 초기 상태 확인
    TestEqual(TEXT("초기 체력"), HealthSystem.GetCurrentHealth(), 100);
    TestEqual(TEXT("최대 체력"), HealthSystem.GetMaxHealth(), 100);
    TestTrue(TEXT("생존 상태"), HealthSystem.IsAlive());
    UE_LOG(LogTemp, Log, TEXT("[Test] 초기 상태 확인 완료 - 체력: %d/%d"), 
        HealthSystem.GetCurrentHealth(), HealthSystem.GetMaxHealth());

    // 콜백 테스트용 변수
    bool bHealthChangedCalled = false;
    bool bDeathCalled = false;
    int32 LastOldHealth = 0;
    int32 LastNewHealth = 0;

    HealthSystem.OnHealthChanged = [&](int32 Old, int32 New) {
        bHealthChangedCalled = true;
        LastOldHealth = Old;
        LastNewHealth = New;
        UE_LOG(LogTemp, Log, TEXT("[Health Callback] 체력 변화: %d -> %d"), Old, New);
    };

    HealthSystem.OnDeath = [&]() {
        bDeathCalled = true;
        UE_LOG(LogTemp, Warning, TEXT("[Health Callback] 사망!"));
    };

    // Test 1: 데미지 적용
    UE_LOG(LogTemp, Log, TEXT("\n[Test 1] 데미지 적용 (30 데미지)"));
    PROFILE_BOOKMARK("HealthTest_TakeDamage");
    HealthSystem.TakeDamage(30.0f);
    
    TestTrue(TEXT("체력 변화 콜백 호출"), bHealthChangedCalled);
    TestEqual(TEXT("데미지 후 체력"), HealthSystem.GetCurrentHealth(), 70);
    TestEqual(TEXT("콜백 - 이전 체력"), LastOldHealth, 100);
    TestEqual(TEXT("콜백 - 새 체력"), LastNewHealth, 70);
    TestTrue(TEXT("생존 유지"), HealthSystem.IsAlive());
    UE_LOG(LogTemp, Log, TEXT("[Test 1] 결과 - 체력: %d/%d"), 
        HealthSystem.GetCurrentHealth(), HealthSystem.GetMaxHealth());

    // Test 2: 힐링
    UE_LOG(LogTemp, Log, TEXT("\n[Test 2] 힐링 (+20)"));
    PROFILE_BOOKMARK("HealthTest_Heal");
    bHealthChangedCalled = false;
    HealthSystem.Heal(20);
    
    TestTrue(TEXT("힐링 콜백 호출"), bHealthChangedCalled);
    TestEqual(TEXT("힐링 후 체력"), HealthSystem.GetCurrentHealth(), 90);
    UE_LOG(LogTemp, Log, TEXT("[Test 2] 결과 - 체력: %d/%d"), 
        HealthSystem.GetCurrentHealth(), HealthSystem.GetMaxHealth());

    // Test 3: 최대치 초과 힐링 방지
    UE_LOG(LogTemp, Log, TEXT("\n[Test 3] 과다 힐링 방지 (+50, 최대 100)"));
    HealthSystem.Heal(50);
    TestEqual(TEXT("최대 체력 초과 방지"), HealthSystem.GetCurrentHealth(), 100);
    UE_LOG(LogTemp, Log, TEXT("[Test 3] 결과 - 체력: %d/%d (최대치 유지)"), 
        HealthSystem.GetCurrentHealth(), HealthSystem.GetMaxHealth());

    // Test 4: 사망
    UE_LOG(LogTemp, Log, TEXT("\n[Test 4] 사망 테스트 (150 데미지)"));
    PROFILE_BOOKMARK("HealthTest_Death");
    HealthSystem.TakeDamage(150.0f);
    
    TestTrue(TEXT("사망 콜백 호출"), bDeathCalled);
    TestEqual(TEXT("최소 체력 (0)"), HealthSystem.GetCurrentHealth(), 0);
    TestFalse(TEXT("사망 상태"), HealthSystem.IsAlive());
    UE_LOG(LogTemp, Error, TEXT("[Test 4] 결과 - 사망! 체력: %d"), 
        HealthSystem.GetCurrentHealth());

    UE_LOG(LogTemp, Warning, TEXT("========================================"));
    UE_LOG(LogTemp, Warning, TEXT("[W2 Unit Test] Health System 완료 ✓"));
    UE_LOG(LogTemp, Warning, TEXT("========================================\n"));

    return true;
}

// ==================== W2 NEW TEST 2: Stamina System 유닛테스트 ====================
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FStaminaSystemUnitTest,
    "InterfaceTEST.Unit.W2.StaminaSystem",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter
)

bool FStaminaSystemUnitTest::RunTest(const FString& Parameters)
{
    UE_LOG(LogTemp, Warning, TEXT("========================================"));
    UE_LOG(LogTemp, Warning, TEXT("[W2 Unit Test] Stamina System 시작"));
    UE_LOG(LogTemp, Warning, TEXT("========================================"));

    // Stamina System 생성
    CPPCore::StaminaSystem StaminaSystem(100.0f);
    StaminaSystem.SetRegenRate(10.0f);

    // 초기 상태
    TestEqual(TEXT("초기 스태미나"), StaminaSystem.GetCurrentStamina(), 100.0f);
    TestEqual(TEXT("최대 스태미나"), StaminaSystem.GetMaxStamina(), 100.0f);
    UE_LOG(LogTemp, Log, TEXT("[Test] 초기 상태 - 스태미나: %.1f/%.1f"), 
        StaminaSystem.GetCurrentStamina(), StaminaSystem.GetMaxStamina());

    // 콜백 변수
    bool bStaminaChangedCalled = false;
    bool bStaminaDepletedCalled = false;
    float LastOldStamina = 0.0f;
    float LastNewStamina = 0.0f;

    StaminaSystem.OnStaminaChanged = [&](float Old, float New) {
        bStaminaChangedCalled = true;
        LastOldStamina = Old;
        LastNewStamina = New;
        UE_LOG(LogTemp, Log, TEXT("[Stamina Callback] 변화: %.1f -> %.1f"), Old, New);
    };

    StaminaSystem.OnStaminaDepleted = [&]() {
        bStaminaDepletedCalled = true;
        UE_LOG(LogTemp, Warning, TEXT("[Stamina Callback] 고갈!"));
        PROFILE_BOOKMARK("StaminaDepleted");
    };

    // Test 1: 스태미나 소모 (스프린트 시뮬레이션)
    UE_LOG(LogTemp, Log, TEXT("\n[Test 1] 스프린트 시뮬레이션 (40 소모)"));
    StaminaSystem.UseStamina(40.0f);
    
    TestTrue(TEXT("스태미나 변화 콜백"), bStaminaChangedCalled);
    TestEqual(TEXT("소모 후 스태미나"), StaminaSystem.GetCurrentStamina(), 60.0f);
    TestEqual(TEXT("콜백 - 이전 값"), LastOldStamina, 100.0f);
    TestEqual(TEXT("콜백 - 새 값"), LastNewStamina, 60.0f);
    UE_LOG(LogTemp, Log, TEXT("[Test 1] 결과 - 스태미나: %.1f/%.1f"), 
        StaminaSystem.GetCurrentStamina(), StaminaSystem.GetMaxStamina());

    // Test 2: 스태미나 충분 여부
    UE_LOG(LogTemp, Log, TEXT("\n[Test 2] 스태미나 충분 여부 확인"));
    TestTrue(TEXT("30 사용 가능"), StaminaSystem.HasEnoughStamina(30.0f));
    TestFalse(TEXT("80 사용 불가"), StaminaSystem.HasEnoughStamina(80.0f));
    UE_LOG(LogTemp, Log, TEXT("[Test 2] 30 사용 가능: %s, 80 사용 가능: %s"),
        StaminaSystem.HasEnoughStamina(30.0f) ? TEXT("예") : TEXT("아니오"),
        StaminaSystem.HasEnoughStamina(80.0f) ? TEXT("예") : TEXT("아니오"));

    // Test 3: 스태미나 회복 (휴식 시뮬레이션)
    UE_LOG(LogTemp, Log, TEXT("\n[Test 3] 휴식 시뮬레이션 (2초 회복)"));
    bStaminaChangedCalled = false;
    StaminaSystem.RegenerateStamina(2.0f); // 2초 * 10/s = 20
    
    TestTrue(TEXT("회복 콜백 호출"), bStaminaChangedCalled);
    TestEqual(TEXT("회복 후 스태미나"), StaminaSystem.GetCurrentStamina(), 80.0f);
    UE_LOG(LogTemp, Log, TEXT("[Test 3] 결과 - 스태미나: %.1f/%.1f (+20)"), 
        StaminaSystem.GetCurrentStamina(), StaminaSystem.GetMaxStamina());

    // Test 4: 최대치 초과 회복 방지
    UE_LOG(LogTemp, Log, TEXT("\n[Test 4] 과다 회복 방지 (10초 회복 시도)"));
    StaminaSystem.RegenerateStamina(10.0f);
    TestEqual(TEXT("최대 초과 방지"), StaminaSystem.GetCurrentStamina(), 100.0f);
    UE_LOG(LogTemp, Log, TEXT("[Test 4] 결과 - 스태미나: %.1f/%.1f (최대치 유지)"), 
        StaminaSystem.GetCurrentStamina(), StaminaSystem.GetMaxStamina());

    // Test 5: 스태미나 고갈
    UE_LOG(LogTemp, Log, TEXT("\n[Test 5] 스태미나 고갈 테스트 (150 소모)"));
    StaminaSystem.UseStamina(150.0f);
    
    TestTrue(TEXT("고갈 콜백 호출"), bStaminaDepletedCalled);
    TestEqual(TEXT("최소 스태미나 (0)"), StaminaSystem.GetCurrentStamina(), 0.0f);
    UE_LOG(LogTemp, Warning, TEXT("[Test 5] 결과 - 스태미나 고갈! 현재: %.1f"), 
        StaminaSystem.GetCurrentStamina());

    UE_LOG(LogTemp, Warning, TEXT("========================================"));
    UE_LOG(LogTemp, Warning, TEXT("[W2 Unit Test] Stamina System 완료 ✓"));
    UE_LOG(LogTemp, Warning, TEXT("========================================\n"));

    return true;
}

// ==================== W2 NEW TEST 3: FOV 변경 유닛테스트 ====================
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FFOVChangeUnitTest,
    "InterfaceTEST.Unit.W2.FOVSystem",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter
)

bool FFOVChangeUnitTest::RunTest(const FString& Parameters)
{
    PROFILE_SCOPE_FOV();
    
    UE_LOG(LogTemp, Warning, TEXT("========================================"));
    UE_LOG(LogTemp, Warning, TEXT("[W2 Unit Test] FOV System 시작"));
    UE_LOG(LogTemp, Warning, TEXT("========================================"));

    UWorld* World = GEngine->GetWorldFromContextObject(GEngine->GameViewport, EGetWorldErrorMode::ReturnNull);
    
    if (!World)
    {
        AddWarning(TEXT("World not available"));
        return false;
    }

    // 테스트 캐릭터 생성
    FActorSpawnParameters SpawnParams;
    SpawnParams.Name = FName(TEXT("TestChar_FOV"));
    
    ACharacter* TestCharacter = World->SpawnActor<ACharacter>(
        ACharacter::StaticClass(), 
        FVector::ZeroVector, 
        FRotator::ZeroRotator, 
        SpawnParams
    );
    
    if (!TestCharacter)
    {
        AddError(TEXT("캐릭터 생성 실패"));
        return false;
    }

    // 카메라 생성
    UCameraComponent* TestCamera = NewObject<UCameraComponent>(TestCharacter);
    if (!TestCamera)
    {
        AddError(TEXT("카메라 생성 실패"));
        TestCharacter->Destroy();
        return false;
    }

    FUnrealCharacterController Controller(TestCharacter, TestCamera);

    // Test 1: 초기 FOV 확인
    UE_LOG(LogTemp, Log, TEXT("\n[Test 1] 초기 FOV 확인"));
    PROFILE_BOOKMARK("FOVTest_Initial");
    float InitialFOV = Controller.GetCameraFOV();
    TestTrue(TEXT("초기 FOV 유효"), InitialFOV > 0.0f);
    UE_LOG(LogTemp, Log, TEXT("[Test 1] 초기 FOV: %.1f"), InitialFOV);

    // Test 2: 일반 FOV 설정 (90도)
    UE_LOG(LogTemp, Log, TEXT("\n[Test 2] 일반 FOV 설정 (90도)"));
    PROFILE_BOOKMARK("FOVTest_Normal");
    float NormalFOV = 90.0f;
    Controller.SetCameraFOV(NormalFOV);
    float CurrentFOV = Controller.GetCameraFOV();
    
    TestTrue(TEXT("일반 FOV 적용"), FMath::IsNearlyEqual(CurrentFOV, NormalFOV, 0.1f));
    UE_LOG(LogTemp, Log, TEXT("[Test 2] 설정 FOV: %.1f, 현재 FOV: %.1f"), NormalFOV, CurrentFOV);

    // Test 3: 줌 FOV 설정 (45도)
    UE_LOG(LogTemp, Log, TEXT("\n[Test 3] 줌 FOV 설정 (45도)"));
    PROFILE_BOOKMARK("FOVTest_Zoom");
    float ZoomFOV = 45.0f;
    Controller.SetCameraFOV(ZoomFOV);
    CurrentFOV = Controller.GetCameraFOV();
    
    TestTrue(TEXT("줌 FOV 적용"), FMath::IsNearlyEqual(CurrentFOV, ZoomFOV, 0.1f));
    UE_LOG(LogTemp, Warning, TEXT("[Test 3] 줌 적용! 설정: %.1f, 현재: %.1f"), ZoomFOV, CurrentFOV);

    // Test 4: FOV 복귀 테스트
    UE_LOG(LogTemp, Log, TEXT("\n[Test 4] FOV 복귀 (90도)"));
    PROFILE_BOOKMARK("FOVTest_Restore");
    Controller.SetCameraFOV(NormalFOV);
    CurrentFOV = Controller.GetCameraFOV();
    
    TestTrue(TEXT("FOV 복귀"), FMath::IsNearlyEqual(CurrentFOV, NormalFOV, 0.1f));
    UE_LOG(LogTemp, Log, TEXT("[Test 4] 복귀 완료 - 현재 FOV: %.1f"), CurrentFOV);

    // Test 5: FOV 경계값 테스트
    UE_LOG(LogTemp, Log, TEXT("\n[Test 5] FOV 경계값 테스트"));
    
    // 최소 FOV (30도)
    float MinFOV = 30.0f;
    Controller.SetCameraFOV(MinFOV);
    float MinResult = Controller.GetCameraFOV();
    TestTrue(TEXT("최소 FOV 설정"), FMath::IsNearlyEqual(MinResult, MinFOV, 0.1f));
    UE_LOG(LogTemp, Log, TEXT("[Test 5-1] 최소 FOV: %.1f"), MinResult);
    
    // 최대 FOV (120도)
    float MaxFOV = 120.0f;
    Controller.SetCameraFOV(MaxFOV);
    float MaxResult = Controller.GetCameraFOV();
    TestTrue(TEXT("최대 FOV 설정"), FMath::IsNearlyEqual(MaxResult, MaxFOV, 0.1f));
    UE_LOG(LogTemp, Log, TEXT("[Test 5-2] 최대 FOV: %.1f"), MaxResult);

    // 정리
    TestCharacter->Destroy();

    UE_LOG(LogTemp, Warning, TEXT("========================================"));
    UE_LOG(LogTemp, Warning, TEXT("[W2 Unit Test] FOV System 완료 ✓"));
    UE_LOG(LogTemp, Warning, TEXT("========================================\n"));

    return true;
}

// ==================== W2 INTEGRATION: 통합 시나리오 테스트 ====================
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FW2IntegratedScenarioTest,
    "InterfaceTEST.Integration.W2.CompleteScenario",
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter
)

bool FW2IntegratedScenarioTest::RunTest(const FString& Parameters)
{
    UE_LOG(LogTemp, Warning, TEXT("========================================"));
    UE_LOG(LogTemp, Warning, TEXT("[W2 Integration] 통합 시나리오 테스트"));
    UE_LOG(LogTemp, Warning, TEXT("시나리오: 전투 → 도망(스프린트) → 회복"));
    UE_LOG(LogTemp, Warning, TEXT("========================================"));

    // 시스템 생성
    CPPCore::HealthSystem Health(100);
    CPPCore::StaminaSystem Stamina(100.0f);
    Stamina.SetRegenRate(10.0f);

    // 전투 - 데미지 받음
    UE_LOG(LogTemp, Log, TEXT("\n=== 시나리오 1: 전투 시작 ==="));
    PROFILE_BOOKMARK("Scenario_CombatStart");
    
    Health.TakeDamage(40.0f);
    TestEqual(TEXT("전투 후 체력"), Health.GetCurrentHealth(), 60);
    UE_LOG(LogTemp, Warning, TEXT("적에게 공격받음! 데미지: -40, 남은 체력: %d/100"), Health.GetCurrentHealth());

    // 도망 - 스프린트 (3초)
    UE_LOG(LogTemp, Log, TEXT("\n=== 시나리오 2: 스프린트로 도망 ==="));
    PROFILE_BOOKMARK("Scenario_Sprint");
    
    for (int i = 1; i <= 3; ++i)
    {
        Stamina.UseStamina(15.0f); // 초당 15 소모
        UE_LOG(LogTemp, Log, TEXT("스프린트 %d초: 스태미나 %.1f/100.0"), i, Stamina.GetCurrentStamina());
    }
    
    TestEqual(TEXT("스프린트 후 스태미나"), Stamina.GetCurrentStamina(), 55.0f);
    UE_LOG(LogTemp, Warning, TEXT("안전 지대 도착! 남은 스태미나: %.1f/100.0"), Stamina.GetCurrentStamina());

    // 안전 지대 - 스태미나 회복
    UE_LOG(LogTemp, Log, TEXT("\n=== 시나리오 3: 안전 지대에서 회복 ==="));
    PROFILE_BOOKMARK("Scenario_StaminaRegen");
    
    UE_LOG(LogTemp, Log, TEXT("휴식 중..."));
    for (int i = 1; i <= 5; ++i)
    {
        Stamina.RegenerateStamina(1.0f); // 1초씩
        if (i % 2 == 0) // 2초마다 로그
        {
            UE_LOG(LogTemp, Log, TEXT("회복 %d초: 스태미나 %.1f/100.0"), i, Stamina.GetCurrentStamina());
        }
    }
    
    TestEqual(TEXT("회복 완료"), Stamina.GetCurrentStamina(), 100.0f);
    UE_LOG(LogTemp, Warning, TEXT("스태미나 완전 회복! 현재: %.1f/100.0"), Stamina.GetCurrentStamina());

    // 힐링 아이템 발견
    UE_LOG(LogTemp, Log, TEXT("\n=== 시나리오 4: 힐링 아이템 사용 ==="));
    PROFILE_BOOKMARK("Scenario_Healing");
    
    Health.Heal(30);
    UE_LOG(LogTemp, Log, TEXT("힐링 포션 사용! +30 체력"));
    UE_LOG(LogTemp, Warning, TEXT("현재 체력: %d/100"), Health.GetCurrentHealth());
    
    Health.Heal(20);
    UE_LOG(LogTemp, Log, TEXT("체력 회복 완료!"));
    
    TestEqual(TEXT("최종 체력"), Health.GetCurrentHealth(), 100);
    TestEqual(TEXT("최종 스태미나"), Stamina.GetCurrentStamina(), 100.0f);

    UE_LOG(LogTemp, Warning, TEXT("\n========================================"));
    UE_LOG(LogTemp, Warning, TEXT("[W2 Integration] 시나리오 완료 ✓"));
    UE_LOG(LogTemp, Warning, TEXT("최종 상태 - 체력: %d/100, 스태미나: %.1f/100.0"), 
        Health.GetCurrentHealth(), 
        Stamina.GetCurrentStamina());
    UE_LOG(LogTemp, Warning, TEXT("========================================\n"));

    return true;
}