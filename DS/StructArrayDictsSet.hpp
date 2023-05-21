#pragma once
#include "ArrayDictsSet.hpp"
#include "StructArrayDict.hpp"

namespace DS
{
	template <typename TKey, typename DictType>
	class StructArrayDictsSet final: public ArrayDictsSet<TKey, VMValue, StructArray, DictType>
	{
	public:
		StructArrayDictsSet(UInt32 structId, UInt32 structVersion)
			: ArrayDictsSet(structId, structVersion)
		{

		}
		bool CreateDict(BGSKeyword* identifier, BSFixedString structName)
		{
			if (!identifier)
				return FD("CreateDict: invalid identifier");

			StructInfo found;
			if (!StructsInfoCache::Cache.GetInfoByName(structName, found))
				return FW("Can't find struct [%s]", structName.c_str());

			BSWriteLocker lock(&dictsLock);
			DictType dict(identifier, structName);
			return dicts.insert(std::make_pair(identifier->formID, dict)).second;
		}

		SInt32 IndexOf(BGSKeyword* identifier, TKey& key, BSFixedString& fieldName, VMValue fieldValue, SInt32 startIndex)
		{
			if (!identifier)
			{
				D("IndexOf: invalid identifier");
				return -1;
			}
			BSReadLocker lock(&dictsLock);
			auto pair = dicts.find(identifier->formID);
			if (pair == dicts.end())
				return -1;
			return pair->second.IndexOf(key, fieldName, fieldValue, startIndex);
		}
		bool SortArray(BGSKeyword* identifier, TKey& key, BSFixedString& fieldName, UInt32 mode)
		{
			if (!identifier)
				return FD("SortArray: invalid identifier");

			BSWriteLocker lock(&dictsLock);
			const auto pair = dicts.find(identifier->formID);
			if (pair == dicts.end())
				return FD("SortArray: no dictionary found");
			return pair->second.SortArray(key, fieldName, mode);
		}

		bool AppendArray(BGSKeyword* identifier, TKey& key, BGSKeyword* arrayId, std::function<bool(BGSKeyword*, BSFixedString, std::vector<VMValue>&)>& get)
		{
			if (!identifier)
				return FD("SortArray: invalid identifier");
			BSWriteLocker lock(&dictsLock);
			const auto pair = dicts.find(identifier->formID);
			if (pair == dicts.end())
				return FD("AppendArray: no dictionary found");
			const BSFixedString structName(pair->second.StructName());
			std::vector<VMValue> toAdd;
			if (get(arrayId, structName, toAdd))
			{
				pair->second.AddElementsRange(key, toAdd);
				return true;
			}
			return false;
		}

		bool CopyToArray(BGSKeyword* identifier, TKey& key, BGSKeyword* arrayId, std::function<bool(BGSKeyword*, BSFixedString, std::vector<VMValue>&)>& assign)
		{
			if (!identifier)
				return FD("CopyToArray: invalid identifier");
			BSReadLocker lock(&dictsLock);
			const auto pair = dicts.find(identifier->formID);
			if (pair == dicts.end())
				return FD("CopyToArray: no dictionary found");

			return pair->second.CopyToArray(key, arrayId, assign);
		}
	};
}