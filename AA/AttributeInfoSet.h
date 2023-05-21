#pragma once

#include "f4se/GameForms.h"
#include "f4se/PluginAPI.h"
#include "AttributeInfo.h"
#include <unordered_map>
#include <vector>
#include "Changeable.h"

class AttributeInfoSet : public Changeable
{
	std::unordered_map<UInt32, AttributeInfo> avis;
	std::vector<ActorValueInfo*> sorted;
public:

	bool Save(const F4SESerializationInterface* serializer);

	bool Load(const F4SESerializationInterface* serializer, UInt32 version);
	
	void Clear();

	bool Add(ActorValueInfo* avi, float defaultValue, float minValue, float defaultMax, float randomMin, float randomMax, float mean, float deviation);
	bool Update(ActorValueInfo* avi, float defaultValue, float randomMin, float randomMax, float mean, float deviation);
	bool Remove(const ActorValueInfo* avi);

	bool Contains(const ActorValueInfo* avi);

	bool Contains(UInt32 id);

	bool IsEmpty();

	UInt32 Size();

	AttributeInfo Get(const ActorValueInfo* avi);
	std::vector<AttributeInfo> GetValues(const std::vector<ActorValueInfo*>& attributes);
	std::vector<ActorValueInfo*> GetSorted();

	void ForEach(const std::function<void(const AttributeInfo&)>& functor);

	static UInt32 currentDataStructureVersion;

	void DumpAll();

protected:
	virtual void FireChanged();
};


