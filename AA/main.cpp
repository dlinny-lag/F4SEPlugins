#include <shlobj.h>
#include <ctime>

#include "AttributesAPIExport.hpp"
#include "f4se/PluginAPI.h"
#include "f4se_common/f4se_version.h"
#include "f4se/PapyrusVM.h"

#include "AVChangesAPIExport.hpp"
#include "PluginAPIExport.hpp"
#include "CalculatorsAPIExport.hpp"
#include "CalculatorAttributesIndices.hpp"
#include "DebugAPIExport.hpp"
#include "UniqueIdGenerator.hpp"

#include "MyTrace.h"
#include "Dispatcher.h"

#if F4SE_PRE_DECLARATIVE_LOAD
	#define REQUIRED_RUNTIME RUNTIME_VERSION_1_10_163
#endif
#if _F4SE_DECLARATIVE_LOAD
	#define REQUIRED_RUNTIME RUNTIME_VERSION_1_10_984
#endif

#ifndef REQUIRED_RUNTIME
	#error Invalid project configuration
#endif


PluginHandle g_pluginHandle = kPluginHandle_Invalid;

F4SEPapyrusInterface* g_papyrus = nullptr;
F4SESerializationInterface* g_serialization = nullptr;
F4SEMessagingInterface* g_messaging = nullptr;
F4SETaskInterface* g_tasks = nullptr;


bool RegisterExportingFunctions(VirtualMachine* vm)
{
	_MESSAGE("RegisterFuncs");
	PluginAPIExport::Register(vm);
	AttributesAPIExport::Register(vm);
	AVChangesAPIExport::Register(vm);
	CalculatorsAPIExport::Register(vm);
	DebugAPIExport::Register(vm);
	UniqueIdGenerator::Register(vm);
	return true;
}

void Serialization_Revert(const F4SESerializationInterface* intfc)
{
	_MESSAGE("Revert");
	AVChangesAPIExport::Clear();
	AttributesAPIExport::Clear();
	CalculatorsAPIExport::Clear();
	CalculationSession::Clear();
	UniqueIdGenerator::Clear();
	
}
void Serialization_Save(const F4SESerializationInterface* intfc)
{
	_MESSAGE("Game Saving...");
	// order has matter!
	AVChangesAPIExport::Save(intfc);
	AttributesAPIExport::Save(intfc);
	CalculatorsAPIExport::Save(intfc);
	CalculationSession::Save(intfc);
	UniqueIdGenerator::Save(intfc);
	_MESSAGE("Game Saved");
}
void Serialization_Load(const F4SESerializationInterface* intfc)
{
	_MESSAGE("Game Loading...");
	// order has matter!
	AVChangesAPIExport::Load(intfc);
	AttributesAPIExport::Load(intfc);
	CalculatorsAPIExport::Load(intfc);
	CalculationSession::Load(intfc);
	UniqueIdGenerator::Load(intfc);
	if (LogSettings::LogLevel == LogSettings::Debug)
	{
		DebugAPIExport::Dump(nullptr);
	}
	_MESSAGE("Game Loaded");
}

void MessageCallback(F4SEMessagingInterface::Message* msg)
{
	if (msg->type == F4SEMessagingInterface::kMessage_PreLoadGame || msg->type == F4SEMessagingInterface::kMessage_NewGame)
	{
		
	}
}

void InitLogs()
{
	IDebugLog::OpenRelative(CSIDL_MYDOCUMENTS, R"(\My Games\Fallout4\F4SE\AA.log)");
	LogSettings::Init("F4AA");
}

extern "C"
{
#if F4SE_PRE_DECLARATIVE_LOAD
	__declspec(dllexport) bool F4SEPlugin_Query(const F4SEInterface * f4se, PluginInfo * info)
	{
		InitLogs();

		// populate info structure
		info->infoVersion = PluginInfo::kInfoVersion;
		info->name = PluginAPIExport::pluginName;
		info->version = PluginAPIExport::pluginVersionInt;

		_MESSAGE("F4SEPlugin_Query successful.");
		return true;
	}
#endif

#if _F4SE_DECLARATIVE_LOAD
	__declspec(dllexport) F4SEPluginVersionData F4SEPlugin_Version =
	{
		F4SEPluginVersionData::kVersion,
		
		PluginAPIExport::pluginVersionInt,
		AA_PluginName,
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
		g_tasks = (F4SETaskInterface*)f4se->QueryInterface(kInterface_Task);
		if (!g_tasks)
		{
			_ERROR("Failed to get F4SETaskInterface");
			return false;
		}

		Notifications::Dispatcher::Init(g_pluginHandle, g_messaging, g_tasks);

		srand(static_cast <unsigned> (time(0)));

		g_AttributesManager.SubscribeForChange(CalculatorAttributesIndices::Reset);
		CalculatorsAPIExport::calculators.SubscribeForChange(CalculatorAttributesIndices::Reset);

		g_serialization->SetUniqueID(g_pluginHandle, PluginAPIExport::pluginUID);
		g_serialization->SetRevertCallback(g_pluginHandle, Serialization_Revert);
		g_serialization->SetSaveCallback(g_pluginHandle, Serialization_Save);
		g_serialization->SetLoadCallback(g_pluginHandle, Serialization_Load);
		//g_serialization->SetFormDeleteCallback(g_pluginHandle, nullptr);

		// register papyrus exports
		g_papyrus->Register(RegisterExportingFunctions);


		_MESSAGE("Loaded %s", PluginAPIExport::pluginVersionString);
		return true;
	}

};
