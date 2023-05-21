#pragma once
#include "Identifiable.h"
#include "f4se/GameRTTI.h"

template <typename CollectionType>
struct CollectionCreator
{
	static CollectionType Create(const F4SESerializationInterface* serializer, bool& success)
	{
		UInt32 collectionId;
		success = Identifiable::LoadId(serializer, &collectionId);
		if (!success)
		{
			return CollectionType(nullptr);
		}
		BGSKeyword* id = Identifiable::TryResolveId(serializer, collectionId);
		success = id != nullptr;
		return CollectionType(id);
	}
	static bool SaveHeader(const F4SESerializationInterface* serializer, CollectionType& collection)
	{
		if (!collection.SaveId(serializer, false))
			return FE("Failed to save collection's id %08X", collection.FormId());
		return true;
	}
};
