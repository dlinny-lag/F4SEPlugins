#pragma once
#include "CollectionCreator.hpp"
#include "StructArray.hpp"

template<>
struct CollectionCreator<DS::StructArray>
{
	static DS::StructArray Create(const F4SESerializationInterface* serializer, bool& success)
	{
		UInt32 arrayId;
		success = Identifiable::LoadId(serializer, &arrayId);

		if (!success)
			return DS::StructArray(nullptr, "");

		BSFixedString structName;
		if (!SerializationHelper::ReadString(serializer, &structName))
		{
			E("StructArrayCreator: Failed to read struct name");
			success = false;
		}

		if (!success)
			return DS::StructArray(nullptr, "");
		
		BGSKeyword* id = Identifiable::TryResolveId(serializer, arrayId);
		success = id != nullptr;
		return DS::StructArray(id, structName);
	}
	static bool SaveHeader(const F4SESerializationInterface* serializer, DS::StructArray& array)
	{
		if (!array.SaveId(serializer, false))
			return FE("Failed to save array id %08X", array.FormId());
		BSFixedString structName(array.StructName());
		bool retVal = SerializationHelper::WriteString(serializer, &structName);
		structName.Release();
		return retVal;
	}
};

