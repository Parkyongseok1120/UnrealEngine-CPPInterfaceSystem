#include "Tests/CoreSystemTests.h"
#include "Stamina/StaminaSystem.h"


// ==================== TEST 1: Stamina System ====================
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FStaminaSystemTest,
    "CPPCore.Systems.StaminaSystem",
    EAutomationTestFlags::CommandletContext | EAutomationTestFlags::ProductFilter
)

bool FStaminaSystemTest::RunTest(const FString& Parameters)
{
    // === Setup ===
    StaminaSystem Stamina(100.0f);
    Stamina.SetRegenRate(10.0f);

    bool bDepletedCalled = false;
    bool bChangedCalled = false;
    float LastOldValue = 0.0f;
    float LastNewValue = 0.0f;

    Stamina.OnStaminaDepleted = [&bDepletedCalled]() { bDepletedCalled = true; };
    Stamina.OnStaminaChanged = [&](float Old, float New) {
        bChangedCalled = true;
        LastOldValue = Old;
        LastNewValue = New;
    };

    // === Test 1: 초기값 확인 ===
    TestEqual(TEXT("Initial Stamina"), Stamina.GetCurrentStamina(), 100.0f);
    TestTrue(TEXT("Has Enough Stamina (50)"), Stamina.HasEnoughStamina(50.0f));

    // === Test 2: 스태미나 사용 ===
    Stamina.UseStamina(30.0f);
    TestEqual(TEXT("After Use (30)"), Stamina.GetCurrentStamina(), 70.0f);
    TestTrue(TEXT("Changed Event Fired"), bChangedCalled);
    TestEqual(TEXT("Old Value"), LastOldValue, 100.0f);
    TestEqual(TEXT("New Value"), LastNewValue, 70.0f);

    // === Test 3: 스태미나 고갈 ===
    bChangedCalled = false;
    Stamina.UseStamina(80.0f);
    TestEqual(TEXT("Depleted (Clamped to 0)"), Stamina.GetCurrentStamina(), 0.0f);
    TestTrue(TEXT("Depleted Event Fired"), bDepletedCalled);

    // === Test 4: 스태미나 회복 ===
    bChangedCalled = false;
    Stamina.RegenerateStamina(0.5f); // 10 * 0.5s = 5.0 회복
    TestEqual(TEXT("After Regen (0.5s)"), Stamina.GetCurrentStamina(), 5.0f);
    TestTrue(TEXT("Changed Event on Regen"), bChangedCalled);

    // === Test 5: 최대치 초과 방지 ===
    for (int i = 0; i < 20; ++i)
    {
        Stamina.RegenerateStamina(1.0f); // 10 * 1s = 10.0씩 회복
    }
    TestEqual(TEXT("Max Stamina Clamped"), Stamina.GetCurrentStamina(), 100.0f);

    // === Test 6: RegenRate 확인 ===
    CPPCore::StaminaSystem TestStamina(50.0f);
    TestStamina.UseStamina(50.0f); // 스테미나를 0으로 만들고
    TestStamina.RegenerateStamina(1.0f); // 다시 10 정도로 회복
    TestEqual(TEXT("RegenRate Default (10.0f)"), TestStamina.GetCurrentStamina(), 10.0f);

    return true;
}

// ==================== TEST 2: Jump System ====================
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FJumpSystemTest,
    "CPPCore.Systems.JumpSystem",
    EAutomationTestFlags::CommandletContext | EAutomationTestFlags::ProductFilter
)

bool FJumpSystemTest::RunTest(const FString& Parameters)
{
    // === Setup ===
    FMockCharacterController MockController;
    MockController.bIsGrounded = true;

    CPPCore::CharacterStatsConfig Config;
    Config.MaxJumpCount = 2;

    CPPCore::PlayerCharacterCore Core(&MockController, Config);

    int32 JumpEventCount = 0;
    Core.OnJumped = [&JumpEventCount]() { JumpEventCount++; };

    // === Test 1: 지상에서 점프 가능 ===
    TestTrue(TEXT("Can Jump on Ground"), Core.CanJump());

    Core.HandleJumpInput();
    TestEqual(TEXT("Jump Called Once"), MockController.JumpCallCount, 1);
    TestEqual(TEXT("Jump Event Fired Once"), JumpEventCount, 1);

    // === Test 2: 공중에서 더블 점프 ===
    MockController.bIsGrounded = false;
    Core.Update(0.016f);

    TestTrue(TEXT("Can Double Jump"), Core.CanJump());

    Core.HandleJumpInput();
    TestEqual(TEXT("Jump Called Twice"), MockController.JumpCallCount, 2);

    // === Test 3: 최대 점프 횟수 도달 ===
    Core.Update(0.016f);

    TestFalse(TEXT("Cannot Triple Jump"), Core.CanJump());

    Core.HandleJumpInput(); // 호출되지 않아야 함
    TestEqual(TEXT("Jump Still Twice"), MockController.JumpCallCount, 2);

    // === Test 4: 착지 후 리셋 ===
    MockController.bIsGrounded = true;
    Core.Update(0.016f);

    TestTrue(TEXT("Can Jump After Landing"), Core.CanJump());

    return true;
}

// ==================== TEST 3: FOV Zoom System ====================
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FFOVZoomSystemTest,
    "CPPCore.Systems.FOVZoomSystem",
    EAutomationTestFlags::CommandletContext | EAutomationTestFlags::ProductFilter
)

bool FFOVZoomSystemTest::RunTest(const FString& Parameters)
{
    // === Setup ===
    FMockCharacterController MockController;
    MockController.CameraFOV = 90.0f;

    CPPCore::CharacterStatsConfig Config;
    Config.DefaultFOV = 90.0f;
    Config.ZoomedFOV = 45.0f;
    Config.ZoomInterpSpeed = 20.0f;

    CPPCore::PlayerCharacterCore Core(&MockController, Config);

    bool bZoomStateChanged = false;
    bool bLastZoomState = false;
    Core.OnZoomStateChanged = [&](bool bZooming) {
        bZoomStateChanged = true;
        bLastZoomState = bZooming;
    };

    // === Test 1: 초기 FOV ===
    TestEqual(TEXT("Initial FOV"), MockController.CameraFOV, 90.0f);
    TestFalse(TEXT("Not Zooming Initially"), Core.IsZooming());

    // === Test 2: 줌 시작 ===
    Core.HandleZoomStart();
    TestTrue(TEXT("Zoom State Changed"), bZoomStateChanged);
    TestTrue(TEXT("Is Zooming"), bLastZoomState);

    // === Test 3: FOV 보간 (여러 프레임) ===
    float PrevFOV = MockController.CameraFOV;
    for (int i = 0; i < 10; ++i)
    {
        Core.Update(0.016f); // 60 FPS
        TestTrue(TEXT("FOV Decreasing"), MockController.CameraFOV < PrevFOV || FMath::IsNearlyEqual(MockController.CameraFOV, 45.0f, 0.1f));
        PrevFOV = MockController.CameraFOV;
    }

    // 충분한 시간 후 목표 FOV 도달
    for (int i = 0; i < 50; ++i)
    {
        Core.Update(0.016f);
    }
    TestTrue(TEXT("FOV Reached Target"), FMath::IsNearlyEqual(MockController.CameraFOV, 45.0f, 1.0f));

    // === Test 4: 줌 해제 ===
    bZoomStateChanged = false;
    Core.HandleZoomEnd();
    TestTrue(TEXT("Zoom State Changed on End"), bZoomStateChanged);
    TestFalse(TEXT("Not Zooming After End"), bLastZoomState);

    // FOV 복귀 확인
    for (int i = 0; i < 50; ++i)
    {
        Core.Update(0.016f);
    }
    TestTrue(TEXT("FOV Returned to Default"), FMath::IsNearlyEqual(MockController.CameraFOV, 90.0f, 1.0f));

    // === Test 5: 달리기 중 줌 불가 ===
    Core.HandleSprintStart();
    Core.HandleZoomStart(); // 달리기 중단되고 줌 시작
    TestFalse(TEXT("Cannot Sprint While Zooming"), Core.IsSprinting());

    return true;
}