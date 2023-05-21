#include "SubscriptionsHolder.h"
#include "f4se/Serialization.h"
#include "f4se/PapyrusArgs.h"
#include "f4se/PapyrusEvents.h"
#include "SerializationHelper.h"
#include "Dispatcher.h"
#include "MyTrace.h"



constexpr int dataStructure1 = 1;
UInt32 SubscriptionsHolder::currentDataStructureVersion = dataStructure1;
SubscriptionsHolder g_Subscriptions;

void SubscriptionsHolder::DumpAll()
{
	Dmp("Registered attribute changes subscribers:");
	BSReadLocker lock(&handlersLock);
	for (auto& pair : subscriptions)
	{
		Dmp("  [%s] (%016X)", pair.first.Name.c_str(), pair.first.Handle);

		Dmp("  Actors:");
		pair.second.Actors.ForEach(FormsSet<Actor>::PrintForm);

		Dmp("  Attributes:");
		pair.second.AVs.ForEach(FormsSet<ActorValueInfo>::PrintForm);
	}
}

bool SubscriptionsHolder::Register(UInt64 handle, BSFixedString scriptName, VMArray<Actor*> actors, VMArray<ActorValueInfo*> avis)
{
	const ScriptHandle script{ handle, scriptName };
	BSWriteLocker lock(&handlersLock);
	if (subscriptions.find(script) != subscriptions.end())
		return FD("Subscriber with name %s is already registered. Tried %016X", scriptName.c_str(), handle);

	if (actors.Length() == 0 && avis.Length() == 0)
		return FD("Actors and AVs are empty");

	SubscriberInfo si;

	for (UInt32 i = 0; i < actors.Length(); i++)
	{
		Actor* actor;
		actors.Get(&actor, i);
		if (actor)
			si.Actors.Add(actor);
		else
			return FD("Empty actor");
	}

	for (UInt32 i = 0; i < avis.Length(); i++)
	{
		ActorValueInfo* avi;
		avis.Get(&avi, i);
		if (avi)
			si.AVs.Add(avi);
		else
			return FD("Empty AV");
	}

	if (!subscriptions.insert(std::make_pair(script, si)).second)
		return FD("Failed to insert Subscriber [%s](%016X)", scriptName.c_str(), handle);

	IObjectHandlePolicy* policy = (*g_gameVM)->m_virtualMachine->GetHandlePolicy();
	policy->AddRef(handle);

	return true;
}

bool SubscriptionsHolder::Unregister(UInt64 handle, BSFixedString scriptName)
{
	const ScriptHandle script{ handle, scriptName };
	BSWriteLocker lock(&handlersLock);
	const auto iter = subscriptions.find(script);
	if (iter == subscriptions.end())
		return FD("Subscriber %s not found", scriptName.c_str());
	VirtualMachine* vm = (*g_gameVM)->m_virtualMachine;
	subscriptions.erase(iter);
	vm->GetHandlePolicy()->Release(handle);
	return true;
}

bool SubscriptionsHolder::Contains(UInt64 handle, BSFixedString scriptName)
{
	const ScriptHandle script{ handle, scriptName };
	BSReadLocker lock(&handlersLock);
	return subscriptions.find(script) != subscriptions.end();
}

inline void SubscriptionsHolder::Notify(std::vector<TESForm*>& senders, const char* eventName, Actor* a, ActorValueInfo* attribute, float newVal, float prevVal, float exceed)
{
	D("Sending %s for actor %08X, attribute=%08X. values %f, %f, %f. Subs=%d", eventName, a->formID, attribute->formID, newVal, prevVal, exceed, subscriptions.size());
	BSReadLocker lock(&handlersLock);
	for (auto& pair : subscriptions)
	{
		const bool actorFlag = pair.second.Actors.IsEmpty() || pair.second.Actors.Contains(a);
		const bool avFlag = pair.second.AVs.IsEmpty() || pair.second.AVs.Contains(attribute);
		if (actorFlag && avFlag)
		{
			D("Call (%016X)%s.%s", pair.first.Handle, pair.first.Name.c_str(), eventName);
			VMArray<TESForm*> arr(senders);
			SendPapyrusEvent6<VMArray<TESForm*>, Actor*, ActorValueInfo*, float, float, float>(pair.first.Handle, pair.first.Name, eventName,
				arr, a, attribute, newVal, prevVal, exceed);
		}
	}
}

void NotifyF4SEPlugins(bool increase, const std::vector<TESForm*>& senders, Actor* a, ActorValueInfo* attribute, float newVal, float prevVal, float exceed)
{
	Notifications::Change change(a, attribute, prevVal, newVal, exceed, senders);
	if (increase)
		Notifications::Dispatcher::DispatchIncrease(std::move(change));
	else
		Notifications::Dispatcher::DispatchDecrease(std::move(change));
}

// no arguments check
void SubscriptionsHolder::NotifyIncreased(std::vector<TESForm*>& senders, Actor* a, ActorValueInfo* attribute, float newVal, float prevVal, float exceed)
{
	Notify(senders, "OnIncreased", a, attribute, newVal, prevVal, exceed);
	NotifyF4SEPlugins(true, senders, a, attribute, newVal, prevVal, exceed);
}

// No arguments check
void SubscriptionsHolder::NotifyDecreased(std::vector<TESForm*>& senders, Actor* a, ActorValueInfo* attribute, float newVal, float prevVal, float exceed)
{
	Notify(senders, "OnDecreased", a, attribute, newVal, prevVal, exceed);
	NotifyF4SEPlugins(false, senders, a, attribute, newVal, prevVal, exceed);
}

void SubscriptionsHolder::Clear()
{
	// TODO: Not sure why we don't call Release during Clear
//	VirtualMachine* vm = (*g_gameVM)->m_virtualMachine;
//	auto policy = vm->GetHandlePolicy();
//	for (auto& sub : subscriptions)
//	{
//		policy->Release(sub.first.Handle);
//	}
	subscriptions.clear();
}

bool SubscriptionsHolder::Save(const F4SESerializationInterface* serializer)
{
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
		if (!pair.second.AVs.Save(serializer))
			return FE("Failed to write subscriber's AVs");
	}

	return true;
}

bool SubscriptionsHolder::Load(const F4SESerializationInterface* serializer, UInt32 version)
{
	if (version != currentDataStructureVersion)
		return FE("Wrong SubscriptionsHolder version %d. Expected %d", version, currentDataStructureVersion);

	UInt32 size;
	if (!serializer->ReadRecordData(&size, sizeof(size)))
		return FE("Failed to read size of subscribers");
	VirtualMachine * vm = (*g_gameVM)->m_virtualMachine;
	for (UInt32 i = 0; i < size; i++)
	{
		SubscriberInfo si;
		ScriptHandle script;
		if (!SerializationHelper::ReadString(serializer, &script.Name))
			return FE("Failed to read subscriber's script name");
		if (!Serialization::ReadData(serializer, &script.Handle))
			return FE("Failed to read subscriber's script handle");
		bool valid = SerializationHelper::ResolveHandle(serializer, script.Handle, &script.Handle);
		if (valid)
		{
			VMIdentifier * identifier = nullptr;
			if (vm->GetObjectIdentifier(script.Handle, script.Name.c_str(), 0, &identifier, 0))
			{
				// release our reference
				if(identifier)
				{
					valid = true;
					if(!identifier->DecrementLock())
					{
						identifier->Destroy();
						D("Destroyed identifier for script %s (%016X)", script.Name.c_str(), script.Handle);
					}
				}
				else
				{
					valid = false;
				}
			}
			else
			{
				valid = false;
			}
		}
		UInt32 lostActors = 0;
		if (!si.Actors.Load(serializer, lostActors))
			return FE("Failed to read actors for [%s] subscriber", script.Name.c_str());
		UInt32 lostAVs = 0;
		if (!si.AVs.Load(serializer, lostAVs))
			return FE("Failed to read SVs for [%s] subscriber", script.Name.c_str());

		D("Reading subscription record. Script: %s (%016X)", script.Name.c_str(), script.Handle);

		// it is possible that some registered actors or/and AVs are lost due to mod(s) uninstallation
		if (si.Actors.IsEmpty() && si.AVs.IsEmpty())
		{
			// do not add subscriber that hasn't any filters
			W("[%s] subscriber lost all filters.", script.Name.c_str());
		}
		else if (si.Actors.IsEmpty() && lostActors > 0)
		{
			// some actors was registered but lost
			// so don't spam subscriber with ALL actors
			W("[%s] subscriber lost all actors.", script.Name.c_str());
		}
		else if (si.AVs.IsEmpty() && lostAVs > 0)
		{
			// some AVs was registered and they lost
			// so don't spam subscriber with ALL AVs
			W("[%s] subscriber lost all AVs.", script.Name.c_str());
		}
		else if (valid)
		{
			
			if (!subscriptions.insert(std::make_pair(script, si)).second)
				W("Subscriptions already have record for [%s]", script.Name.c_str());
		}
		else
		{
			W("[%s] subscriber disappeared.", script.Name.c_str());
		}
	}
	return true;
}