#include "StringSubscriptionsHolder.h"
#include "f4se/Serialization.h"
#include "f4se/PapyrusArgs.h"
#include "f4se/PapyrusEvents.h"
#include "Shared/SerializationHelper.h"
#include "Shared/MyTrace.h"
#include "DataTypeId.h"

constexpr UInt32 dataStructure1 = 1;
UInt32 StringSubscriptionsHolder::currentDataStructureVersion = dataStructure1;

bool StringSubscriptionsHolder::Register(const ScriptHandle& script, VMArray<Actor*> actors, VMArray<BGSKeyword*> attributes)
{
	const auto pair = subscriptions.find(script);
	if (pair != subscriptions.end())
		return FE("Subscriber already registered");
	if (actors.Length() == 0 && attributes.Length() == 0)
		return FD("Actors and Attributes are empty");

	StringSubscriberInfo si;
	const char* name = script.Name.c_str();
	D("Registering string attributes change handler [%s]", name);

	for (UInt32 i = 0; i < actors.Length(); i++)
	{
		Actor* actor;
		actors.Get(&actor, i);
		if (actor)
			si.Actors.Add(actor);
		else
			return FD("Empty actor");
	}

	for (UInt32 i = 0; i < attributes.Length(); i++)
	{
		BGSKeyword* attr;
		attributes.Get(&attr, i);
		if (attr)
			si.Attributes.Add(attr);
		else
			return FD("Empty AV");
	}

	if (!subscriptions.insert(std::make_pair(script, si)).second)
		return FD("Failed to insert Subscriber with name %s", name);

	IObjectHandlePolicy* policy = (*g_gameVM)->m_virtualMachine->GetHandlePolicy();
	policy->AddRef(script.Handle);

	return true;
}

bool StringSubscriptionsHolder::Unregister(const ScriptHandle& script)
{
	const auto iter = subscriptions.find(script);
	if (iter == subscriptions.end())
		return FD("Subscriber %s not found", script.Name.c_str());
	VirtualMachine* vm = (*g_gameVM)->m_virtualMachine;
	subscriptions.erase(iter);
	vm->GetHandlePolicy()->Release(script.Handle);
	return true;
}

bool StringSubscriptionsHolder::Contains(const ScriptHandle& script) const
{
	return subscriptions.find(script) != subscriptions.end();
}

void StringSubscriptionsHolder::Notify(TESForm* sender, Actor* a, BGSKeyword* attribute, BSFixedString& prevValue, BSFixedString& newValue)
{
	for (auto& pair : subscriptions)
	{
		const bool actorFlag = pair.second.Actors.IsEmpty() || pair.second.Actors.Contains(a);
		const bool avFlag = pair.second.Attributes.IsEmpty() || pair.second.Attributes.Contains(attribute);
		if (actorFlag && avFlag)
		{
			SendPapyrusEvent5<TESForm*, Actor*, BGSKeyword*, BSFixedString, BSFixedString>(pair.first.Handle, pair.first.Name, EventName,
				sender, a, attribute, newValue, prevValue);
		}
	}
}

constexpr UInt32 rootTag = StringSubscriptionsHolderTypeId;
bool StringSubscriptionsHolder::Save(const F4SESerializationInterface* serializer) const
{
	if (!serializer->OpenRecord(rootTag, StringSubscriptionsHolder::currentDataStructureVersion))
	{
		return FE("Failed to open record (%.4s)", &rootTag);
	}

	const UInt32 size = subscriptions.size();
	if (!serializer->WriteRecordData(&size, sizeof(size)))
		return FE("Failed to write size of subscribers");

	for (auto& pair : subscriptions)
	{
		if (!SerializationHelper::WriteString(serializer, &pair.first.Name))
			return FE("Failed to write subscriber script name");
		if (!Serialization::WriteData(serializer, &pair.first.Handle))
			return FE("Failed to write subscriber script handle");
		if (!pair.second.Actors.Save(serializer))
			return FE("Failed to write subscriber's actors");
		if (!pair.second.Attributes.Save(serializer))
			return FE("Failed to write subscriber's AVs");
	}

	return true;
}

bool StringSubscriptionsHolder::Load(const F4SESerializationInterface* serializer)
{
	UInt32 recType, length, curVersion;
	if (serializer->GetNextRecordInfo(&recType, &curVersion, &length))
	{
		if (recType != rootTag)
		{
			return FE("Invalid record type (%.4s). Expected (%.4s)", &recType, &rootTag);
		}
		return LoadInternal(serializer, curVersion);
	}
	return FE("StringSubscriptionsHolder. Failed to get record.");
}

bool StringSubscriptionsHolder::LoadInternal(const F4SESerializationInterface* serializer, UInt32 version)
{
	if (version != currentDataStructureVersion)
		return FE("Wrong SubscriptionsHolder version %d. Expected %d", version, currentDataStructureVersion);

	UInt32 size;
	if (!serializer->ReadRecordData(&size, sizeof(size)))
		return FE("Failed to read size of subscribers");

	for (UInt32 i = 0; i < size; i++)
	{
		ScriptHandle key;
		StringSubscriberInfo si;
		if (!SerializationHelper::ReadString(serializer, &key.Name))
			return FE("Failed to read subscriber's script name");
		UInt64 scriptHandle;
		if (!Serialization::ReadData(serializer, &scriptHandle))
			return FE("Failed to read subscriber's script handle");

		const bool valid = SerializationHelper::ResolveHandle(serializer, scriptHandle, &key.Handle);
		UInt32 lostActors = 0;
		if (!si.Actors.Load(serializer, lostActors))
			return FE("Failed to read actors for [%s] subscriber", key.Name.c_str());
		UInt32 lostAVs = 0;
		if (!si.Attributes.Load(serializer, lostAVs))
			return FE("Failed to read SVs for [%s] subscriber", key.Name.c_str());

		D("Reading string subscription record. Script: %s (%016X)", key.Name.c_str(), key.Handle);

		// it is possible that some registered actors or/and AVs are lost due to mod(s) uninstallation
		if (si.Actors.IsEmpty() && si.Attributes.IsEmpty())
		{
			// do not add subscriber that hasn't any filters
			W("[%s] subscriber lost all filters.", key.Name.c_str());
		}
		else if (si.Actors.IsEmpty() && lostActors > 0)
		{
			// some actors was registered but lost
			// so don't spam subscriber with ALL actors
			W("[%s] subscriber lost all actors.", key.Name.c_str());
		}
		else if (si.Attributes.IsEmpty() && lostAVs > 0)
		{
			// some AVs was registered and they lost
			// so don't spam subscriber with ALL AVs
			W("[%s] subscriber lost all string attributes.", key.Name.c_str());
		}
		else if (valid)
		{
			if (!subscriptions.insert(std::make_pair(key, si)).second)
				W("Subscriptions already have record for [%s]", key.Name.c_str());
		}
		else
		{
			W("[%s] subscriber disappeared.", key.Name.c_str());
		}
	}
	return true;
}

void StringSubscriptionsHolder::Clear()
{
	subscriptions.clear();
}


void StringSubscriptionsHolder::DumpAll()
{
	Dmp("Registered string attribute changes subscribers:");
	for (auto& pair : subscriptions)
	{
		Dmp("  [%s] (%016X)", pair.first.Name.c_str(), pair.first.Handle);

		Dmp("  Actors:");
		pair.second.Actors.ForEach(FormsSet<Actor>::PrintForm);

		Dmp("  Attributes:");
		pair.second.Attributes.ForEach(FormsSet<BGSKeyword>::PrintForm);
	}
}