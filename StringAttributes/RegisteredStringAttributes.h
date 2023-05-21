#pragma once
#include "f4se/GameForms.h"
#include "f4se/PluginAPI.h"
#include <unordered_map>

class RegisteredStringAttributes
{
	std::unordered_map<BGSKeyword*, BSFixedString> registeredAttributes;
public:
	bool Add(BGSKeyword* attribute, BSFixedString& defaultValue);
	bool Remove(BGSKeyword* attribute);
	bool TryGet(BGSKeyword* attribute, BSFixedString* defaultValue) const;
	bool Save(const F4SESerializationInterface* serializer) const;
	bool Load(const F4SESerializationInterface* serializer);
	void ForEach(const std::function<void(BGSKeyword*, BSFixedString&)>& functor);

	static UInt32 currentDataStructureVersion;

	void DumpAll() const;

	void Clear();

private :
	bool LoadInternal(const F4SESerializationInterface* serializer, UInt32 version);
};

