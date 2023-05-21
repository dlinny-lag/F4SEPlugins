#pragma once
#include "Set.hpp"
#include <unordered_map>
#include "CollectionCreator.hpp"

namespace DS
{
	template <typename T, class SetType>
	class SetsSet final
	{
		UInt32 structVersion;
		std::unordered_map<UInt32, SetType> sets;
		BSReadWriteLock setsLock;
	public:
		SetsSet(UInt32 structId, UInt32 structVersion)
			:structVersion(structVersion)
			,DataId(structId)
		{

		}
		const UInt32 DataId;

		bool CreateSet(BGSKeyword* identifier)
		{
			if (!identifier)
				return FD("CreateSet: invalid identifier");
			BSWriteLocker lock(&setsLock);
			SetType set(identifier);
			return sets.insert(std::make_pair(identifier->formID, set)).second;
		}

		bool DeleteSet(BGSKeyword* identifier)
		{
			if (!identifier)
				return FD("DeleteSet: invalid identifier");
			BSWriteLocker lock(&setsLock);
			return sets.erase(identifier->formID) > 0;
		}


		SInt32 Size(BGSKeyword* identifier)
		{
			if (!identifier)
			{
				D("Size: invalid identifier");
				return -1;
			}
			BSReadLocker lock(&setsLock);
			const auto pair = sets.find(identifier->formID);
			if (pair == sets.end())
				return -1;
			return pair->second.Length();
		}

		void Validate(BGSKeyword* identifier)
		{
			if (!identifier)
				return;
			BSReadLocker lock(&setsLock);
			const auto pair = sets.find(identifier->formID);
			if (pair == sets.end())
				return;
			pair->second.Validate();
		}

		bool Add(BGSKeyword* identifier, T& value)
		{
			if (!identifier)
				return FD("Add: invalid identifier");
			BSWriteLocker lock(&setsLock);
			const auto pair = sets.find(identifier->formID);
			if (pair == sets.end())
				return FD("Set %08X not found", identifier->formID);
			return pair->second.Add(value);
		}
		SInt32 AddRange(BGSKeyword* identifier, std::vector<T>& toAdd)
		{
			if (!identifier)
			{
				D("AddRange: invalid identifier");
				return -1;
			}
			BSWriteLocker lock(&setsLock);
			const auto pair = sets.find(identifier->formID);
			if (pair == sets.end())
			{
				D("AddRange: Set %08X not found", identifier->formID);
				return -1;
			}
			return pair->second.AddRange(toAdd);
		}

		SInt32 RemoveRange(BGSKeyword* identifier, std::vector<T>& toRemove)
		{
			if (!identifier)
			{
				D("RemoveRange: invalid identifier");
				return -1;
			}
			BSWriteLocker lock(&setsLock);
			const auto pair = sets.find(identifier->formID);
			if (pair == sets.end())
			{
				D("RemoveRange: Set %08X not found", identifier->formID);
				return -1;
			}
			return pair->second.RemoveRange(toRemove);
		}

		bool Intersect(BGSKeyword* identifier, std::unordered_set<T>& other)
		{
			if (!identifier)
			{
				return FD("Intersect: invalid identifier");
			}
			BSWriteLocker lock(&setsLock);
			const auto pair = sets.find(identifier->formID);
			if (pair == sets.end())
				return FD("Intersect: Set %08X not found", identifier->formID);
			return pair->second.Intersect(other);
		}
		bool Contains(BGSKeyword* identifier, T& value)
		{
			if (!identifier)
				return FD("Contains: invalid identifier");
			BSReadLocker lock(&setsLock);
			const auto pair = sets.find(identifier->formID);
			if (pair == sets.end())
				return false;
			return pair->second.Contains(value);
		}
		bool Remove(BGSKeyword* identifier, T& value)
		{
			if (!identifier)
				return FD("Remove: invalid identifier");
			BSWriteLocker lock(&setsLock);
			const auto pair = sets.find(identifier->formID);
			if (pair == sets.end())
				return FD("Remove: Set %08X not found", identifier->formID);
			return pair->second.Remove(value);
		}
		void Clear()
		{
			BSWriteLocker lock(&setsLock);
			sets.clear();
		}

		bool Save(const F4SESerializationInterface* serializer)
		{
			if (!serializer->OpenRecord(DataId, structVersion))
				return FE("Failed to oped record (%.4s)", DataId);

			const UInt32 len = sets.size();
			if (!serializer->WriteRecordData(&len, sizeof(len)))
				return FE("Failed to write length of sets");
			for (auto& set : sets)
			{
				if (!CollectionCreator<SetType>::SaveHeader(serializer, set.second))
					return FE("Failed to save set header");
				if (!set.second.SaveData(serializer, false))
					return FE("Failed to save set data");
			}
			return true;
		}

		bool Load(const F4SESerializationInterface* serializer, UInt32 version, UInt32 length)
		{
			if (version != structVersion)
				return FE("Unexpected Sets version %d. Expected %d", version, structVersion);
			UInt32 len;
			if (!serializer->ReadRecordData(&len, sizeof(len)))
				return FE("Failed to read sets length");
			D("Reading %d sets", len);
			for (UInt32 i = 0; i < len; i++)
			{
				bool success;
				D("Loading set #%d...", i);
				SetType set = CollectionCreator<SetType>::Create(serializer, success);
				if (!set.LoadData(serializer, false))
				{
					W("Failed to read set");
					continue; // most likely data is missing. ignore invalid set
				}
				if (success)
					sets.insert(std::make_pair(set.FormId(), set));
			}
			return true;
		}

		std::unordered_map<UInt32, UInt64> GetVersionsMap()
		{
			BSReadLocker lock(&setsLock);
			std::unordered_map<UInt32, UInt64> retVal;
			for (auto& set : sets)
			{
				retVal.insert(std::make_pair(set.first, set.second.Version()));
			}
			return retVal;
		}

		template <typename TResult>
		bool ExecuteOn(BGSKeyword* identifier, const std::function<TResult(SetType&)>& functor, TResult& result)
		{
			if (!identifier)
				return FD("Sets: ExecuteOn: invalid identifier");
			BSReadLocker lock(&setsLock); // TODO: write lock?
			const auto pair = sets.find(identifier->formID);
			if (pair == sets.end())
				return false;
			result = functor(pair->second);
			return true;
		}


		void DumpAll(int indent)
		{
			BSReadLocker lock(&setsLock);
			for (auto& set : sets)
			{
				set.second.DumpAll(indent);
			}
		}
		void Dump(BGSKeyword* identifier, int indent)
		{
			if (!identifier)
				return;
			BSReadLocker lock(&setsLock);
			const auto pair = sets.find(identifier->formID);
			if (pair != sets.end())
				pair->second.DumpAll(indent);
		}

		bool CopyToArray(BGSKeyword* identifier, BGSKeyword* arrayId, std::function<bool(BGSKeyword* , std::vector<T>&)>& assign)
		{
			if (!identifier)
				return FD("CopyToArray: invalid identifier");
			if (!arrayId)
				return FD("CopyToArray: invalid array identifier");

			BSReadLocker lock(&setsLock);
			const auto pair = sets.find(identifier->formID);
			if (pair == sets.end())
				return FD("CopyToArray: Set %08X not found", identifier->formID);
			std::vector<T> data;
			UInt64 version;
			pair->second.GetState(data, version);
			return assign(arrayId, data);
		}

		SInt32 AddArray(BGSKeyword* identifier, BGSKeyword* arrayId, std::function<bool(BGSKeyword*, std::vector<T>&)>& get)
		{
			if (!identifier)
			{
				D("AddArray: invalid identifier");
				return -1;
			}
			if (!arrayId)
			{
				D("AddArray: invalid array identifier");
				return -1;
			}
			BSWriteLocker lock(&setsLock);
			const auto pair = sets.find(identifier->formID);
			if (pair == sets.end())
			{
				D("AddArray: Set %08X not found", identifier->formID);
				return -1;
			}

			std::vector<T> toAdd;
			if (get(arrayId, toAdd))
				return pair->second.AddRange(toAdd);

			return -1;
		}
		SInt32 AddSet(BGSKeyword* identifier, BGSKeyword* addingSet)
		{
			if (!identifier)
			{
				D("AddSet: invalid identifier");
				return -1;
			}
			if (!addingSet)
			{
				D("AddSet: invalid adding set identifier");
				return -1;
			}
			BSWriteLocker lock(&setsLock);
			const auto pairTo = sets.find(identifier->formID);
			if (pairTo == sets.end())
			{
				D("AddSet: Set %08X not found", identifier->formID);
				return -1;
			}
			const auto pairFrom = sets.find(addingSet->formID);
			if (pairFrom == sets.end())
			{
				D("AddSet: adding set %08X not found", addingSet->formID);
				return -1;
			}
			std::vector<T> toAdd;
			UInt64 version;
			pairFrom->second.GetState(toAdd, version);
			return pairTo->second.AddRange(toAdd);
		}

		bool IntersectArray(BGSKeyword* identifier, BGSKeyword* arrayId, std::function<bool(BGSKeyword*, std::vector<T>&)>& get)
		{
			if (!identifier)
				return FD("IntersectArray: invalid identifier");
			if (!arrayId)
				return FD("IntersectArray: invalid array identifier");

			BSWriteLocker lock(&setsLock);

			const auto pair = sets.find(identifier->formID);
			if (pair == sets.end())
				return FD("IntersectArray: set %08X not found", identifier->formID);

			std::vector<T> array;
			if (get(arrayId, array))
			{
				std::unordered_set<T> other(array.begin(), array.end());
				return pair->second.Intersect(other);
			}

			return false;
		}
		bool IntersectSet(BGSKeyword* identifier, BGSKeyword* otherSet)
		{
			if (!identifier)
				return FD("IntersectSet: invalid identifier");
			if (!otherSet)
				return FD("IntersectSet: invalid other set identifier");

			BSWriteLocker lock(&setsLock);

			const auto pairTo = sets.find(identifier->formID);
			if (pairTo == sets.end())
				return FD("IntersectSet: set %08X not found", identifier->formID);

			const auto pairFrom = sets.find(otherSet->formID);
			if (pairFrom == sets.end())
				return FD("IntersectSet: other set %08X not found", otherSet->formID);

			std::unordered_set<T> other;
			pairFrom->second.GetContent(other);
			return pairTo->second.Intersect(other);
		}

		SInt32 RemoveArray(BGSKeyword* identifier, BGSKeyword* arrayId, std::function<bool(BGSKeyword*, std::vector<T>&)>& get)
		{
			if (!identifier)
				return FD("RemoveArray: invalid identifier");
			if (!arrayId)
				return FD("RemoveArray: invalid array identifier");

			BSWriteLocker lock(&setsLock);

			const auto pair = sets.find(identifier->formID);
			if (pair == sets.end())
				return FD("IntersectArray: set %08X not found", identifier->formID);

			std::vector<T> array;
			if (get(arrayId, array))
				return pair->second.RemoveRange(array);

			return false;
		}

		bool RemoveSet(BGSKeyword* identifier, BGSKeyword* setId)
		{
			if (!identifier)
				return FD("RemoveSet: invalid identifier");
			if (!setId)
				return FD("RemoveSet: invalid other set identifier");

			BSWriteLocker lock(&setsLock);

			const auto pairTo = sets.find(identifier->formID);
			if (pairTo == sets.end())
				return FD("RemoveSet: set %08X not found", identifier->formID);

			const auto pairFrom = sets.find(setId->formID);
			if (pairFrom == sets.end())
				return FD("RemoveSet: other set %08X not found", setId->formID);

			std::vector<T> array;
			UInt64 version;
			pairFrom->second.GetState(array, version);
			return pairTo->second.RemoveRange(array);
		}

		bool SetSet(BGSKeyword* identifier, std::vector<T>& elements)
		{
			if (!identifier)
				return FD("SetSet: invalid identifier");
			BSWriteLocker lock(&setsLock);
			const auto pair = sets.find(identifier->formID);
			if (pair == sets.end())
			{
				// create new and populate
				SetType set(identifier);
				set.AddRange(elements);
				sets.insert(std::make_pair(identifier->formID, set));
				return true;
			}
			if (pair->second.Length() > 0)
				return FW("SetSet: target is not empty");
			pair->second.AddRange(elements);
			return true;
		}

		bool GetSet(BGSKeyword* identifier, std::vector<T>& elements)
		{
			if (!identifier)
				return FD("GetSet: invalid identifier");
			BSReadLocker lock(&setsLock);
			const auto pair = sets.find(identifier->formID);
			if (pair != sets.end())
			{
				UInt64 version;
				pair->second.GetState(elements, version);
				return true;
			}
			return FD("GetSet: set %08X not found", identifier->formID);
		}

		UInt32 Clear(BGSKeyword* identifier)
		{
			if (!identifier)
			{
				D("Sets.Clear: invalid identifier");
				return -1;
			}
			BSReadLocker lock(&setsLock);
			const auto pair = sets.find(identifier->formID);
			if (pair != sets.end())
			{
				return pair->second.Clear();
			}
			D("Clear: set %08X not found", identifier->formID);
			return -1;

		}
	};
}