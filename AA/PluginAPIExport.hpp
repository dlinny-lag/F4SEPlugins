#pragma once

#include "Change.hpp"
#include "f4se/PapyrusVM.h"
#include "f4se/PapyrusNativeFunctions.h"


namespace PluginAPIExport
{
	const char* EXPORT_PAPYRUS_SCRIPT = "AA:Plugin";

	const char* pluginName = AA_PluginName;
	const UInt32 pluginUID = 'AA';
	const UInt32 pluginVersionInt = 0x0101;
	const char* pluginVersionString = "1.0.1";

	BSFixedString GetVersionString(StaticFunctionTag* _)
	{
		return pluginVersionString;
	};

	UInt32 GetVersionInt(StaticFunctionTag* _)
	{
		return pluginVersionInt;
	};

	bool Register(VirtualMachine* vm)
	{
		vm->RegisterFunction(
			new NativeFunction0<StaticFunctionTag, BSFixedString>("GetVersionString", EXPORT_PAPYRUS_SCRIPT, GetVersionString, vm));
		vm->SetFunctionFlags(EXPORT_PAPYRUS_SCRIPT, "GetVersionString", IFunction::kFunctionFlag_NoWait);

		vm->RegisterFunction(
			new NativeFunction0<StaticFunctionTag, UInt32>("GetVersionInt", EXPORT_PAPYRUS_SCRIPT, GetVersionInt, vm));
		vm->SetFunctionFlags(EXPORT_PAPYRUS_SCRIPT, "GetVersionInt", IFunction::kFunctionFlag_NoWait);
		
		return true;
	}
}