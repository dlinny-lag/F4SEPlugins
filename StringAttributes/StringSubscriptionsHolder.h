#pragma once

#include "f4se/Serialization.h"
#include "f4se/GameReferences.h"
#include "f4se/PapyrusArgs.h"
#include "Shared/FormsSet.hpp"
#include <unordered_map>
#include "Shared/ScriptHandle.hpp"

struct StringSubscriberInfo
{
	FormsSet<Actor> Actors; // list of actors to listen
	FormsSet<BGSKeyword> Attributes; // list of attributes to listen
};

class StringSubscriptionsHolder
{
	std::unordered_map<ScriptHandle, StringSubscriberInfo > subscriptions;
public:

	const char * const EventName = "OnStringChanged";

	bool Register(const ScriptHandle& script, VMArray<Actor*> actors, VMArray<BGSKeyword*> attributes);

	bool Unregister(const ScriptHandle& script);

	void Notify(TESForm* sender, Actor* a, BGSKeyword* attribute, BSFixedString& prevValue, BSFixedString& newValue);

	bool Save(const F4SESerializationInterface* serializer) const;

	bool Load(const F4SESerializationInterface* serializer);

	void Clear();

	bool Contains(const ScriptHandle& script) const;

	static UInt32 currentDataStructureVersion;

	void DumpAll();
private :
	bool LoadInternal(const F4SESerializationInterface* serializer, UInt32 version);
};

