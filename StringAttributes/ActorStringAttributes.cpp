#include "ActorStringAttributes.h"
#include "SerializationHelper.h"
#include "f4se/GameRTTI.h"
#include "Shared/MyTrace.h"
#include "DataTypeId.h"

constexpr int dataStructure1 = 1;
UInt32 ActorStringAttributes::currentDataStructureVersion = dataStructure1;

BSFixedString ActorStringAttributes::SetValue(Actor* actor, BGSKeyword* attribute, BSFixedString& newValue)
{
	if (!actor)
	{
		D("SetValue: no actor");
		return BSFixedString();
	}
	if (!attribute)
	{
		D("SetValue: no attribute");
		return BSFixedString();
	}

	const auto actorPair = map.find(actor->formID);

	if (actorPair == map.end())
	{
		std::unordered_map<UInt32, BSFixedString> attrDict;
		attrDict.insert(std::make_pair(attribute->formID, newValue));
		map.insert(std::make_pair(actor->formID, attrDict));
		return BSFixedString();
	}
	const auto attrPair = actorPair->second.find(attribute->formID);
	if (attrPair == actorPair->second.end())
	{
		actorPair->second.insert(std::make_pair(attribute->formID, newValue));
		return BSFixedString();
	}
	const BSFixedString retVal = attrPair->second;
	attrPair->second = newValue;
	return retVal;
}

BSFixedString ActorStringAttributes::GetValue(Actor* actor, BGSKeyword* attribute, std::function<BSFixedString(BGSKeyword*)>& functor) const
{
	if (!actor)
	{
		D("GetValue: no actor");
		return BSFixedString();
	}
	if (!attribute)
	{
		D("GetValue: no attribute");
		return BSFixedString();
	}
	const auto actorPair = map.find(actor->formID);
	if (actorPair == map.end())
		return functor(attribute);
	const auto attrPair = actorPair->second.find(attribute->formID);
	if (attrPair == actorPair->second.end())
		return BSFixedString();
	return attrPair->second;
}

const UInt32 rootTag = ActorStringAttributesTypeId;
bool ActorStringAttributes::Save(const F4SESerializationInterface* serializer) const
{
	if (!serializer->OpenRecord(rootTag, ActorStringAttributes::currentDataStructureVersion))
	{
		return FE("Failed to open record (%.4s)", &rootTag);
	}

	const UInt32 actorsCount = map.size();
	if (!serializer->WriteRecordData(&actorsCount, sizeof(actorsCount)))
		return FE("Failed to write actors string attributes map size");

	for (auto& actorPair : map)
	{
		if (!serializer->WriteRecordData(&actorPair.first, sizeof(actorPair.first)))
			return FE("Failed to write actor id for string attributes");

		UInt32 attributesCount = actorPair.second.size();
		if (!serializer->WriteRecordData(&attributesCount, sizeof(attributesCount)))
			return FE("Failed to write string attribues size");
		for (auto& attrPair : actorPair.second)
		{
			if (!serializer->WriteRecordData(&attrPair.first, sizeof(attrPair.first)))
				return FE("Failed to write string attribute id");

			if (!SerializationHelper::WriteString(serializer, &attrPair.second))
				return FE("Failed to write string attribute value");
		}
	}
	return true;
}

bool ActorStringAttributes::Load(const F4SESerializationInterface* serializer)
{
	UInt32 recType, length, curVersion;
	if (serializer->GetNextRecordInfo(&recType, &curVersion, &length))
	{
		if (recType != rootTag)
		{
			return FE("Invalid record type (%.4s). Expected (%.4s)", &recType, &rootTag);
		}
		return LoadInternal(serializer, curVersion);
	}
	return FE("ActorStringAttributes. Failed to get record.");
}

bool ActorStringAttributes::LoadInternal(const F4SESerializationInterface* serializer, UInt32 version)
{
	if (version != currentDataStructureVersion)
		return FE("Unexpected ActorStringAttributes version %d. Expected %d", version, currentDataStructureVersion);

	UInt32 actorsCount = 0;
	
	if (!serializer->ReadRecordData(&actorsCount, sizeof(actorsCount)))
	{
		return FE("Error reading actors string attributes map size");
	}
	for (UInt32 i = 0; i < actorsCount; i++)
	{
		UInt32 actorId;
		if (!serializer->ReadRecordData(&actorId, sizeof(actorId)))
			return FE("Error reading actor's ID for string attributes");


		UInt32 actualActorId;
		bool actorValid = true;
		std::unordered_map<UInt32, BSFixedString> attributes;

		if (!serializer->ResolveFormId(actorId, &actualActorId))
		{
			W("Failed to resolve Actor ID %08X", actorId);
			actorValid = false;
		}
		else
		{
			Actor* actor = DYNAMIC_CAST(LookupFormByID(actualActorId), TESForm, Actor);
			if (!actor)
			{
				W("Failed to find Actor with ID %08X", actualActorId);
				actorValid = false;
			}
		}

		UInt32 attributesCount;
		if (!serializer->ReadRecordData(&attributesCount, sizeof(attributesCount)))
			return FE("Failed to read string attributes size");

		for (int o = 0; o < attributesCount; o++)
		{
			UInt32 keywordId;
			if (!serializer->ReadRecordData(&keywordId, sizeof(keywordId)))
				return FE("Failed to read string attribute id");

			UInt32 actialKeywordId;
			bool attributeValid = true;
			if (!serializer->ResolveFormId(keywordId, &actialKeywordId))
			{
				W("Failed to resolve Keyword ID %08X", keywordId);
				attributeValid = false;
			}
			else
			{
				const BGSKeyword* keyword = DYNAMIC_CAST(LookupFormByID(actialKeywordId), TESForm, BGSKeyword);
				if (!keyword)
				{
					W("Failed to find Keyword with ID %08X", actialKeywordId);
					attributeValid = false;
				}
			}
			BSFixedString value;
			if (!SerializationHelper::ReadString(serializer, &value))
				return FE("Failed to read string attribute value");

			if (attributeValid)
				attributes.insert(std::make_pair(actialKeywordId, value));
		}

		if (actorValid && !attributes.empty())
			map.insert(std::make_pair(actualActorId, attributes));
	}
	return true;
}

void ActorStringAttributes::Clear()
{
	map.clear();
}


void ActorStringAttributes::DumpAll()
{
	Dmp("Actors string attributes:");
	for (auto& actorPair : map)
	{
		Dmp(" Actor %08X:", actorPair.first);
		for (auto& attrPair : actorPair.second)
		{
			Dmp("  %08X = %s", attrPair.first, attrPair.second.c_str());
		}
	}
}