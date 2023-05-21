#pragma once
#include "f4se/PapyrusVM.h"
#include "f4se/PapyrusNativeFunctions.h"
#include "f4se/GameReferences.h"
#include "f4se/PapyrusEvents.h"

#include "ActorStringAttributes.h"
#include "RegisteredStringAttributes.h"
#include "StringSubscriptionsHolder.h"

#include "Shared/MyTrace.h"
#include "Shared/ScriptHandle.hpp"

#define StringAttributes_PAPYRUS_SCRIPT "StringAttribute"

namespace StringAttributeAPIExport
{
	const char* EXPORT_PAPYRUS_SCRIPT = StringAttributes_PAPYRUS_SCRIPT;
	const char* pluginName = "StringAttributes";
	const UInt32 pluginUID = 'F4SA';
	const UInt32 pluginVersionInt = 0x0050;
	const char* pluginVersionString = "0.5.0";

	UInt32 GetVersion(StaticFunctionTag* _)
	{
		return pluginVersionInt;
	}
	BSFixedString GetVersionString(StaticFunctionTag* _)
	{
		return pluginVersionString;
	}

	RegisteredStringAttributes registeredAttributes;
	BSReadWriteLock registeredLock;

	bool RegisterStringAttribute(StaticFunctionTag* _, BGSKeyword* attribute, BSFixedString defaultValue)
	{
		BSWriteLocker lock(&registeredLock);
		return registeredAttributes.Add(attribute, defaultValue);
	}
	bool UnregisterStringAttribute(StaticFunctionTag* _, BGSKeyword* attribute)
	{
		BSWriteLocker lock(&registeredLock);
		return registeredAttributes.Remove(attribute);
	}

	bool IsStringAttributeRegistered(StaticFunctionTag* _, BGSKeyword* attribute)
	{
		BSReadLocker lock(&registeredLock);
		BSFixedString v;
		return registeredAttributes.TryGet(attribute, &v);
	}

	BSFixedString GetDefault(BGSKeyword* attribute)
	{
		BSReadLocker lock(&registeredLock);
		BSFixedString defaultValue;
		registeredAttributes.TryGet(attribute, &defaultValue);
		return defaultValue;
	}
	std::function<BSFixedString(BGSKeyword*)> getDefault(GetDefault);

	ActorStringAttributes storage;
	BSReadWriteLock storageLock;
	StringSubscriptionsHolder subscribers;
	BSReadWriteLock susbcribersLock;


	BSFixedString SetString(StaticFunctionTag* _, TESForm* sender, Actor* actor, BGSKeyword* attribute, BSFixedString value)
	{
		BSFixedString retVal;
		{
			BSWriteLocker lock(&storageLock);
			retVal = storage.SetValue(actor, attribute, value);
		}
		BSReadLocker lock(&susbcribersLock);
		subscribers.Notify(sender, actor, attribute, value, retVal);
		return retVal;
	};

	BSFixedString GetString(StaticFunctionTag* _, Actor* actor, BGSKeyword* attribute)
	{
		BSReadLocker lock(&storageLock);
		return storage.GetValue(actor, attribute, getDefault);
	}


	BSFixedString Append(StaticFunctionTag* _, TESForm* sender, Actor* actor, BGSKeyword* attribute, BSFixedString value)
	{
		BSFixedString retVal;
		BSFixedString prevValue;
		{ 
			// TODO: optimize
			BSWriteLocker lock(&storageLock);
			prevValue = storage.GetValue(actor, attribute, getDefault);
			std::string combined = prevValue.c_str();
			combined.append(value.c_str());
			retVal = combined.c_str();
			storage.SetValue(actor, attribute, retVal);
		}
		BSReadLocker lock(&susbcribersLock);
		subscribers.Notify(sender, actor, attribute, prevValue, retVal);
		return retVal;
	}

	bool RegisterForAttributesChange(StaticFunctionTag* _, VMObject* script, VMArray<Actor*> actors, VMArray<BGSKeyword*> attributes)
	{
		if (!script)
			return FD("Script not provided");
		const ScriptHandle s(script);
		if (s.IsInvalid())
			return FW("Invalid script");

		BSWriteLocker lock(&susbcribersLock);
		return subscribers.Register(s, actors, attributes);
	}
	bool UnregisterForAttributesChange(StaticFunctionTag* _, VMObject* script)
	{
		if (!script)
			return FD("Script not provided");
		const ScriptHandle s(script);
		if (s.IsInvalid())
			return FW("Invalid script");

		BSWriteLocker lock(&susbcribersLock);
		return subscribers.Unregister(s);
	}

	bool IsHandlerRegistered(StaticFunctionTag* _, VMObject* script)
	{
		if (!script)
			return false;
		const ScriptHandle s(script);
		if (s.IsInvalid())
			return FW("Invalid script");

		BSReadLocker lock(&susbcribersLock);
		return subscribers.Contains(s);
	}

	DECLARE_STRUCT(ScriptDescriptor, StringAttributes_PAPYRUS_SCRIPT);

	ScriptDescriptor GetSubscribedDescriptor(StaticFunctionTag* _, VMObject* script)
	{
		ScriptDescriptor retVal;
		retVal.SetNone(true);
		if (!script)
		{
			D("Empty script");
			return retVal;
		}
		const ScriptHandle s(script);
		if (s.IsInvalid())
		{
			E("Invalid script");
			return retVal;
		}
		
		bool res;
		{
			BSReadLocker lock(&susbcribersLock);
			res = subscribers.Contains(s);
		}
		if (!res)
			return retVal;

		const UInt32 highWord = s.Handle >> 32;
		const UInt32 formId = s.Handle & 0xFFFFFFFF;
		retVal.SetNone(false);
		retVal.Set<UInt32>("FormId", formId);
		retVal.Set<UInt32>("HighWord", highWord);
		retVal.Set<BSFixedString>("Name", s.Name);

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
		const ScriptHandle s(handle, scriptName);
		bool res;
		{
			BSWriteLocker lock(&susbcribersLock);
			res = subscribers.Unregister(s);
		}
		D("UnregisterForAttributesChangesByDescriptor: [%s](%016X) with result=%s", scriptName.c_str(), handle, S(res));
		return res;
	}

	void DumpAll()
	{
		{
			BSWriteLocker lock(&registeredLock);
			registeredAttributes.DumpAll();
		}
		{
			BSWriteLocker lock(&storageLock);
			storage.DumpAll();
		}
		{
			BSWriteLocker lock(&susbcribersLock);
			subscribers.DumpAll();
		}
	}
	void Clear()
	{
		{
			BSWriteLocker lock(&registeredLock);
			registeredAttributes.Clear();
		}
		{
			BSWriteLocker lock(&storageLock);
			storage.Clear();
		}
		{
			BSWriteLocker lock(&susbcribersLock);
			subscribers.Clear();
		}
	}

	bool Save(const F4SESerializationInterface* serializer)
	{
		{
			BSWriteLocker lock(&registeredLock);
			registeredAttributes.Save(serializer);
		}
		{
			BSWriteLocker lock(&storageLock);
			storage.Save(serializer);
		}
		{
			BSWriteLocker lock(&susbcribersLock);
			subscribers.Save(serializer);
		}
		return true;
	}
	bool Load(const F4SESerializationInterface* serializer)
	{
		D("Loading registeredAttributes");
		registeredAttributes.Load(serializer);
		D("Loading storage");
		storage.Load(serializer);
		D("Loading subscribers");
		subscribers.Load(serializer);
		return true;
	}


	bool Register(VirtualMachine* vm)
	{
		_MESSAGE("StringAttributeAPIExport: Register");

		vm->RegisterFunction(
			new NativeFunction0("GetVersion", EXPORT_PAPYRUS_SCRIPT, GetVersion, vm));
		vm->SetFunctionFlags(EXPORT_PAPYRUS_SCRIPT, "GetVersion", IFunction::kFunctionFlag_NoWait);
		vm->RegisterFunction(
			new NativeFunction0("GetVersionString", EXPORT_PAPYRUS_SCRIPT, GetVersionString, vm));
		vm->SetFunctionFlags(EXPORT_PAPYRUS_SCRIPT, "GetVersionString", IFunction::kFunctionFlag_NoWait);


		vm->RegisterFunction(
			new NativeFunction2("RegisterStringAttribute", EXPORT_PAPYRUS_SCRIPT, RegisterStringAttribute, vm));
		vm->SetFunctionFlags(EXPORT_PAPYRUS_SCRIPT, "RegisterStringAttribute", IFunction::kFunctionFlag_NoWait);

		vm->RegisterFunction(
			new NativeFunction1("UnregisterStringAttribute", EXPORT_PAPYRUS_SCRIPT, UnregisterStringAttribute, vm));
		vm->SetFunctionFlags(EXPORT_PAPYRUS_SCRIPT, "UnregisterStringAttribute", IFunction::kFunctionFlag_NoWait);

		vm->RegisterFunction(
			new NativeFunction1("IsStringAttributeRegistered", EXPORT_PAPYRUS_SCRIPT, IsStringAttributeRegistered, vm));
		vm->SetFunctionFlags(EXPORT_PAPYRUS_SCRIPT, "IsStringAttributeRegistered", IFunction::kFunctionFlag_NoWait);


		vm->RegisterFunction(
			new NativeFunction4("Append", EXPORT_PAPYRUS_SCRIPT, Append, vm));
		vm->SetFunctionFlags(EXPORT_PAPYRUS_SCRIPT, "Append", IFunction::kFunctionFlag_NoWait);

		vm->RegisterFunction(
			new NativeFunction4("SetString", EXPORT_PAPYRUS_SCRIPT, SetString, vm));
		vm->SetFunctionFlags(EXPORT_PAPYRUS_SCRIPT, "SetString", IFunction::kFunctionFlag_NoWait);

		vm->RegisterFunction(
			new NativeFunction2("GetString", EXPORT_PAPYRUS_SCRIPT, GetString, vm));
		vm->SetFunctionFlags(EXPORT_PAPYRUS_SCRIPT, "GetString", IFunction::kFunctionFlag_NoWait);



		vm->RegisterFunction(
			new NativeFunction3("RegisterForAttributesChange", EXPORT_PAPYRUS_SCRIPT, RegisterForAttributesChange, vm));
		vm->SetFunctionFlags(EXPORT_PAPYRUS_SCRIPT, "RegisterForAttributesChange", IFunction::kFunctionFlag_NoWait);

		vm->RegisterFunction(
			new NativeFunction1("UnregisterForAttributesChange", EXPORT_PAPYRUS_SCRIPT, UnregisterForAttributesChange, vm));
		vm->SetFunctionFlags(EXPORT_PAPYRUS_SCRIPT, "UnregisterForAttributesChange", IFunction::kFunctionFlag_NoWait);

		vm->RegisterFunction(
			new NativeFunction1("IsHandlerRegistered", EXPORT_PAPYRUS_SCRIPT, IsHandlerRegistered, vm));
		vm->SetFunctionFlags(EXPORT_PAPYRUS_SCRIPT, "IsHandlerRegistered", IFunction::kFunctionFlag_NoWait);

		vm->RegisterFunction(
			new NativeFunction1("GetSubscribedDescriptor", EXPORT_PAPYRUS_SCRIPT, GetSubscribedDescriptor, vm));
		vm->SetFunctionFlags(EXPORT_PAPYRUS_SCRIPT, "GetSubscribedDescriptor", IFunction::kFunctionFlag_NoWait);

		vm->RegisterFunction(
			new NativeFunction1("UnregisterForAttributesChangesByDescriptor", EXPORT_PAPYRUS_SCRIPT, UnregisterForAttributesChangesByDescriptor, vm));
		vm->SetFunctionFlags(EXPORT_PAPYRUS_SCRIPT, "UnregisterForAttributesChangesByDescriptor", IFunction::kFunctionFlag_NoWait);

		return true;
	}


}
