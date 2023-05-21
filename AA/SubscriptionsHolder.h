#pragma once
#include "f4se/GameReferences.h"
#include "f4se/PapyrusArgs.h"
#include "FormsSet.hpp"
#include <unordered_map>
#include "Shared/ScriptHandle.hpp"
struct SubscriberInfo
{
	FormsSet<Actor> Actors; // list of actors to listen
	FormsSet<ActorValueInfo> AVs; // list of actor values to listed
};

class SubscriptionsHolder
{
	std::unordered_map<ScriptHandle, SubscriberInfo> subscriptions;
	BSReadWriteLock handlersLock;

public:
	bool Register(UInt64 handle, BSFixedString name, VMArray<Actor*> actors, VMArray<ActorValueInfo*> avis);

	bool Unregister(UInt64 handle, BSFixedString name);

	void NotifyIncreased(std::vector<TESForm*>& senders, Actor* a, ActorValueInfo* attribute, float newVal, float prevVal, float exceed);

	void NotifyDecreased(std::vector<TESForm*>& senders, Actor* a, ActorValueInfo* attribute, float newVal, float prevVal, float exceed);

	bool Save(const F4SESerializationInterface* serializer);

	bool Load(const F4SESerializationInterface* serializer, UInt32 version);

	void Clear();

	static UInt32 currentDataStructureVersion;
	bool Contains(UInt64 handle, BSFixedString scriptName);
	void DumpAll();

private:
	inline void Notify(std::vector<TESForm*>& senders, const char* eventName, Actor* a, ActorValueInfo* attribute, float newVal, float prevVal, float exceed);
};

extern SubscriptionsHolder g_Subscriptions;


