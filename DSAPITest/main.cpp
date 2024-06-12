#include <shlobj.h>

#include "f4se/PluginAPI.h"
#include "f4se_common/f4se_version.h"
#include "f4se/PapyrusVM.h"
#include "f4se/PapyrusNativeFunctions.h"
#include "../DS/CollectionAccessor.h"
#include "TestRunner.h"
#include "MyTrace.h"

#if F4SE_PRE_DECLARATIVE_LOAD
	#define REQUIRED_RUNTIME RUNTIME_VERSION_1_10_163
#endif
#if _F4SE_DECLARATIVE_LOAD
	#define REQUIRED_RUNTIME RUNTIME_VERSION_1_10_984
#endif

#ifndef REQUIRED_RUNTIME
	#error Invalid project configuration
#endif

constexpr UInt32 pluginUID = 'DSTS'; // DS Test

PluginHandle g_pluginHandle = kPluginHandle_Invalid;

F4SEPapyrusInterface* g_papyrus = nullptr;
F4SEMessagingInterface* g_messaging = nullptr;
F4SESerializationInterface* g_serialization = nullptr;

void DSMessageHandler(F4SEMessagingInterface::Message* msg)
{
	Test::ProcessMessage(msg);
}
void Serialization_Revert(const F4SESerializationInterface* intfc)
{
	Test::CancelTest();
}

void F4SEMessageCallback(F4SEMessagingInterface::Message* msg)
{
	if (msg->type == F4SEMessagingInterface::kMessage_PostPostLoad)
	{
		DSAPI::RegisterForAccessorsReceiving(g_messaging, g_pluginHandle, DSMessageHandler);
		Test::InitiateAccessorsInitialization();
	}
}

void StartTest(StaticFunctionTag* _)
{
	Test::InitiateTestsRun();
}

bool RegisterExportingFunctions(VirtualMachine* vm)
{
	vm->RegisterFunction(new NativeFunction0("StartTest", "DSTest:Runner", StartTest, vm));
	return true;
}
void InitLogs()
{
	IDebugLog::OpenRelative(CSIDL_MYDOCUMENTS, "\\My Games\\Fallout4\\F4SE\\DSTest.log");
	LogSettings::SetLogLevel(LogSettings::Debug);
	LogSettings::SetTraceToConsole(true);
}
extern "C"
{
#if F4SE_PRE_DECLARATIVE_LOAD
	__declspec(dllexport) bool F4SEPlugin_Query(const F4SEInterface * f4se, PluginInfo * info)
	{
		InitLogs();
		// populate info structure
		info->infoVersion = PluginInfo::kInfoVersion;
		info->name = "DSTest";
		info->version = 1;


		_MESSAGE("F4SEPlugin_Query successful.");
		return true;
	}
#endif

#if _F4SE_DECLARATIVE_LOAD
	__declspec(dllexport) F4SEPluginVersionData F4SEPlugin_Version =
	{
		F4SEPluginVersionData::kVersion,
		
		1,
		"F4DS Tests",
		"Dlinny_Lag",

		F4SEPluginVersionData::kAddressIndependence_AddressLibrary_1_10_980,
		F4SEPluginVersionData::kStructureIndependence_1_10_980Layout,
		{ REQUIRED_RUNTIME, 0 },
		0,
		0,
		0,
		{0}
	};

#endif

	__declspec(dllexport) bool F4SEPlugin_Load(const F4SEInterface * f4se)
	{
#if _F4SE_DECLARATIVE_LOAD
		// logs was not initialized at F4SEPlugin_Query
		InitLogs();
#endif

		g_pluginHandle = f4se->GetPluginHandle();

		if(f4se->isEditor)
		{
			_ERROR("Editor is not supported");

			return false;
		}
		else if(f4se->runtimeVersion < REQUIRED_RUNTIME)
		{
			_ERROR("Unsupported runtime version %08X (expected %08X or higher)", f4se->runtimeVersion, REQUIRED_RUNTIME);

			return false;
		}

		g_papyrus = (F4SEPapyrusInterface *)f4se->QueryInterface(kInterface_Papyrus);
		if(!g_papyrus)
		{
			_ERROR("Failed to get F4SEPapyrusInterface");
			return false;
		}

		g_messaging = (F4SEMessagingInterface *)f4se->QueryInterface(kInterface_Messaging);
		if(!g_messaging)
		{
			_ERROR("Failed to get F4SEMessagingInterface");
			return false;
		}
		g_serialization = (F4SESerializationInterface*)f4se->QueryInterface(kInterface_Serialization);
		if (!g_serialization)
		{
			_ERROR("Failed to get F4SESerializationInterface");
			return false;
		}

		g_messaging->RegisterListener(g_pluginHandle, "F4SE", F4SEMessageCallback);

		// register papyrus exports
		g_papyrus->Register(RegisterExportingFunctions);
		g_serialization->SetUniqueID(g_pluginHandle, pluginUID);
		g_serialization->SetRevertCallback(g_pluginHandle, Serialization_Revert);

		Test::Init(g_messaging, g_pluginHandle);

		// NativeFunctionBase

		_MESSAGE("F4SEPlugin_Load successful.");
		return true;
	}

};
