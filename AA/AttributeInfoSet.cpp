#include "AttributeInfoSet.h"
#include "f4se/Serialization.h"
#include "f4se/GameRTTI.h"
#include "f4se/GameReferences.h"
#include  <algorithm>
#include "MyTrace.h"
#include "Shared/Comparer.hpp"

const int dataStructure1 = 1;
const int dataStructure2 = 2;
const int dataStructure3 = 3;
UInt32 AttributeInfoSet::currentDataStructureVersion = dataStructure3;

void AttributeInfoSet::DumpAll()
{
	Dmp("Registered attributes:");
	for (auto& aiPair : avis)
	{
		aiPair.second.Print(aiPair.first);
	}
}

bool AttributeInfoSet::Save(const F4SESerializationInterface* serializer)
{
	const UInt32 size = avis.size();
	if (!serializer->WriteRecordData(&size, sizeof(size)))
		return FE("Failed to write AVs size");

	for (auto& avi : avis)
	{
		if (!avi.second.Save(serializer))
			return FE("Failed to save Attribute Info");
	}

	return true;
}

bool AttributeInfoSet::Load(const F4SESerializationInterface* serializer, UInt32 version)
{
	if (version != currentDataStructureVersion)
		return FE("Unexpected AttributeInfoSet version %d. Expected %d", version, currentDataStructureVersion);

	sorted.clear();
	UInt32 size = 0;
	if (!Serialization::ReadData(serializer, &size))
	{
		return FE("Error reading AVIF list size");
	}

	for (UInt32 i = 0; i < size; i++)
	{
		AttributeInfo ai;

		if (!AttributeInfo::Load(serializer, ai))
		{
			return FE("Error reading AttributeInfo");
		}

		const UInt32 savedId = ai.Id;
		UInt32 actualId = 0;

		if (!serializer->ResolveFormId(savedId, &actualId))
		{
			I("Failed to resolve AV (%08X)", savedId);
			continue; // skip if AVIF is missing
		}
		if (ActorValueInfo* avi = DYNAMIC_CAST(LookupFormByID(actualId), TESForm, ActorValueInfo))
		{
			ai.Id = actualId;
			ai.AV = avi;
			ai.InitDistributions();
			avis.insert(std::make_pair(actualId, ai)); // insert directly
		}
		else // ignore invalid ActorValueInfo... not sure how it could happens
			W("Invalid AVIF with saved id=%d and resolved id=%d", savedId, actualId);
	}
	FireChanged();
	return true;
}

std::vector<ActorValueInfo*> AttributeInfoSet::GetSorted()
{
	
	if (sorted.size() == avis.size())
		return sorted;
	sorted.clear();
	sorted.reserve(avis.size());
	for (auto& pair : avis)
	{
		sorted.push_back(pair.second.AV);
	}
	sort(sorted.begin(), sorted.end(), ComparerLess<FORM_TYPE>());
	return sorted;
}

bool AttributeInfoSet::Add(ActorValueInfo* avi, float defaultValue, float minValue, float defaultMax, float randomMin, float randomMax, float mean, float deviation)
{
	if (avi)
	{
		AttributeInfo ai;
		ai.AV = avi;
		ai.Id = avi->formID;
		ai.DefaultValue = defaultValue;
		ai.MinValue = minValue;
		ai.DefaultBase = defaultMax;
		ai.RandomMin = randomMin;
		ai.RandomMax = randomMax;
		ai.RandomMean = mean;
		ai.RandomDeviation = deviation;
		if (!ai.IsValid())
			return FD("Invalid Attribute Info");
		const bool retVal = avis.insert(std::make_pair(avi->formID, ai)).second;
		if (retVal)
		{
			ai.InitDistributions();
			FireChanged();
		}
		return retVal;
	}
	return FD("Attempt to add empty AV");
}
bool AttributeInfoSet::Update(ActorValueInfo* avi, float defaultValue, float randomMin, float randomMax, float mean, float deviation)
{
	if (avi)
	{
		const auto ptr = avis.find(avi->formID);
		if (ptr == avis.end())
			return FD("Update: attribute %08X is not registered", avi->formID);

		const AttributeInfo& existing = ptr->second;

		AttributeInfo ai;
		ai.AV = avi;
		ai.Id = avi->formID;
		ai.DefaultValue = defaultValue;
		ai.MinValue = existing.MinValue;
		ai.DefaultBase = existing.DefaultBase;
		ai.RandomMin = randomMin;
		ai.RandomMax = randomMax;
		ai.RandomMean = mean;
		ai.RandomDeviation = deviation;
		if (!ai.IsValid())
			return FD("Invalid Attribute Info");

		avis.erase(avi->formID);
		const bool retVal = avis.insert(std::make_pair(avi->formID, ai)).second;
		if (retVal)
		{
			ai.InitDistributions();
			FireChanged();
		}
		return retVal;
	}
	return FD("Attempt to add empty AV");
}

bool AttributeInfoSet::Remove(const ActorValueInfo* avi)
{
	if (avi)
	{
		const bool retVal = avis.erase(avi->formID) > 0; // something removed
		if (retVal)
			FireChanged();
		return retVal;
	}
	return FD("Attempt to remove empty attribute");
}

bool AttributeInfoSet::Contains(UInt32 id)
{
	return avis.find(id) != avis.end();
}

bool AttributeInfoSet::Contains(const ActorValueInfo* avi)
{
	return avi && avis.find(avi->formID) != avis.end();
}

bool AttributeInfoSet::IsEmpty()
{
	return avis.empty();
}

UInt32 AttributeInfoSet::Size()
{
	return avis.size();
}

void AttributeInfoSet::Clear()
{
	avis.clear();
	FireChanged();
}

AttributeInfo AttributeInfoSet::Get(const ActorValueInfo* avi)
{
	if (!avi)
		return AttributeInfo();
	const auto pair = avis.find(avi->formID);
	if (pair == avis.end())
		return AttributeInfo();
	return pair->second;
}
std::vector<AttributeInfo> AttributeInfoSet::GetValues(const std::vector<ActorValueInfo*>& attributes)
{
	
	std::vector<AttributeInfo> retVal;
	retVal.reserve(attributes.size());
	for(const ActorValueInfo* attr : attributes)
	{
		retVal.emplace_back(Get(attr));
	}
	return retVal;
}

void AttributeInfoSet::ForEach(const std::function<void(const AttributeInfo&)>& functor)
{
	for (auto &pair : avis)
	{
		functor(pair.second);
	}
}

void AttributeInfoSet::FireChanged()
{
	sorted.clear();
	Changeable::FireChanged();
}

