#pragma once

#include "CoreMinimal.h"

// Unreal Insights가 활성화된 경우에만 프로파일링 활성화
#if CPUPROFILERTRACE_ENABLED

#include "ProfilingDebugging/CpuProfilerTrace.h"

// ==================== Stat Groups ====================
DECLARE_STATS_GROUP(TEXT("PlayerCharacter"), STATGROUP_PlayerCharacter, STATCAT_Advanced);
DECLARE_STATS_GROUP(TEXT("PostProcess"), STATGROUP_PostProcess, STATCAT_Advanced);
DECLARE_STATS_GROUP(TEXT("W2_Performance"), STATGROUP_W2Performance, STATCAT_Advanced);
DECLARE_STATS_GROUP(TEXT("Health"), STATGROUP_Health, STATCAT_Advanced);  // Health 전용 그룹

// ==================== Cycle Counters (기존) ====================
DECLARE_CYCLE_STAT(TEXT("Character Tick Total"), STAT_CharacterTick, STATGROUP_PlayerCharacter);
DECLARE_CYCLE_STAT(TEXT("Core Update"), STAT_CoreUpdate, STATGROUP_PlayerCharacter);
DECLARE_CYCLE_STAT(TEXT("Movement Processing"), STAT_MovementProcessing, STATGROUP_PlayerCharacter);

DECLARE_CYCLE_STAT(TEXT("Ground Check"), STAT_GroundCheck, STATGROUP_PlayerCharacter);
DECLARE_CYCLE_STAT(TEXT("Jump Logic"), STAT_JumpLogic, STATGROUP_PlayerCharacter);

DECLARE_CYCLE_STAT(TEXT("Health Check"), STAT_HealthCheck, STATGROUP_PlayerCharacter);
DECLARE_CYCLE_STAT(TEXT("Attack Processing"), STAT_AttackProcessing, STATGROUP_PlayerCharacter);

DECLARE_CYCLE_STAT(TEXT("FOV Interpolation"), STAT_FOVInterp, STATGROUP_PlayerCharacter);
DECLARE_CYCLE_STAT(TEXT("PostProcess Material"), STAT_PostProcessMaterial, STATGROUP_PostProcess);

// ==================== Health 전용 Stats ====================
DECLARE_CYCLE_STAT(TEXT("Damage Processing"), STAT_DamageProcessing, STATGROUP_Health);
DECLARE_CYCLE_STAT(TEXT("Heal Processing"), STAT_HealProcessing, STATGROUP_Health);
DECLARE_CYCLE_STAT(TEXT("Death Processing"), STAT_DeathProcessing, STATGROUP_Health);

// Health 카운터
DECLARE_DWORD_COUNTER_STAT(TEXT("Total Damage Taken"), STAT_TotalDamageTaken, STATGROUP_Health);
DECLARE_DWORD_COUNTER_STAT(TEXT("Total Healing Received"), STAT_TotalHealingReceived, STATGROUP_Health);
DECLARE_DWORD_COUNTER_STAT(TEXT("Current Health"), STAT_CurrentHealth, STATGROUP_Health);
DECLARE_DWORD_COUNTER_STAT(TEXT("Death Count"), STAT_DeathCount, STATGROUP_Health);

// ==================== W2 추가: 단계별 Cycle Counters ====================
DECLARE_CYCLE_STAT(TEXT("W2 Tick Phase"), STAT_W2_TickPhase, STATGROUP_W2Performance);
DECLARE_CYCLE_STAT(TEXT("W2 Physics Phase"), STAT_W2_PhysicsPhase, STATGROUP_W2Performance);
DECLARE_CYCLE_STAT(TEXT("W2 Combat Phase"), STAT_W2_CombatPhase, STATGROUP_W2Performance);
DECLARE_CYCLE_STAT(TEXT("W2 PostProcess Phase"), STAT_W2_PostProcessPhase, STATGROUP_W2Performance);

// ==================== Scoped Profiler Macros (기존) ====================
#define PROFILE_SCOPE_CHARACTER_TICK() \
    SCOPE_CYCLE_COUNTER(STAT_CharacterTick); \
    TRACE_CPUPROFILER_EVENT_SCOPE(CharacterTick)

#define PROFILE_SCOPE_CORE_UPDATE() \
    SCOPE_CYCLE_COUNTER(STAT_CoreUpdate); \
    TRACE_CPUPROFILER_EVENT_SCOPE(CoreUpdate)

#define PROFILE_SCOPE_MOVEMENT() \
    SCOPE_CYCLE_COUNTER(STAT_MovementProcessing); \
    TRACE_CPUPROFILER_EVENT_SCOPE(MovementProcessing)

#define PROFILE_SCOPE_GROUND_CHECK() \
    SCOPE_CYCLE_COUNTER(STAT_GroundCheck); \
    TRACE_CPUPROFILER_EVENT_SCOPE(GroundCheck)

#define PROFILE_SCOPE_JUMP() \
    SCOPE_CYCLE_COUNTER(STAT_JumpLogic); \
    TRACE_CPUPROFILER_EVENT_SCOPE(JumpLogic)

#define PROFILE_SCOPE_HEALTH() \
    SCOPE_CYCLE_COUNTER(STAT_HealthCheck); \
    TRACE_CPUPROFILER_EVENT_SCOPE(HealthCheck)

#define PROFILE_SCOPE_ATTACK() \
    SCOPE_CYCLE_COUNTER(STAT_AttackProcessing); \
    TRACE_CPUPROFILER_EVENT_SCOPE(AttackProcessing)

#define PROFILE_SCOPE_FOV() \
    SCOPE_CYCLE_COUNTER(STAT_FOVInterp); \
    TRACE_CPUPROFILER_EVENT_SCOPE(FOVInterpolation)

#define PROFILE_SCOPE_POSTPROCESS() \
    SCOPE_CYCLE_COUNTER(STAT_PostProcessMaterial); \
    TRACE_CPUPROFILER_EVENT_SCOPE(PostProcessMaterial)

// ==================== Health 전용 Profiler Macros ====================
#define PROFILE_SCOPE_DAMAGE() \
    SCOPE_CYCLE_COUNTER(STAT_DamageProcessing); \
    TRACE_CPUPROFILER_EVENT_SCOPE(DamageProcessing)

#define PROFILE_SCOPE_HEAL() \
    SCOPE_CYCLE_COUNTER(STAT_HealProcessing); \
    TRACE_CPUPROFILER_EVENT_SCOPE(HealProcessing)

#define PROFILE_SCOPE_DEATH() \
    SCOPE_CYCLE_COUNTER(STAT_DeathProcessing); \
    TRACE_CPUPROFILER_EVENT_SCOPE(DeathProcessing)

// Health 카운터 업데이트
#define PROFILE_UPDATE_DAMAGE(Amount) \
    INC_DWORD_STAT_BY(STAT_TotalDamageTaken, (uint32)Amount)

#define PROFILE_UPDATE_HEALING(Amount) \
    INC_DWORD_STAT_BY(STAT_TotalHealingReceived, (uint32)Amount)

#define PROFILE_UPDATE_CURRENT_HEALTH(Health) \
    SET_DWORD_STAT(STAT_CurrentHealth, (uint32)Health)

#define PROFILE_UPDATE_DEATH_COUNT() \
    INC_DWORD_STAT(STAT_DeathCount)

// ==================== W2 추가: 단계별 Profiler Macros ====================
// Tick → Physics → Combat → PostProcess 단계별 추적

#define PROFILE_SCOPE_W2_TICK() \
    SCOPE_CYCLE_COUNTER(STAT_W2_TickPhase); \
    TRACE_CPUPROFILER_EVENT_SCOPE(W2_TickPhase)

#define PROFILE_SCOPE_W2_PHYSICS() \
    SCOPE_CYCLE_COUNTER(STAT_W2_PhysicsPhase); \
    TRACE_CPUPROFILER_EVENT_SCOPE(W2_PhysicsPhase)

#define PROFILE_SCOPE_W2_COMBAT() \
    SCOPE_CYCLE_COUNTER(STAT_W2_CombatPhase); \
    TRACE_CPUPROFILER_EVENT_SCOPE(W2_CombatPhase)

#define PROFILE_SCOPE_W2_POSTPROCESS() \
    SCOPE_CYCLE_COUNTER(STAT_W2_PostProcessPhase); \
    TRACE_CPUPROFILER_EVENT_SCOPE(W2_PostProcessPhase)

// ==================== Bookmark Macros (기존) ====================
#define PROFILE_BOOKMARK(Name) TRACE_BOOKMARK(TEXT(Name))

// ==================== W2 추가: 단계별 Bookmark Macros ====================
// 각 단계의 시작과 끝을 명확히 표시

#define PROFILE_W2_BOOKMARK_TICK_START() \
    TRACE_BOOKMARK(TEXT("W2_Tick_Start")); \
    UE_LOG(LogTemp, Verbose, TEXT("[W2 Profile] Tick 단계 시작"))

#define PROFILE_W2_BOOKMARK_TICK_END() \
    TRACE_BOOKMARK(TEXT("W2_Tick_End")); \
    UE_LOG(LogTemp, Verbose, TEXT("[W2 Profile] Tick 단계 종료"))

#define PROFILE_W2_BOOKMARK_PHYSICS_START() \
    TRACE_BOOKMARK(TEXT("W2_Physics_Start")); \
    UE_LOG(LogTemp, Verbose, TEXT("[W2 Profile] Physics 단계 시작"))

#define PROFILE_W2_BOOKMARK_PHYSICS_END() \
    TRACE_BOOKMARK(TEXT("W2_Physics_End")); \
    UE_LOG(LogTemp, Verbose, TEXT("[W2 Profile] Physics 단계 종료"))

#define PROFILE_W2_BOOKMARK_COMBAT_START() \
    TRACE_BOOKMARK(TEXT("W2_Combat_Start")); \
    UE_LOG(LogTemp, Verbose, TEXT("[W2 Profile] Combat 단계 시작"))

#define PROFILE_W2_BOOKMARK_COMBAT_END() \
    TRACE_BOOKMARK(TEXT("W2_Combat_End")); \
    UE_LOG(LogTemp, Verbose, TEXT("[W2 Profile] Combat 단계 종료"))

#define PROFILE_W2_BOOKMARK_POSTPROCESS_START() \
    TRACE_BOOKMARK(TEXT("W2_PostProcess_Start")); \
    UE_LOG(LogTemp, Verbose, TEXT("[W2 Profile] PostProcess 단계 시작"))

#define PROFILE_W2_BOOKMARK_POSTPROCESS_END() \
    TRACE_BOOKMARK(TEXT("W2_PostProcess_End")); \
    UE_LOG(LogTemp, Verbose, TEXT("[W2 Profile] PostProcess 단계 종료"))

// ==================== W2 추가: 성능 측정 헬퍼 ====================
// ms 단위 측정을 위한 타이머 헬퍼

#define PROFILE_W2_TIMER_START(TimerName) \
    double TimerName##_StartTime = FPlatformTime::Seconds()

#define PROFILE_W2_TIMER_END(TimerName, PhaseName) \
    do { \
        double TimerName##_EndTime = FPlatformTime::Seconds(); \
        double TimerName##_Duration = (TimerName##_EndTime - TimerName##_StartTime) * 1000.0; \
        UE_LOG(LogTemp, Log, TEXT("[W2 Performance] %s: %.3f ms"), TEXT(PhaseName), TimerName##_Duration); \
    } while(0)

// ==================== W2 추가: 이벤트 특화 북마크 ====================
// 특정 이벤트 발생 시 명확한 마커

#define PROFILE_W2_EVENT_DAMAGE_TAKEN() \
    TRACE_BOOKMARK(TEXT("W2_Event_DamageTaken")); \
    UE_LOG(LogTemp, Verbose, TEXT("[W2 Event] 데미지 받음"))

#define PROFILE_W2_EVENT_HEALTH_CHANGED(Old, New) \
    TRACE_BOOKMARK(TEXT("W2_Event_HealthChanged")); \
    UE_LOG(LogTemp, Verbose, TEXT("[W2 Event] 체력 변화: %d -> %d"), Old, New)

#define PROFILE_W2_EVENT_STAMINA_DEPLETED() \
    TRACE_BOOKMARK(TEXT("W2_Event_StaminaDepleted")); \
    UE_LOG(LogTemp, Verbose, TEXT("[W2 Event] 스태미나 고갈"))

#define PROFILE_W2_EVENT_SPRINT_START() \
    TRACE_BOOKMARK(TEXT("W2_Event_SprintStart")); \
    UE_LOG(LogTemp, Verbose, TEXT("[W2 Event] 스프린트 시작"))

#define PROFILE_W2_EVENT_SPRINT_END() \
    TRACE_BOOKMARK(TEXT("W2_Event_SprintEnd")); \
    UE_LOG(LogTemp, Verbose, TEXT("[W2 Event] 스프린트 종료"))

#define PROFILE_W2_EVENT_ZOOM_CHANGED(bZooming) \
    TRACE_BOOKMARK(TEXT("W2_Event_ZoomChanged")); \
    UE_LOG(LogTemp, Verbose, TEXT("[W2 Event] 줌 상태 변경: %s"), bZooming ? TEXT("ON") : TEXT("OFF"))

#define PROFILE_W2_EVENT_JUMP() \
    TRACE_BOOKMARK(TEXT("W2_Event_Jump")); \
    UE_LOG(LogTemp, Verbose, TEXT("[W2 Event] 점프"))

#define PROFILE_W2_EVENT_ATTACK() \
    TRACE_BOOKMARK(TEXT("W2_Event_Attack")); \
    UE_LOG(LogTemp, Verbose, TEXT("[W2 Event] 공격"))

#else

// 프로파일링 비활성화 시 빈 매크로
#define PROFILE_SCOPE_CHARACTER_TICK()
#define PROFILE_SCOPE_CORE_UPDATE()
#define PROFILE_SCOPE_MOVEMENT()
#define PROFILE_SCOPE_GROUND_CHECK()
#define PROFILE_SCOPE_JUMP()
#define PROFILE_SCOPE_HEALTH()
#define PROFILE_SCOPE_ATTACK()
#define PROFILE_SCOPE_FOV()
#define PROFILE_SCOPE_POSTPROCESS()
#define PROFILE_BOOKMARK(Name)

// Health 매크로 비활성화
#define PROFILE_SCOPE_DAMAGE()
#define PROFILE_SCOPE_HEAL()
#define PROFILE_SCOPE_DEATH()
#define PROFILE_UPDATE_DAMAGE(Amount)
#define PROFILE_UPDATE_HEALING(Amount)
#define PROFILE_UPDATE_CURRENT_HEALTH(Health)
#define PROFILE_UPDATE_DEATH_COUNT()

// W2 매크로들도 비활성화
#define PROFILE_SCOPE_W2_TICK()
#define PROFILE_SCOPE_W2_PHYSICS()
#define PROFILE_SCOPE_W2_COMBAT()
#define PROFILE_SCOPE_W2_POSTPROCESS()

#define PROFILE_W2_BOOKMARK_TICK_START()
#define PROFILE_W2_BOOKMARK_TICK_END()
#define PROFILE_W2_BOOKMARK_PHYSICS_START()
#define PROFILE_W2_BOOKMARK_PHYSICS_END()
#define PROFILE_W2_BOOKMARK_COMBAT_START()
#define PROFILE_W2_BOOKMARK_COMBAT_END()
#define PROFILE_W2_BOOKMARK_POSTPROCESS_START()
#define PROFILE_W2_BOOKMARK_POSTPROCESS_END()

#define PROFILE_W2_TIMER_START(TimerName)
#define PROFILE_W2_TIMER_END(TimerName, PhaseName)

#define PROFILE_W2_EVENT_DAMAGE_TAKEN()
#define PROFILE_W2_EVENT_HEALTH_CHANGED(Old, New)
#define PROFILE_W2_EVENT_STAMINA_DEPLETED()
#define PROFILE_W2_EVENT_SPRINT_START()
#define PROFILE_W2_EVENT_SPRINT_END()
#define PROFILE_W2_EVENT_ZOOM_CHANGED(bZooming)
#define PROFILE_W2_EVENT_JUMP()
#define PROFILE_W2_EVENT_ATTACK()

#endif