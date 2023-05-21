#pragma once

#include "f4se/PapyrusVM.h"
#include "f4se/PapyrusNativeFunctions.h"
#include "f4se/GameReferences.h"
#include "f4se/PapyrusEvents.h"
#include "FormsSet.hpp"
#include "SubscriptionsHolder.h"
#include "AttributeModifier.hpp"

#include "Shared/ScriptHandle.hpp"

#define AA_AVChanges_PAPYRUS_SCRIPT "AA:AVChanges"

namespace AVChangesAPIExport
{
	const char* EXPORT_PAPYRUS_SCRIPT = AA_AVChanges_PAPYRUS_SCRIPT;

	void DumpAll()
	{
		g_Subscriptions.DumpAll();
	}

	void Clear()
	{
		g_Subscriptions.Clear();
	}

	const UInt32 rootTag = 'AVCS';
	bool Save(const F4SESerializationInterface* intfc)
	{
		if (intfc->OpenRecord(rootTag, SubscriptionsHolder::currentDataStructureVersion))
		{
			return g_Subscriptions.Save(intfc);
		}
		return FE("Failed to open record (%.4s)", &rootTag);
	}

	bool Load(const F4SESerializationInterface* intfc)
	{
		UInt32 recType, length, curVersion;
		if (intfc->GetNextRecordInfo(&recType, &curVersion, &length))
		{
			if (recType != rootTag)
			{
				return FE("Invalid record type (%.4s). Expected (%.4s)", &recType, &rootTag);
			}
			
			return g_Subscriptions.Load(intfc, curVersion);
		}
		return FE("AVChangesAPIExport. Failed to get record.");
	}

	

	bool RegisterForAttributesChanges(StaticFunctionTag* _, VMObject* script, VMArray<Actor*> actors, VMArray<ActorValueInfo*> avis)
	{
		if (!script)
			return FD("Empty script");
		const UInt64 handle = SafeGetHandle(script);
		if (handle == 0)
			return FE("Invalid script");
		const BSFixedString scriptName = script->GetObjectType();
		const bool res = g_Subscriptions.Register(handle, scriptName, actors, avis);
		D("RegisterForAttributesChanges: [%s](%016X) with result=%s", scriptName.c_str(), handle, S(res));
		return res;
	}


	bool UnregisterForAttributesChanges(StaticFunctionTag* _, VMObject* script)
	{
		if (!script)
			return FD("Empty script");

		const UInt64 handle = SafeGetHandle(script);
		if (handle == 0)
			return FE("Invalid script");
		const BSFixedString scriptName = script->GetObjectType();
		const bool res = g_Subscriptions.Unregister(handle, scriptName);
		D("UnregisterForAttributesChanges: [%s](%016X) with result=%s", scriptName.c_str(), handle, S(res));
		return res;
	}


	bool IsHandlerRegistered(StaticFunctionTag* _, VMObject* script)
	{
		if (!script)
			return FD("Empty script");
		const UInt64 handle = SafeGetHandle(script);
		if (handle == 0)
			return FE("Invalid script");
		const BSFixedString scriptName = script->GetObjectType();
		const bool res = g_Subscriptions.Contains(handle, scriptName);
		D("IsHandlerRegistered: [%s](%016X) with result=%s", scriptName.c_str(), handle, S(res));
		return res;
	}

	DECLARE_STRUCT(ScriptDescriptor, AA_AVChanges_PAPYRUS_SCRIPT);

	ScriptDescriptor GetSubscribedDescriptor(StaticFunctionTag* _, VMObject* script)
	{
		ScriptDescriptor retVal;
		retVal.SetNone(true);
		if (!script)
		{
			D("Empty script");
			return retVal;
		}
		
		const UInt64 handle = SafeGetHandle(script);
		if (handle == 0)
		{
			E("Invalid script");
			return retVal;
		}
		const BSFixedString scriptName = script->GetObjectType();
		const bool res = g_Subscriptions.Contains(handle, scriptName);
		if (!res)
			return retVal;

		const UInt32 highWord = handle >> 32;
		const UInt32 formId = handle & 0xFFFFFFFF;
		retVal.SetNone(false);
		retVal.Set<UInt32>("FormId", formId);
		retVal.Set<UInt32>("HighWord", highWord);
		retVal.Set<BSFixedString>("Name", scriptName);

		return retVal;
	}

	bool UnregisterForAttributesChangesByDescriptor(StaticFunctionTag* _, ScriptDescriptor descriptor)
	{
		if (descriptor.IsNone())
			return FD("None descriptor");
		UInt32 highWord;
		UInt32 formId;
		BSFixedString scriptName;
		if (!descriptor.Get("HighWord", &highWord))
			return FW("Can't get HighWord field");
		if (!descriptor.Get("FormId", &formId))
			return FW("Can't get FormId field");
		if (!descriptor.Get("Name", &scriptName))
			return FW("Can't get Name field");
		UInt64 handle = highWord;
		handle <<= 32;
		handle |= formId;
		const bool res = g_Subscriptions.Unregister(handle, scriptName);
		D("UnregisterForAttributesChangesByDescriptor: [%s](%016X) with result=%s", scriptName.c_str(), handle, S(res));
		return res;
	}
	void Register(VirtualMachine* vm)
	{

		// notifications subscribers registering

		vm->RegisterFunction(
			new NativeFunction3("RegisterForAttributesChanges", EXPORT_PAPYRUS_SCRIPT, RegisterForAttributesChanges, vm));
		vm->SetFunctionFlags(EXPORT_PAPYRUS_SCRIPT, "RegisterForAttributesChanges", IFunction::kFunctionFlag_NoWait);

		vm->RegisterFunction(
			new NativeFunction1("UnregisterForAttributesChanges", EXPORT_PAPYRUS_SCRIPT, UnregisterForAttributesChanges, vm));
		vm->SetFunctionFlags(EXPORT_PAPYRUS_SCRIPT, "UnregisterForAttributesChanges", IFunction::kFunctionFlag_NoWait);

		vm->RegisterFunction(
			new NativeFunction1("IsHandlerRegistered", EXPORT_PAPYRUS_SCRIPT, IsHandlerRegistered, vm));
		vm->SetFunctionFlags(EXPORT_PAPYRUS_SCRIPT, "IsHandlerRegistered", IFunction::kFunctionFlag_NoWait);

		vm->RegisterFunction(
			new NativeFunction1("GetSubscribedDescriptor", EXPORT_PAPYRUS_SCRIPT, GetSubscribedDescriptor, vm));
		vm->SetFunctionFlags(EXPORT_PAPYRUS_SCRIPT, "GetSubscribedDescriptor", IFunction::kFunctionFlag_NoWait);

		vm->RegisterFunction(
			new NativeFunction1("UnregisterForAttributesChangesByDescriptor", EXPORT_PAPYRUS_SCRIPT, UnregisterForAttributesChangesByDescriptor, vm));
		vm->SetFunctionFlags(EXPORT_PAPYRUS_SCRIPT, "UnregisterForAttributesChangesByDescriptor", IFunction::kFunctionFlag_NoWait);


	}
};

