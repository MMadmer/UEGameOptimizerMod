#include "UFunctionUtils.hpp"

#include <Mod/CppUserModBase.hpp>
#include <Unreal/UObject.hpp>
#include <DynamicOutput/DynamicOutput.hpp>

using namespace ::RC::Unreal;

bool UFunctionUtils::TryCallUFunction(UObject* Caller, const TCHAR* FuncName, void* Args)
{
    UFunction* Func = TryGetUFunction(Caller, FuncName);
    if (!Func) return false;

    Caller->ProcessEvent(Func, Args);
    return true;
}

UFunction* UFunctionUtils::TryGetUFunction(UObject* Caller, const TCHAR* FuncName)
{
    if (!Caller)
    {
        Output::send<LogLevel::Warning>(STR("[Optimizer] {} UFunction caller not found!\n"), FuncName);
        return nullptr;
    }

    UFunction* Func = Caller->GetFunctionByNameInChain(FuncName);
    if (!Func) Output::send<LogLevel::Warning>(STR("[Optimizer] {} UFunction not found!\n"), FuncName);

    return Func;
}
