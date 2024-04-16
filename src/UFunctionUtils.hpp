#pragma once

#include "UnrealDef.hpp"

class UFunctionUtils : RC::UObject
{
public:
    /**
     *  @param Caller UFunction owner
     *  @param FuncName UFunction name
     *  @param Args UFunction args. Structure only
     */
    static bool TryCallUFunction(UObject* Caller, const TCHAR* FuncName, void* Args);

    /**
     *  @param Caller UFunction owner
     *  @param FuncName UFunction name
     */
    static RC::UFunction* TryGetUFunction(UObject* Caller, const TCHAR* FuncName);
};
