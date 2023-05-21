#pragma once
#include "ArrayDictsSet.hpp"
#include "PrimitiveArray.hpp"
#include "PrimitiveArrayDict.hpp"
#include "PrimitiveArrayHandler.hpp"

namespace DS
{
	template <typename TKey, typename TValue, typename DictType>
	class PrimitiveArrayDictsSet final : public ArrayDictsSet<TKey, TValue, PrimitiveArray<TValue>, DictType>
	{
	public:
		PrimitiveArrayDictsSet(UInt32 structId, UInt32 structVersion)
			: ArrayDictsSet(structId, structVersion)
		{

		}
		bool CreateDict(BGSKeyword* identifier)
		{
			if (!identifier)
				return FD("CreateDict: invalid identifier");
			DictType dict(identifier);
			BSWriteLocker lock(&dictsLock);
			return dicts.insert(std::make_pair(identifier->formID, dict)).second;
		}

		SInt32 IndexOf(BGSKeyword* identifier, TKey& key, TValue& val, SInt32 startIndex)
		{
			if (!identifier)
			{
				D("IndexOf: invalid identifier");
				return -1;
			}
			BSReadLocker lock(&dictsLock);
			const auto pair = dicts.find(identifier->formID);
			if (pair == dicts.end())
			{
				D("IndexOf: no dictionary found");
				return -1;
			}
			return pair->second.IndexOf(key, val, startIndex);
		}
		bool SortArray(BGSKeyword* identifier, TKey& key, UInt32 mode)
		{
			if (!identifier)
			{
				return FD("SortArray: invalid identifier");
			}
			BSWriteLocker lock(&dictsLock);
			const auto pair = dicts.find(identifier->formID);
			if (pair == dicts.end())
				return FE("SortArray: no dictionary found");
			return pair->second.SortArray(key, mode);
		}

		bool AppendArray(BGSKeyword* identifier, TKey& key, BGSKeyword* arrayId, std::function<bool(BGSKeyword*, std::vector<TValue>&)>& get)
		{
			if (!identifier)
				return FD("SortArray: invalid identifier");
			
			BSWriteLocker lock(&dictsLock);
			const auto pair = dicts.find(identifier->formID);
			if (pair == dicts.end())
				return FD("AppendArray: no dictionary found");
			
			std::vector<TValue> toAdd;
			if (get(arrayId, toAdd))
			{
				pair->second.AddElementsRange(key, toAdd);
				return true;
			}
			return false;
		}

		bool CopyToArray(BGSKeyword* identifier, TKey& key, BGSKeyword* arrayId, std::function<bool(BGSKeyword*, std::vector<TValue>&)>& assign)
		{
			if (!identifier)
				return FD("CopyToArray: invalid identifier");
			BSWriteLocker lock(&dictsLock);
			const auto pair = dicts.find(identifier->formID);
			if (pair == dicts.end())
				return FD("CopyToArray: no dictionary found");

			return pair->second.CopyToArray(key, arrayId, assign);
		}
	};
	}