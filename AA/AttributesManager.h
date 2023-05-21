#pragma once
#include "AttributeInfo.h"
#include "AttributeInfoSet.h"
#include "f4se/GameForms.h"
#include "f4se/PapyrusArgs.h"

class AttributesManager
{
	BSReadWriteLock attributesLock;
	AttributeInfoSet attributesStorage;

public:
	void DumpAll();

	AttributeInfo GetRegistrationInfo(const ActorValueInfo* avi);
	std::vector<AttributeInfo> GetRegistrationInfo(VMArray<ActorValueInfo*>&);
	std::vector<ActorValueInfo*> GetRegisteredAttributes();

	bool Save(const F4SESerializationInterface* intfc);
	bool Load(const F4SESerializationInterface* intfc);
	void Clear();

	bool Add(ActorValueInfo* avi, float defaultValue, float minValue, float defaultMax, float randomMin, float randomMax, float mean, float deviation);
	bool Update(ActorValueInfo* avi, float defaultValue, float randomMin, float randomMax, float mean, float deviation);
	bool Remove(const ActorValueInfo* avi);
	bool Contains(const ActorValueInfo* avi);
	std::vector<AttributeInfo> GetValues(const std::vector<ActorValueInfo*>& attributes);

	void SubscribeForChange(const std::function<void(void)>& functor);
};
extern AttributesManager g_AttributesManager;

