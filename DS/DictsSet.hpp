#pragma once
#include "CollectionCreator.hpp"
#include <optional>

namespace DS
{
	template <typename TKey, typename TValue, class DictType>
	class DictsSet
	{
	private:
		UInt32 structVersion;
	protected:
		std::unordered_map<UInt32, DictType> dicts;
		BSReadWriteLock dictsLock;
	public:

		DictsSet(UInt32 structId, UInt32 structVersion)
			:structVersion(structVersion)
			,DataId(structId)
		{

		}
		const UInt32 DataId;

		bool DeleteDict(BGSKeyword* identifier)
		{
			if (!identifier)
				return FD("DeleteDict: invalid identifier");
			BSWriteLocker lock(&dictsLock);
			return dicts.erase(identifier->formID) > 0;
		}

		SInt32 Size(BGSKeyword* identifier)
		{
			if (!identifier)
			{
				D("Size: invalid identifier");
				return -1;
			}
			BSReadLocker lock(&dictsLock);
			const auto pair = dicts.find(identifier->formID);
			if (pair == dicts.end())
				return -1;
			return pair->second.Size();
		}

		void Validate(BGSKeyword* identifier)
		{
			if (!identifier)
				return;
			BSReadLocker lock(&dictsLock);
			const auto pair = dicts.find(identifier->formID);
			if (pair == dicts.end())
				return;
			pair->second.Validate();
		}

		bool Add(BGSKeyword* identifier, TKey& key, TValue& value)
		{
			if (!identifier)
			{
				return FD("Get: invalid identifier");
			}
			BSWriteLocker lock(&dictsLock);
			const auto pair = dicts.find(identifier->formID);
			if (pair == dicts.end())
				return false;
			return pair->second.Add(key, value);
		}

		bool Set(BGSKeyword* identifier, TKey& key, TValue& value)
		{
			if (!identifier)
			{
				return FD("Set: invalid identifier");
			}
			BSWriteLocker lock(&dictsLock);
			const auto pair = dicts.find(identifier->formID);
			if (pair == dicts.end())
				return FD("Set: no dict found");
			return pair->second.Set(key, value);
		}

		bool Replace(BGSKeyword* identifier, TKey& key, TValue& newValue)
		{
			if (!identifier)
			{
				return FD("Set: invalid identifier");
			}
			BSWriteLocker lock(&dictsLock);
			const auto pair = dicts.find(identifier->formID);
			if (pair == dicts.end())
				return FD("Set: no dict found");
			return pair->second.Replace(key, newValue);
		}
		
		std::optional<TValue> Get(BGSKeyword* identifier, TKey& key)
		{
			if (!identifier)
			{
				D("Get: invalid identifier");
				return std::nullopt;
			}
			BSReadLocker lock(&dictsLock);
			const auto pair = dicts.find(identifier->formID);
			if (pair == dicts.end())
				return std::nullopt;
			return pair->second.Get(key);
		}


		bool Contains(BGSKeyword* identifier, TKey& key)
		{
			if (!identifier)
				return FD("Contains: invalid identifier");
			BSReadLocker lock(&dictsLock);
			const auto pair = dicts.find(identifier->formID);
			if (pair == dicts.end())
				return false;
			return pair->second.Contains(key);
		}
		bool Remove(BGSKeyword* identifier, TKey& key)
		{
			if (!identifier)
				return FD("Remove: invalid identifier");
			BSWriteLocker lock(&dictsLock);
			const auto pair = dicts.find(identifier->formID);
			if (pair == dicts.end())
				return false;
			return pair->second.Remove(key);
		}

		void Clear()
		{
			BSWriteLocker lock(&dictsLock);
			dicts.clear();
		}

		bool Save(const F4SESerializationInterface* serializer)
		{
			if (!serializer->OpenRecord(DataId, structVersion))
				return FE("Failed to oped record (%.4s)", DataId);

			const UInt32 len = dicts.size();
			if (!serializer->WriteRecordData(&len, sizeof(len)))
				return FE("Failed to write length of dicts");
			for (auto& pair : dicts)
			{
				if (!CollectionCreator<DictType>::SaveHeader(serializer, pair.second))
					return FE("Failed to save dict header");
				if (!pair.second.SaveData(serializer, false))
					return FE("Failed to save dict data");
			}
			return true;
		}
		bool Load(const F4SESerializationInterface* serializer, UInt32 version, UInt32 length)
		{
			if (version != structVersion)
				return FE("Unexpected Dicts version %d. Expected %d", version, structVersion);
			D("Reading %d bytes...", length);
			UInt32 len;
			if (!serializer->ReadRecordData(&len, sizeof(len)))
				return FE("Failed to read dicts length");
			for (UInt32 i = 0; i < len; i++)
			{
				bool success = false;
				DictType dict = CollectionCreator<DictType>::Create(serializer, success);
				if (!dict.LoadData(serializer, false))
				{
					W("Failed to read dict");
					continue; // most likely data is missing. ignore invalid dict
				}
				if (success)
					dicts.insert(std::make_pair(dict.FormId(), dict));
			}
			return true;
		}

		void DumpAll(int indent)
		{
			BSReadLocker lock(&dictsLock);
			for (auto& pair : dicts)
			{
				pair.second.DumpAll(indent);
			}
		}

		void Dump(BGSKeyword* identifier, int indent)
		{
			if (!identifier)
				return;
			BSReadLocker lock(&dictsLock);
			const auto pair = dicts.find(identifier->formID);
			if (pair == dicts.end())
				return;
			pair->second.DumpAll(indent);
		}

		std::unordered_map<UInt32, UInt64> GetVersionsMap()
		{
			std::unordered_map<UInt32, UInt64> retVal;
			BSReadLocker lock(&dictsLock);
			for (auto& dict : dicts)
			{
				retVal.insert(std::make_pair(dict.first, dict.second.Version()));
			}
			return retVal;
		}

		std::optional<std::vector<TKey>> KeysToArray(BGSKeyword* identifier)
		{
			BSReadLocker lock(&dictsLock);
			const auto pair = dicts.find(identifier->formID);
			if (pair == dicts.end())
			{
				D("KeysToArray: Dict %08X not found", identifier->formID);
				return std::nullopt;
			}
			std::vector<TKey> data;
			UInt64 version;
			pair->second.GetState(data, version);
			return data;
		}

		template <typename TResult>
		bool ExecuteOn(BGSKeyword* identifier, const std::function<TResult(DictType&)>& functor, TResult& result)
		{
			if (!identifier)
				return FD("ExecuteOnKey: invalid identifier");
			BSReadLocker lock(&dictsLock); // TODO: write lock?
			const auto pair = dicts.find(identifier->formID);
			if (pair == dicts.end())
				return false;
			result = functor(pair->second);
			return true;
		}

		bool CopyKeysToArray(BGSKeyword* identifier, BGSKeyword* arrayId, std::function<bool(BGSKeyword*, std::vector<TKey>&)>& assign)
		{
			if (!identifier)
				return FD("CopyKeysToArray: invalid identifier");
			if (!arrayId)
				return FD("CopyKeysToArray: invalid array identifier");
			BSReadLocker lock(&dictsLock);
			const auto pair = dicts.find(identifier->formID);
			if (pair == dicts.end())
				return FD("CopyKeysToArray: Dict %08X not found", identifier->formID);
			std::vector<TKey> data;
			UInt64 version;
			pair->second.GetState(data, version);
			return assign(arrayId, data);
		}
		bool CopyKeysToSet(BGSKeyword* identifier, BGSKeyword* setId, std::function<bool(BGSKeyword*, std::vector<TKey>&)>& assign)
		{
			if (!identifier)
				return FD("CopyKeysToSet: invalid identifier");
			if (!setId)
				return FD("CopyKeysToSet: invalid set identifier");

			std::vector<TKey> data;
			{ // critical section
				BSReadLocker lock(&dictsLock);
				const auto pair = dicts.find(identifier->formID);
				if (pair == dicts.end())
					return FD("CopyKeysToSet: Dict %08X not found", identifier->formID);
				UInt64 version;
				pair->second.GetState(data, version);
			}
			return assign(setId, data);
		}

		SInt32 RemoveKeysByArray(BGSKeyword* identifier, BGSKeyword* arrayId, std::function<bool(BGSKeyword*, std::vector<TKey>&)>& get)
		{
			if (!identifier)
			{
				D("RemoveKeysByArray: invalid identifier");
				return -1;
			}
			if (!arrayId)
			{
				D("RemoveKeysByArray: invalid array identifier");
				return -1;
			}
			BSWriteLocker lock(&dictsLock);
			const auto pair = dicts.find(identifier->formID);
			if (pair == dicts.end())
			{
				D("RemoveKeysByArray: Dict %08X not found", identifier->formID);
				return -1;
			}
			std::vector<TKey> keysToRemove;
			if (get(arrayId, keysToRemove))
			{
				SInt32 retVal = 0;
				for (auto& key : keysToRemove)
				{
					retVal += pair->second.Remove(key);
				}
				return retVal;
			}
			return -1;
		}
		SInt32 RemoveKeysBySet(BGSKeyword* identifier, BGSKeyword* setId, std::function<bool(BGSKeyword*, std::vector<TKey>&)>& get)
		{
			if (!identifier)
			{
				D("RemoveKeysBySet: invalid identifier");
				return -1;
			}
			if (!setId)
			{
				D("RemoveKeysBySet: invalid array identifier");
				return -1;
			}
			BSWriteLocker lock(&dictsLock);
			const auto pair = dicts.find(identifier->formID);
			if (pair == dicts.end())
			{
				D("RemoveKeysBySet: Dict %08X not found", identifier->formID);
				return -1;
			}
			std::vector<TKey> keysToRemove;
			if (get(setId, keysToRemove))
			{
				SInt32 retVal = 0;
				for (auto& key : keysToRemove)
				{
					retVal += pair->second.Remove(key);
				}
				return retVal;
			}
			return -1;
		}
	};
}