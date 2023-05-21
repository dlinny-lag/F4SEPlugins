#pragma once
#include "f4se/PapyrusArgs.h"
#include "FormsSet.hpp"
#include "Changeable.h"
#include <unordered_map>
#include <vector>
#include <unordered_set>
#include "ScriptHandle.hpp"


class CalculatorsHolder : public Changeable
{
	std::unordered_map<ScriptHandle, std::vector<ActorValueInfo*>> calculators;
	std::vector<ScriptHandle> sorted;
	
public:

	bool Register(const ScriptHandle& script, const std::vector<ActorValueInfo*>& avis);

	bool Unregister(const ScriptHandle& script);

	bool IsRegistered(const ScriptHandle& script);

	const std::vector<ActorValueInfo*> Get(const ScriptHandle& script) const;

	std::vector<ScriptHandle>& GetSorted();

	bool Save(const F4SESerializationInterface* serializer) const;

	bool Load(const F4SESerializationInterface* serializer, UInt32 version);

	void Clear();

	static UInt32 currentDataStructureVersion;

	void DumpAll();

protected:
	void FireChanged() override;
};

