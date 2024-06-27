#pragma once

#include "CollectionTypeId.h"
#include "f4se/PapyrusVM.h"
#include "f4se/PapyrusNativeFunctions.h"
#include "f4se/PapyrusEvents.h"
#include "PapyrusExports.h"

namespace PluginAPIExport
{
	const char* EXPORT_PAPYRUS_SCRIPT = "DS:Plugin";

	const char* pluginName = DS_PluginName;
	const UInt32 pluginUID = 'DS'; // 17491
	const UInt32 pluginVersionInt = 0x0094;
	const char* pluginVersionString = "0.9.4";

	BSFixedString VMGetVersionString(StaticFunctionTag* _)
	{
		//Console_Print("DS version: %s", pluginVersionString);
		return pluginVersionString;
	};

	UInt32 VMGetVersionInt(StaticFunctionTag* _)
	{
		//Console_Print("DS version: %d", pluginVersionInt);
		return pluginVersionInt;
	};

	bool Register(VirtualMachine* vm)
	{
		REGISTER_FUNC(vm, EXPORT_PAPYRUS_SCRIPT, GetVersionString, 0);
		REGISTER_FUNC(vm, EXPORT_PAPYRUS_SCRIPT, GetVersionInt, 0);
		
		return true;
	}
}
