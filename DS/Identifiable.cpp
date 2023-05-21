#include "Identifiable.h"
#include "MyTrace.h"
#include "f4se/GameRTTI.h"


void Identifiable::SetId(BGSKeyword* id)
{
	identifier = id;
}

BGSKeyword* Identifiable::Id() const noexcept
{
	return identifier;
}
UInt32 Identifiable::FormId() const noexcept
{
	return identifier ? identifier->formID : 0;
}

bool Identifiable::SaveId(const F4SESerializationInterface* serializer, bool allowNoId) const
{
	const UInt32 id = identifier ? identifier->formID: 0;
	if (id == 0 && !allowNoId)
		return FE("invalid identifier");

	if (!serializer->WriteRecordData(&id, sizeof(id)))
		return FE("Failed to write id=(%08X)", id);
	return true;
}

bool Identifiable::LoadId(const F4SESerializationInterface* serializer, UInt32 * outId)
{
	if (!serializer->ReadRecordData(outId, sizeof(UInt32)))
		return FE("Identifiable: Failed to read identifier");
	return true;
}

BGSKeyword* Identifiable::TryResolveId(const F4SESerializationInterface* serializer, const UInt32 id)
{
	UInt32 actualId;

	if (!serializer->ResolveFormId(id, &actualId))
	{
		W("Identifiable: Identifier %08X not resolved ", id);
		return nullptr;
	}
	TESForm* f = LookupFormByID(actualId);
	if (!f)
	{
		W("Identifiable: identifier %08X not found", actualId);
		return nullptr;
	}
	BGSKeyword* retVal = DYNAMIC_CAST(f, TESForm, BGSKeyword);
	if (!retVal)
	{
		W("Identifiable: invalid identifier with saved id=%08X and resolved id=%08X", id, actualId);
	}
	return retVal;
}



