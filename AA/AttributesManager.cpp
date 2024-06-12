#include "AttributesManager.h"

#include "MyTrace.h"

AttributesManager g_AttributesManager;

void AttributesManager::DumpAll()
{
	BSReadLocker lock(&attributesLock);
	attributesStorage.DumpAll();
}

void AttributesManager::SubscribeForChange(const std::function<void(void)>& functor)
{
	attributesStorage.SubscribeForChange(functor);
}

AttributeInfo AttributesManager::GetRegistrationInfo(const ActorValueInfo* avi)
{
	BSReadLocker lock(&attributesLock);
	return attributesStorage.Get(avi);
}
std::vector<AttributeInfo> AttributesManager::GetRegistrationInfo(VMArray<ActorValueInfo*>& avis)
{
	std::vector<ActorValueInfo*> attrs;
	attrs.reserve(avis.Length());
	for (UInt32 i = 0; i < avis.Length(); i++)
	{
		ActorValueInfo* avi = nullptr;
		avis.Get(&avi, i);
		attrs.emplace_back(avi);
	}
	BSReadLocker lock(&attributesLock);
	return attributesStorage.GetValues(attrs);
}
std::vector<ActorValueInfo*> AttributesManager::GetRegisteredAttributes()
{
	BSWriteLocker lock(&attributesLock);
	return attributesStorage.GetSorted();
}

constexpr UInt32 rootTag = 'ATTR';
bool AttributesManager::Save(const F4SESerializationInterface* intfc)
{
	if (intfc->OpenRecord(rootTag, AttributeInfoSet::currentDataStructureVersion))
	{
		return attributesStorage.Save(intfc);
	}
	return FE("Failed to open record (%.4s)", &rootTag);
}

bool AttributesManager::Load(const F4SESerializationInterface* intfc)
{
	UInt32 recType, length, curVersion;
	if (intfc->GetNextRecordInfo(&recType, &curVersion, &length))
	{
		if (recType != rootTag)
		{
			return FE("Invalid record type (%.4s). Expected (%.4s)", &recType, &rootTag);
		}
		return attributesStorage.Load(intfc, curVersion);
	}
	return FE("AttributesAPIExport. Failed to get record.");
}

void AttributesManager::Clear()
{
	attributesStorage.Clear();
}

bool AttributesManager::Add(ActorValueInfo* avi, float defaultValue, float minValue, float defaultMax, float randomMin, float randomMax, float mean, float deviation)
{
	BSWriteLocker lock(&attributesLock);
	return attributesStorage.Add(avi, defaultValue, minValue, defaultMax, randomMin, randomMax, mean, deviation);
}

bool AttributesManager::Update(ActorValueInfo* avi, float defaultValue, float randomMin, float randomMax, float mean, float deviation)
{
	BSWriteLocker lock(&attributesLock);
	return attributesStorage.Update(avi, defaultValue, randomMin, randomMax, mean, deviation);
}
bool AttributesManager::Remove(const ActorValueInfo* avi)
{
	BSWriteLocker lock(&attributesLock);
	return attributesStorage.Remove(avi);
}
bool AttributesManager::Contains(const ActorValueInfo* avi)
{
	BSReadLocker lock(&attributesLock);
	return attributesStorage.Contains(avi);
}
std::vector<AttributeInfo> AttributesManager::GetValues(const std::vector<ActorValueInfo*>& attributes)
{
	BSReadLocker lock(&attributesLock);
	return this->attributesStorage.GetValues(attributes);
}