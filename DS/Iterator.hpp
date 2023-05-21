#pragma once
#include <vector>
#include "Identifiable.h"
#include "MyTrace.h"

namespace DS
{
	// Identifiable has id of collection we iterate
	template <typename T>
	class Iterator : public Identifiable
	{
		UInt32 iteratorId;
		// collection data version
		UInt64 version;
		SInt32 position;
		std::vector<T> data;
		UInt32 collectionTypeId;
	public:
		static constexpr UInt32 InvalidID = 0;
		static constexpr UInt64 InvalidVersion = static_cast<UInt32>(-1);
		Iterator(BGSKeyword* collectionIdentifier, UInt32 typeId)
			:Identifiable(collectionIdentifier)
			,iteratorId(InvalidID)
			,version(InvalidVersion)
			,position(-1)
			,collectionTypeId(typeId)
		{

		}

		UInt32 IteratorId() const noexcept
		{
			return iteratorId;
		}


		template <typename CollectionType>	
		void Init(CollectionType& collection, const UInt32 id)
		{
			data.clear();
			collection.GetState(data, version);
			position = -1;
			iteratorId = id;
		}

		bool IsInvalid(UInt64 expectedVersion) const noexcept
		{
			return iteratorId == InvalidID || version == InvalidVersion || expectedVersion != version || FormId() == 0;
		}

		bool IsInvalid() const noexcept
		{
			return iteratorId == InvalidID || version == InvalidVersion || FormId() == 0;
		}


		bool IsFinished() const noexcept
		{
			return position >= static_cast<SInt32>(data.size()) - 1;
		}

		void Next()
		{
			++position;
		}

		// must be called before Get() to ensure that data is available
		bool Available() const noexcept
		{
			return position >= 0 && position < data.size();
		}
		UInt32 Position() const noexcept
		{
			return position;
		}
		// no checks!!!
		T Get() const
		{
			return data[position];
		}

		void Dump(int indent)
		{
			Dmp("%sIter %d(%d). Pos=%d of %d", Indent::Get(indent), iteratorId, version, position, data.size());
			indent += 2;
			for (auto& val : data)
			{
				ValueHelper::Dump(indent, val);
			}
		}

		bool Save(const F4SESerializationInterface* serializer) const
		{
			if (!SaveId(serializer, false))
				return FE("Failed to save collection id of iterator");

			if (!serializer->WriteRecordData(&collectionTypeId, sizeof(collectionTypeId)))
				return FE("Failed to save iterator's collection type id");

			if (!serializer->WriteRecordData(&iteratorId, sizeof(iteratorId)))
				return FE("Failed to save iterator id");
			if (!serializer->WriteRecordData(&version, sizeof(version)))
				return FE("Failed to save iterator version");

			UInt32 size = data.size();
			if (!serializer->WriteRecordData(&size, sizeof(size)))
				return FE("Failed to save iterator elements count");
			for (auto& val : data)
			{
				if (!ValueHelper::Save(serializer, &val))
					return FE("Failed to save iterator's element");
			}
			if (!serializer->WriteRecordData(&position, sizeof(position)))
				return FE("Failed to save iterator's position");

			return true;
		}
		bool Load(const F4SESerializationInterface* serializer)
		{
			UInt32 dictId;
			if (!LoadId(serializer, &dictId))
				return FE("Failed to load id of collection to iterate");

			UInt32 tId;
			if (!serializer->ReadRecordData(&tId, sizeof(tId)))
				return FE("Failed to load iterator's collection type id");
			if (tId != collectionTypeId)
			{
				E("Fatal: type mismatch in iterator. Expected %.4s but got %.4s", &collectionTypeId, &tId);
				throw "Type mismatch";
			}

			if (!serializer->ReadRecordData(&iteratorId, sizeof(iteratorId)))
				return FE("Failed to load iterator iterator id");

			if (!serializer->ReadRecordData(&version, sizeof(version)))
				return FE("Failed to load iterator version");

			UInt32 size;
			if (!serializer->ReadRecordData(&size, sizeof(size)))
				return FE("Failed to load iterator elements count");
			bool success = true;
			for (UInt32 i = 0; i < size; i++)
			{
				T val;
				if (!ValueHelper::Load(serializer, &val))
				{
					W("Failed to load iterator's element");
					success = false;
					continue;
				}
				if (success) // do not waste the CPU in case of failure
					data.push_back(val);
			}

			if (!serializer->ReadRecordData(&position, sizeof(position)))
				return FE("Failed to load iterator position");

			if (!success)
				return false;

			BGSKeyword* id = TryResolveId(serializer, dictId);
			SetId(id);
			return id != nullptr;
		}
	};
}