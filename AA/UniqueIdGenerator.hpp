#pragma once

#include "MyTrace.h"
#include "f4se/PluginAPI.h"
#include "f4se/PapyrusVM.h"
#include "f4se/PapyrusNativeFunctions.h"
#include <unordered_map>
#include "f4se/GameRTTI.h"

namespace UniqueIdGenerator
{
	constexpr SInt32 initialValue = 10000;

	BSReadWriteLock idLock;
	SInt32 value = initialValue;
	std::unordered_map<UInt32, SInt32> uniqueIdsMap;

	constexpr UInt32 rootTag = 'UNIQ';
	bool Save(const F4SESerializationInterface* serializer)
	{
		if (!serializer->OpenRecord(rootTag, 1))
			return FE("Failed to open record (%.4s)", &rootTag);
		if (!serializer->WriteRecordData(&value, sizeof(SInt32)))
			return FE("Failed to write id");
		const UInt32 size = uniqueIdsMap.size();
		if (!serializer->WriteRecordData(&size, sizeof(UInt32)))
			return FE("Failed to write unique ids size");
		for (const auto [formId, uniqId] : uniqueIdsMap)
		{
			if (!serializer->WriteRecordData(&formId, sizeof(UInt32)))
				return FE("Failed to write FormId");
			if (!serializer->WriteRecordData(&uniqId, sizeof(SInt32)))
				return FE("Failed to write UniqId");
		}
		return true;
	}
	bool Load(const F4SESerializationInterface* serializer)
	{
		UInt32 recType, length, curVersion;
		if (!serializer->GetNextRecordInfo(&recType, &curVersion, &length))
			return FE("UniqueIdGenerator. Failed to get record.");

		if (recType != rootTag)
			return FE("Invalid record type (%.4s). Expected (%.4s)", &recType, &rootTag);
		if (!serializer->ReadRecordData(&value, sizeof(value)))
			return FE("Failed to read id");
		UInt32 size;
		if (!serializer->ReadRecordData(&size, sizeof(UInt32)))
			return FE("Failed to read unique ids size");

		for (UInt32 i = 0; i < size; ++i)
		{
			UInt32 formId, actualFormId;
			SInt32 uniqId;
			if (!serializer->ReadRecordData(&formId, sizeof(UInt32)))
				return FE("Failed to read FormId");
			if (!serializer->ReadRecordData(&uniqId, sizeof(SInt32)))
				return FE("Failed to read UniqId");

			if (!serializer->ResolveFormId(formId, &actualFormId))
				continue;
			Actor* actor = DYNAMIC_CAST(LookupFormByID(actualFormId), TESForm, Actor);
			if (!actor)
				continue;
			uniqueIdsMap.insert({ actualFormId, uniqId });
		}
		return true;
	}
	void Clear()
	{
		uniqueIdsMap.clear();
		value = initialValue;
	}

	SInt32 GetUniqueId(StaticFunctionTag* _, Actor* actor)
	{
		if (!actor)
			return 0;
		BSWriteLocker lock(&idLock);
		const auto ptr = uniqueIdsMap.find(actor->formID);
		if (ptr == uniqueIdsMap.end())
		{
			++value;
			uniqueIdsMap.insert({ actor->formID, value });
			D("Actor %08X: new Id=%d", actor->formID, value);
			return value;
		}
		D("Actor %08X: Id=%d", actor->formID, ptr->second);
		return ptr->second;
	}

	bool Register(VirtualMachine* vm)
	{
		vm->RegisterFunction(
			new NativeFunction1<StaticFunctionTag, SInt32, Actor*>("GetUniqueId", AttributesAPIExport::EXPORT_PAPYRUS_SCRIPT, GetUniqueId, vm));
		vm->SetFunctionFlags(AttributesAPIExport::EXPORT_PAPYRUS_SCRIPT, "GetUniqueId", IFunction::kFunctionFlag_NoWait);
		return true;
	}

}
