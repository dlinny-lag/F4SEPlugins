#pragma once
#include "f4se/PapyrusVM.h"
#include "f4se/PapyrusNativeFunctions.h"
#include "f4se/GameReferences.h"
#include "f4se/PapyrusEvents.h"
#include "MyTrace.h"
#include "ArrayAPIExport.h"
#include "SetAPIExport.h"
#include "ValueDictAPIExport.h"
#include "ArrayDictAPIExport.h"

namespace DebugAPIExport
{
	const char* EXPORT_PAPYRUS_SCRIPT = "DS:Debug";

	void VMLogToConsole(StaticFunctionTag* _, bool enable)
	{
		LogSettings::SetTraceToConsole(enable);
		_MESSAGE("TraceToConsole=%s", S(LogSettings::TraceToConsole));
	}

	void VMSetLogLevel(StaticFunctionTag* _, UInt32 level)
	{
		if (level < LogSettings::MyLogLevel::None)
			level = LogSettings::MyLogLevel::None;
		if (level > LogSettings::MyLogLevel::Debug)
			level = LogSettings::MyLogLevel::Debug;

		LogSettings::SetLogLevel(static_cast<LogSettings::MyLogLevel>(level));
		_MESSAGE("LogLevel = %d", LogSettings::LogLevel);
	}

	void VMDump(StaticFunctionTag* _)
	{
		ArrayAPIExport::DumpAll();
		SetAPIExport::DumpAll();
		ValueDictAPIExport::DumpAll();
		ArrayDictAPIExport::DumpAll();
	}

	void VMPrint(StaticFunctionTag* _, BSFixedString message)
	{
		Dmp("%s", message.c_str());
	}

	bool Register(VirtualMachine* vm)
	{
		_MESSAGE("DebugAPIExport: Register");
		REGISTER_FUNC(vm, EXPORT_PAPYRUS_SCRIPT, LogToConsole, 1);
		REGISTER_FUNC(vm, EXPORT_PAPYRUS_SCRIPT, SetLogLevel, 1);
		REGISTER_FUNC(vm, EXPORT_PAPYRUS_SCRIPT, Dump, 0);
		REGISTER_FUNC(vm, EXPORT_PAPYRUS_SCRIPT, Print, 1);

		return true;
	}
}