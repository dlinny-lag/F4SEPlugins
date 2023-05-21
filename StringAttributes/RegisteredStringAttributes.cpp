#include "f4se/GameRTTI.h"

#include "RegisteredStringAttributes.h"
#include "SerializationHelper.h"
#include "Shared/MyTrace.h"
#include "DataTypeId.h"

constexpr UInt32 dataStructure1 = 1;
UInt32 RegisteredStringAttributes::currentDataStructureVersion = dataStructure1;

bool RegisteredStringAttributes::Add(BGSKeyword* attribute, BSFixedString& defaultValue)
{
	if (!attribute)
		return FD("Attribute not provided");
	D("Registering string attribute %08X wit default value [%s]", attribute->formID, defaultValue.c_str());
	return registeredAttributes.insert(std::make_pair(attribute, defaultValue)).second;
}
bool RegisteredStringAttributes::Remove(BGSKeyword* attribute)
{
	if (!attribute)
		return FD("Attribute not provided");
	D("Unregistering string attribute %08X", attribute->formID);
	return registeredAttributes.erase(attribute) > 0;
}

bool RegisteredStringAttributes::TryGet(BGSKeyword* attribute, BSFixedString* defaultValue) const
{
	if (!attribute)
		return FD("Attribute not provided");
	const auto pair = registeredAttributes.find(attribute);
	if (pair == registeredAttributes.end())
		return FD("Attribute not found");
	*defaultValue = pair->second;
	return true;
}

const UInt32 rootTag = RegisteredStringAttributesTypeId;
bool RegisteredStringAttributes::Save(const F4SESerializationInterface* serializer) const
{
	if (!serializer->OpenRecord(rootTag, RegisteredStringAttributes::currentDataStructureVersion))
	{
		return FE("Failed to open record (%.4s)", &rootTag);
	}

	const UInt32 size = registeredAttributes.size();
	if (!serializer->WriteRecordData(&size, sizeof(size)))
		return FE("Failed to write size of registered string attributes");

	for (auto& pair : registeredAttributes)
	{
		if (pair.first)
		{
			if (!serializer->WriteRecordData(&pair.first->formID, sizeof(pair.first->formID)))
				return FE("Failed to write string attribute ID");
			if (!SerializationHelper::WriteString(serializer, &pair.second))
				return FE("Failed to write string attribute default value");
		}
	}
	return true;
}

bool RegisteredStringAttributes::Load(const F4SESerializationInterface* serializer)
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
	return FE("RegisteredStringAttributes. Failed to get record.");
}

bool RegisteredStringAttributes::LoadInternal(const F4SESerializationInterface* serializer, UInt32 version)
{
	if (version != currentDataStructureVersion)
		return FE("Wrong SubscriptionsHolder version %d. Expected %d", version, currentDataStructureVersion);

	UInt32 size;
	if (!serializer->ReadRecordData(&size, sizeof(size)))
		return FE("Failed to read size of string attributes");

	for (UInt32 i = 0; i < size; i++)
	{
		UInt32 attrId, actualAttrId;
		BSFixedString defaultValue;
		if (!serializer->ReadRecordData(&attrId, sizeof(attrId)))
			return FE("Failed to read string attribute id");
		if (!SerializationHelper::ReadString(serializer, &defaultValue))
			return FE("Failed to read string attribute default value");

		if (!serializer->ResolveFormId(attrId, &actualAttrId))
		{
			I("Failed to resolve string Attribute (%08X)", attrId);
			continue; // skip if attribute is missing
		}
		BGSKeyword* attr = DYNAMIC_CAST(LookupFormByID(actualAttrId), TESForm, BGSKeyword);
		if (attr)
		{
			registeredAttributes.insert(std::make_pair(attr, defaultValue));
		}
		else // ignore invalid attribute... not sure how it could happens
			W("Invalid attribute with saved id=%d and resolved id=%d", attrId, actualAttrId);
	}
	return true;
}

void RegisteredStringAttributes::ForEach(const std::function<void(BGSKeyword*, BSFixedString&)>& functor)
{
	for (auto& pair : registeredAttributes)
	{
		if (pair.first)
			functor(pair.first, pair.second);
	}
}

void RegisteredStringAttributes::DumpAll() const
{
	Dmp("Registered string attributes");
	for (auto& pair : registeredAttributes)
	{
		Dmp("  %08X = %s", pair.first ? pair.first->formID : 0, pair.second.c_str());
	}
}

void RegisteredStringAttributes::Clear()
{
	registeredAttributes.clear();
}