#pragma once

#define PapyrusPrefix "DS:"
#define AS_IS(val) val
#define AS_STR(val) #val

#define REGISTER_FUNC(vm, ScriptName, Func, ArgNo) \
		vm->RegisterFunction(new NativeFunction##ArgNo(AS_STR(Func), ScriptName, VM##Func, vm)); \
		vm->SetFunctionFlags(ScriptName, AS_STR(Func), IFunction::kFunctionFlag_NoWait)

namespace PapyrusExport
{

};