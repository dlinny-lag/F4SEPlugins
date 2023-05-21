#include <shlobj.h>
#include <ctime>
#include "f4se/PluginAPI.h"
#include "f4se_common/f4se_version.h"
#include "f4se/PapyrusVM.h"

#include "StringAttributeAPIExport.hpp"
#include "../Shared/MyTrace.h"

#define REQUIRED_RUNTIME RUNTIME_VERSION_1_10_163


PluginHandle g_pluginHandle = kPluginHandle_Invalid;

F4SEPapyrusInterface* g_papyrus = nullptr;
F4SESerializationInterface* g_serialization = nullptr;
F4SEMessagingInterface* g_messaging = nullptr;


bool RegisterExportingFunctions(VirtualMachine* vm)
{
	_MESSAGE("RegisterFuncs");
	StringAttributeAPIExport::Register(vm);
	return true;
}

void Serialization_Revert(const F4SESerializationInterface* intfc)
{
	_MESSAGE("Revert");
	StringAttributeAPIExport::Clear();
}
void Serialization_Save(const F4SESerializationInterface* intfc)
{
	_MESSAGE("Game Saving...");
	StringAttributeAPIExport::Save(intfc);
	_MESSAGE("Game Saved");
}
void Serialization_Load(const F4SESerializationInterface* intfc)
{
	_MESSAGE("Game Loading...");
	StringAttributeAPIExport::Load(intfc);
}



void F4SEMessageCallback(F4SEMessagingInterface::Message* msg)
{
	if (msg->type == F4SEMessagingInterface::kMessage_PostLoad)
	{
	}
}

extern "C"
{
	bool F4SEPlugin_Query(const F4SEInterface * f4se, PluginInfo * info)
	{
		IDebugLog::OpenRelative(CSIDL_MYDOCUMENTS, R"(\My Games\Fallout4\F4SE\StringAttributes.log)");
		LogSettings::Init("F4DS");

		// populate info structure
		info->infoVersion = PluginInfo::kInfoVersion;
		info->name = StringAttributeAPIExport::pluginName;
		info->version = StringAttributeAPIExport::pluginVersionInt;

		// store plugin handle so we can identify ourselves later
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

		_MESSAGE("F4SEPlugin_Query successful.");
		return true;
	}

	bool F4SEPlugin_Load(const F4SEInterface * f4se)
	{
		g_serialization->SetUniqueID(g_pluginHandle, StringAttributeAPIExport::pluginUID);
		g_serialization->SetRevertCallback(g_pluginHandle, Serialization_Revert);
		g_serialization->SetSaveCallback(g_pluginHandle, Serialization_Save);
		g_serialization->SetLoadCallback(g_pluginHandle, Serialization_Load);
		//g_serialization->SetFormDeleteCallback(g_pluginHandle, nullptr);

		g_messaging->RegisterListener(g_pluginHandle, "F4SE", F4SEMessageCallback);

		// register papyrus exports
		g_papyrus->Register(RegisterExportingFunctions);

		_MESSAGE("Loaded %s", StringAttributeAPIExport::pluginVersionString);
		return true;
	}

};
