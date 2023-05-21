#pragma once

#include "Array.hpp"
#include <unordered_map>
#include "StructArrayCreator.hpp"
#include <optional>

namespace DS
{
	template <typename T, class ArrayType, UInt32 structId, UInt32 structVersion = 1>
	class ArraysSet
	{
	protected:
		std::unordered_map<UInt32, ArrayType> arrays;
		BSReadWriteLock arraysLock;
	public:
		ArraysSet()
			:arrays()
			,arraysLock()
			,DataId(structId)
		{
			
		}
		virtual ~ArraysSet() = default;

		const UInt32 DataId;

		bool DeleteArray(BGSKeyword* identifier)
		{
			if (!identifier)
				return FD("DeleteArray: invalid identifier");
			BSWriteLocker lock(&arraysLock);
			return arrays.erase(identifier->formID) > 0;
		}

		template <typename TResult>
		bool ExecuteOn(BGSKeyword* identifier, const std::function<TResult(ArrayType&)>& functor, TResult& result)
		{
			if (!identifier)
				return FD("Arrays:ExecuteOn: invalid identifier");
			BSReadLocker lock(&arraysLock); // TODO: write lock?
			const auto pair = arrays.find(identifier->formID);
			if (pair == arrays.end())
				return false;
			result = functor(pair->second);
			return true;
		}

		SInt32 Size(BGSKeyword* identifier)
		{
			if (!identifier)
			{
				D("Size: invalid identifier");
				return -1;
			}
			BSReadLocker lock(&arraysLock);
			const auto pair = arrays.find(identifier->formID);
			if (pair == arrays.end())
			{
				D("Size: no array found");
				return -1;
			}
			return pair->second.Length();
		}

		bool Add(BGSKeyword* identifier, T& value)
		{
			if (!identifier)
				return FD("Add: invalid identifier");
			BSWriteLocker lock(&arraysLock);
			const auto pair = arrays.find(identifier->formID);
			if (pair == arrays.end())
				return FD("Add: no array found");
			pair->second.Add(value);
			return true;
		}
		bool AddRange(BGSKeyword* identifier, const std::vector<T>& toAdd)
		{
			if (!identifier)
				return FD("AddRange: invalid identifier");
			BSWriteLocker lock(&arraysLock);
			const auto pair = arrays.find(identifier->formID);
			if (pair == arrays.end())
				return FD("AddRange: no array found");
			pair->second.AddRange(toAdd);
			return true;
		}

		bool AppendArray(BGSKeyword* identifier, BGSKeyword* toAdd)
		{
			if (!identifier)
				return FD("AppendArray: invalid identifier");
			if (!toAdd)
				return FD("AppendArray: invalid identifier for adding array");
			BSWriteLocker lock(&arraysLock);
			const auto pair = arrays.find(identifier->formID);
			if (pair == arrays.end())
				return FD("AddRange: no array found");

			const auto toAddPair = arrays.find(toAdd->formID);
			if (toAddPair == arrays.end())
				return FD("AddRange: no array to add found");

			std::vector<T> toAddVector;
			UInt64 version;
			toAddPair->second.GetState(toAddVector, version);
			pair->second.AddRange(toAddVector);
			return true;
		}

		std::optional<T> Get(BGSKeyword* identifier, SInt32 index)
		{
			if (!identifier)
			{
				D("Get: invalid identifier");
				return std::nullopt;
			}
			BSReadLocker lock(&arraysLock);
			const auto pair = arrays.find(identifier->formID);
			if (pair == arrays.end())
				return std::nullopt;
			return pair->second.Get(index);
		}

		bool Set(BGSKeyword* identifier, SInt32 index, T& value)
		{
			if (!identifier)
				return FD("Set: invalid identifier");
			BSWriteLocker lock(&arraysLock);
			const auto pair = arrays.find(identifier->formID);
			if (pair == arrays.end())
				return FD("Set: no array found");
			return pair->second.Set(index, value);
		}

		bool Insert(BGSKeyword* identifier, T& value, SInt32 index)
		{
			if (!identifier)
				return FD("Insert: invalid identifier");
			BSWriteLocker lock(&arraysLock);
			const auto pair = arrays.find(identifier->formID);
			if (pair == arrays.end())
				return FD("Insert: no array found");
			return pair->second.Insert(value, index);
		}

		bool Remove(BGSKeyword* identifier, SInt32 index)
		{
			if (!identifier)
				return FD("Remove: invalid identifier");
			BSWriteLocker lock(&arraysLock);
			const auto pair = arrays.find(identifier->formID);
			if (pair == arrays.end())
				return FD("Remove: no array found");
			return pair->second.Remove(index);
		}

		void Clear()
		{
			BSWriteLocker lock(&arraysLock);
			arrays.clear();
		}

		UInt32 Clear(BGSKeyword* identifier)
		{
			if (!identifier)
				return FD("Arrays.Clear: invalid identifier");
			BSReadLocker lock(&arraysLock);
			const auto pair = arrays.find(identifier->formID);
			if (pair != arrays.end())
			{
				return pair->second.Clear();
			}
			return FD("Clear: array %08X not found", identifier->formID);
		}
		bool Save(const F4SESerializationInterface* serializer)
		{
			if (!serializer->OpenRecord(structId, structVersion))
				return FE("Failed to oped record (%.4s)", structId);

			const UInt32 len = arrays.size();
			if (!serializer->WriteRecordData(&len, sizeof(len)))
				return FE("Failed to write length of arrays");
			for (auto& arr : arrays)
			{
				if (!CollectionCreator<ArrayType>::SaveHeader(serializer, arr.second))
					return FE("Failed to save array header");
				if (!arr.second.SaveData(serializer, false))
					return FE("Failed to save array data");
			}
			return true;
		}
		bool Load(const F4SESerializationInterface* serializer, UInt32 version, UInt32 length)
		{
			if (version != structVersion)
				return FE("Unexpected Arrays version %d. Expected %d", version, structVersion);
			UInt32 len;
			if (!serializer->ReadRecordData(&len, sizeof(len)))
				return FE("Failed to read arrays length");
			for (UInt32 i = 0; i < len; i++)
			{
				bool success = false;
				ArrayType arr = CollectionCreator<ArrayType>::Create(serializer, success);
				if (!arr.LoadData(serializer))
				{
					W("Failed to read array");
					continue; // most likely data is missing. ignore invalid array
				}
				if (success)
					arrays.insert(std::make_pair(arr.FormId(), arr));
			}
			return true;
		}

		void DumpAll(int indent)
		{
			BSReadLocker lock(&arraysLock);
			for (auto& arr : arrays)
			{
				arr.second.DumpAll(indent);
			}
		}
		void Dump(BGSKeyword* identifier, int indent)
		{
			if (!identifier)
			{
				D("Dump: invalid identifier");
				return;
			}
			BSReadLocker lock(&arraysLock);
			auto pair = arrays.find(identifier->formID);
			if (pair == arrays.end())
			{
				D("No array found");
				return;
			}
			pair->second.DumpAll(indent);
		}
	};
}