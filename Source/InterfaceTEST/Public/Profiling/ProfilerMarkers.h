#pragma once

#include "CoreMinimal.h"

// Unreal Insights가 활성화된 경우에만 프로파일링 활성화
#if CPUPROFILERTRACE_ENABLED

#include "ProfilingDebugging/CpuProfilerTrace.h"

// ==================== Stat Groups ====================
DECLARE_STATS_GROUP(TEXT("PlayerCharacter"), STATGROUP_PlayerCharacter, STATCAT_Advanced);
DECLARE_STATS_GROUP(TEXT("PostProcess"), STATGROUP_PostProcess, STATCAT_Advanced);

// ==================== Cycle Counters ====================
DECLARE_CYCLE_STAT(TEXT("Character Tick Total"), STAT_CharacterTick, STATGROUP_PlayerCharacter);
DECLARE_CYCLE_STAT(TEXT("Core Update"), STAT_CoreUpdate, STATGROUP_PlayerCharacter);
DECLARE_CYCLE_STAT(TEXT("Movement Processing"), STAT_MovementProcessing, STATGROUP_PlayerCharacter);

DECLARE_CYCLE_STAT(TEXT("Ground Check"), STAT_GroundCheck, STATGROUP_PlayerCharacter);
DECLARE_CYCLE_STAT(TEXT("Jump Logic"), STAT_JumpLogic, STATGROUP_PlayerCharacter);

DECLARE_CYCLE_STAT(TEXT("Health Check"), STAT_HealthCheck, STATGROUP_PlayerCharacter);
DECLARE_CYCLE_STAT(TEXT("Attack Processing"), STAT_AttackProcessing, STATGROUP_PlayerCharacter);

DECLARE_CYCLE_STAT(TEXT("FOV Interpolation"), STAT_FOVInterp, STATGROUP_PlayerCharacter);
DECLARE_CYCLE_STAT(TEXT("PostProcess Material"), STAT_PostProcessMaterial, STATGROUP_PostProcess);

// ==================== Scoped Profiler Macros ====================
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

// ==================== Bookmark Macros ====================
#define PROFILE_BOOKMARK(Name) TRACE_BOOKMARK(TEXT(Name))

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

#endif	