#pragma once
#include "CollectionCreator.hpp"
#include "StructArrayDict.hpp"

template <typename TKey>
struct CollectionCreator<DS::StructArrayDict<TKey>>
{
	static DS::StructArrayDict<TKey> Create(const F4SESerializationInterface* serializer, bool& success)
	{
		UInt32 arrayId;
		success = Identifiable::LoadId(serializer, &arrayId);

		if (!success)
			return DS::StructArrayDict<TKey>(nullptr, "");

		BSFixedString structName;
		if (!SerializationHelper::ReadString(serializer, &structName))
		{
			E("StructArrayCreator: Failed to read struct name");
			success = false;
		}

		if (!success)
			return DS::StructArrayDict<TKey>(nullptr, "");

		BGSKeyword* id = Identifiable::TryResolveId(serializer, arrayId);
		success = id != nullptr;
		return DS::StructArrayDict<TKey>(id, structName);
	}
	static bool SaveHeader(const F4SESerializationInterface* serializer, DS::StructArrayDict<TKey>& collection)
	{
		if (!collection.SaveId(serializer, false))
			return FE("Failed to save collection's id %08X", collection.FormId());
		BSFixedString structName(collection.StructName());
		bool retVal = SerializationHelper::WriteString(serializer, &structName);
		structName.Release();
		return retVal;
	}
};