#pragma once
#include "f4se/Serialization.h"
#include "f4se/GameReferences.h"
#include "f4se/PapyrusArgs.h"
#include <unordered_map>
#include <functional>

class ActorStringAttributes
{
	// first key - Actor's FormId, second key - Keyword's FormId
	std::unordered_map<UInt32, std::unordered_map<UInt32, BSFixedString>> map;

public:

	// returns previous value
	BSFixedString SetValue(Actor* actor, BGSKeyword* attribute, BSFixedString& newValue);

	BSFixedString GetValue(Actor* actor, BGSKeyword* attribute, std::function<BSFixedString(BGSKeyword*)>& functor) const;

	bool Save(const F4SESerializationInterface* serializer) const;

	bool Load(const F4SESerializationInterface* serializer);

	void Clear();

	static UInt32 currentDataStructureVersion;

	void DumpAll();
private:
	bool LoadInternal(const F4SESerializationInterface* serializer, UInt32 version);
};

