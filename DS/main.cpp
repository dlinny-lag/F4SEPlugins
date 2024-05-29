#include <shlobj.h>
#include <ctime>
#include "f4se/PluginAPI.h"
#include "f4se_common/f4se_version.h"
#include "f4se/PapyrusVM.h"
#include "PluginAPIExport.hpp"

#include "ArrayAPIExport.h"
#include "DebugAPIExport.hpp"
#include "SetAPIExport.h"
#include "../Shared/MyTrace.h"
#include "ValueDictAPIExport.h"
#include "ArrayDictAPIExport.h"
#include "AccessRequestsHandler.h"
#include "LoadManager.h"

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


bool RegisterExportingFunctions(VirtualMachine* vm)
{
	_MESSAGE("RegisterFuncs");
	PluginAPIExport::Register(vm);
	DebugAPIExport::Register(vm);
	ArrayAPIExport::Register(vm);
	SetAPIExport::Register(vm);
	ValueDictAPIExport::Register(vm);
	ArrayDictAPIExport::Register(vm);
	return true;
}

void Serialization_Revert(const F4SESerializationInterface* intfc)
{
	_MESSAGE("Revert");
	ArrayAPIExport::Clear();
	SetAPIExport::Clear();
	ValueDictAPIExport::Clear();
	ArrayDictAPIExport::Clear();
	
}
void Serialization_Save(const F4SESerializationInterface* intfc)
{
	_MESSAGE("Game Saving...");
	ArrayAPIExport::Save(intfc);
	SetAPIExport::Save(intfc);
	ValueDictAPIExport::Save(intfc);
	ArrayDictAPIExport::Save(intfc);
	_MESSAGE("Game Saved");
}
void Serialization_Load(const F4SESerializationInterface* intfc)
{
	_MESSAGE("Game Loading...");
	DS::LoadManager::LoadRecords(intfc);
	_MESSAGE("Game Loaded");
	if (
#ifdef _DEBUG
		true ||
#endif
		LogSettings::LogLevel == LogSettings::MyLogLevel::Debug)
	{
		ArrayAPIExport::DumpAll();
		SetAPIExport::DumpAll();
		ValueDictAPIExport::DumpAll();
		ArrayDictAPIExport::DumpAll();
	}
}

void RegisterLoaders()
{
	ArrayAPIExport::RegisterLoaders();
	SetAPIExport::RegisterLoaders();
	ValueDictAPIExport::RegisterLoaders();
	ArrayDictAPIExport::RegisterLoaders();
}

DSAPI::AccessorRequestProcessor* processor = nullptr;
void ProcessRequest(const DSAPI::AccessorRequest* request, const char* requester)
{
	D("Accessor of type [%d] requested by %s", request->Type, requester);
	if (!processor->ProcessRequest(request, requester))
		W("Failed to process accessor request {Type=%d, Version=%d}", request->Type, request->Version);
}

void RequestsMessageCallback(F4SEMessagingInterface::Message* msg)
{
	if (msg->type == DSAPI::AccessorRequest::MessageType)
	{
		if (msg->dataLen != sizeof(DSAPI::AccessorRequest))
			return;
		const auto request = static_cast<DSAPI::AccessorRequest*>(msg->data);
		ProcessRequest(request, msg->sender);
	}
}

void F4SEMessageCallback(F4SEMessagingInterface::Message* msg)
{
	if (msg->type == F4SEMessagingInterface::kMessage_PostLoad)
	{
		if (!g_messaging->RegisterListener(g_pluginHandle, nullptr, RequestsMessageCallback))
			E("Failed to register requests receiver");
		else
			I("Started accessor requests listener");
	}
}

void InitLogs()
{
	IDebugLog::OpenRelative(CSIDL_MYDOCUMENTS, R"(\My Games\Fallout4\F4SE\DS.log)");
	LogSettings::Init("F4DS");
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
		"Fallout 4 Data Structures",
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

		processor = new DSAPI::AccessorRequestProcessor(g_pluginHandle, g_messaging);

		g_serialization = (F4SESerializationInterface*)f4se->QueryInterface(kInterface_Serialization);
		if (!g_serialization)
		{
			_ERROR("Failed to get F4SESerializationInterface");
			return false;
		}
		
		srand(static_cast <unsigned> (time(nullptr))); // TODO: remove?

		g_serialization->SetUniqueID(g_pluginHandle, PluginAPIExport::pluginUID);
		g_serialization->SetRevertCallback(g_pluginHandle, Serialization_Revert);
		g_serialization->SetSaveCallback(g_pluginHandle, Serialization_Save);
		g_serialization->SetLoadCallback(g_pluginHandle, Serialization_Load);
		//g_serialization->SetFormDeleteCallback(g_pluginHandle, nullptr);

		RegisterLoaders();

		g_messaging->RegisterListener(g_pluginHandle, "F4SE", F4SEMessageCallback);

		// register papyrus exports
		g_papyrus->Register(RegisterExportingFunctions);

		_MESSAGE("Loaded %s", PluginAPIExport::pluginVersionString);
		return true;
	}

};
