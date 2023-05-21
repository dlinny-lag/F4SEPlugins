#pragma once
#include "CollectionCreator.hpp"
#include "StructValueDict.hpp"

template <typename TKey>
struct CollectionCreator<DS::StructValueDict<TKey>>
{
	static DS::StructValueDict<TKey> Create(const F4SESerializationInterface* serializer, bool& success)
	{
		UInt32 arrayId;
		success = Identifiable::LoadId(serializer, &arrayId);

		if (!success)
			return DS::StructValueDict<TKey>(nullptr, "");

		BSFixedString structName;
		if (!SerializationHelper::ReadString(serializer, &structName))
		{
			E("StructArrayCreator: Failed to read struct name");
			success = false;
		}

		if (!success)
			return DS::StructValueDict<TKey>(nullptr, "");

		BGSKeyword* id = Identifiable::TryResolveId(serializer, arrayId);
		success = id != nullptr;
		return DS::StructValueDict<TKey>(id, structName);
	}
	static bool SaveHeader(const F4SESerializationInterface* serializer, DS::StructValueDict<TKey>& collection)
	{
		if (!collection.SaveId(serializer, false))
			return FE("Failed to save collection's id %08X", collection.FormId());
		BSFixedString structName(collection.StructName());
		bool retVal = SerializationHelper::WriteString(serializer, &structName);
		structName.Release();
		return retVal;
	}
};
