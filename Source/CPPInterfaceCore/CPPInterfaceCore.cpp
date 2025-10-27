
#include "CPPInterfaceCore.h" //가장 먼저 초기화.
#include "Modules/ModuleManager.h" // FDefaultGameModuleImpl 사용하기 위해 필요함.

// 모듈 클래스 정의 -> FDefaultGameModuleImpl 상속받음.
class FCPPInterfaceCoreModule : public FDefaultGameModuleImpl
{
public:
	/*
	 * 모듈 로드 직후 호출 호출됨.
	 * 순수 C++만 사용하더라도 해당 함수가 없으면 로드에 실패했음.
	 */
	virtual void StartupModule() override
	{
		// 순수 C++ 코어 모듈이므로 여기에 복잡한 UE 초기화 코드는 없음.
		// 로그메시지 정도만.
	}
	
	virtual void ShutdownModule() override
	{
	}


	 // 모듈이 상주하고 있음. 순수 C++ 모듈이더라도 true를 반환해야함.
	virtual bool IsGameModule() const override
	{
		return true; 
	}
};


// 모듈 등록 매크로 -> 모듈을 엔진에 등록.
IMPLEMENT_MODULE(FCPPInterfaceCoreModule, CPPInterfaceCore);