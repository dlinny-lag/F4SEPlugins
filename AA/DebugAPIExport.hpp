#pragma once
#include "f4se/PapyrusVM.h"
#include "f4se/PapyrusNativeFunctions.h"
#include "MyTrace.h"

#include "AVChangesAPIExport.hpp"
#include "PluginAPIExport.hpp"
#include "AttributesAPIExport.hpp"
#include "CalculatorsAPIExport.hpp"

namespace DebugAPIExport
{
	const char* EXPORT_PAPYRUS_SCRIPT = "AA:Debug";

	static void LogToConsole(StaticFunctionTag* _)
	{
		LogSettings::SetTraceToConsole(!LogSettings::TraceToConsole);
		_MESSAGE("TraceToConsole=%s", S(LogSettings::TraceToConsole));
	}

	static void SetLogLevel(StaticFunctionTag* _, UInt32 level)
	{
		if (level < LogSettings::MyLogLevel::None)
			level = LogSettings::MyLogLevel::None;
		if (level > LogSettings::MyLogLevel::Debug)
			level = LogSettings::MyLogLevel::Debug;

		LogSettings::SetLogLevel(static_cast<LogSettings::MyLogLevel>(level));
		_MESSAGE("LogLevel = %d", LogSettings::LogLevel);
	}

	static void Dump(StaticFunctionTag* _)
	{
		AttributesAPIExport::DumpAll();
		CalculatorsAPIExport::DumpAll();
		AVChangesAPIExport::DumpAll();
		CalculationSession::DumpAll();
	}

	static bool Register(VirtualMachine* vm)
	{
		vm->RegisterFunction(
			new NativeFunction0<StaticFunctionTag, void>("LogToConsole", EXPORT_PAPYRUS_SCRIPT, LogToConsole, vm));
		vm->SetFunctionFlags(EXPORT_PAPYRUS_SCRIPT, "LogToConsole", IFunction::kFunctionFlag_NoWait);

		vm->RegisterFunction(
			new NativeFunction1<StaticFunctionTag, void, UInt32>("SetLogLevel", EXPORT_PAPYRUS_SCRIPT, SetLogLevel, vm));
		vm->SetFunctionFlags(EXPORT_PAPYRUS_SCRIPT, "SetLogLevel", IFunction::kFunctionFlag_NoWait);

		vm->RegisterFunction(
			new NativeFunction0<StaticFunctionTag, void>("Dump", EXPORT_PAPYRUS_SCRIPT, Dump, vm));
		vm->SetFunctionFlags(EXPORT_PAPYRUS_SCRIPT, "Dump", IFunction::kFunctionFlag_NoWait);

		return true;
	}
}