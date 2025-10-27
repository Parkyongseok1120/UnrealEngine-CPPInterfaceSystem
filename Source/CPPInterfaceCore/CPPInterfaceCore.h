#pragma once

// CPPINTERFACECORE_API 매크로 정의
#if defined CPPINTERFACECORE_API
	#undef CPPINTERFACECORE_API
#endif

#ifdef CPPINTERFACECORE_EXPORTS 
	#define CPPINTERFACECORE_API __declspec(dllexport)
#else
	#define CPPINTERFACECORE_API __declspec(dllimport)
#endif