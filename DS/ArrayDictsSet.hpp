#pragma once
#include "DictsSet.hpp"
#include "ArrayDict.hpp"

namespace DS
{
	template<typename TKey, typename TValue, typename ArrayType, typename DictType>
	class ArrayDictsSet : public DictsSet<TKey, ArrayType, DictType>
	{
	public:

		ArrayDictsSet(UInt32 structId, UInt32 structVersion)
			:DictsSet(structId, structVersion)
		{

		}

		SInt32 ArrayLength(BGSKeyword* identifier, TKey& key)
		{
			if (!identifier)
			{
				D("Length: invalid identifier");
				return -1;
			}
			BSReadLocker lock(&dictsLock);
			const auto pair = dicts.find(identifier->formID);
			if (pair == dicts.end())
				return -1;
			return pair->second.ArrayLength(key);
		}

		// if no key found then new array will be created for this key
		bool AddElement(BGSKeyword* identifier, TKey& key, TValue& value)
		{
			if (!identifier)
			{
				return FD("AddElement: invalid identifier");
			}
			BSWriteLocker lock(&dictsLock);
			const auto pair = dicts.find(identifier->formID);
			if (pair == dicts.end())
				return FD("Dict %08X not found", identifier->formID);
			return pair->second.AddElement(key, value);
		}

		// if no key found then new array will be created for key
		bool AddRange(BGSKeyword* identifier,  TKey& key, const std::vector<TValue>& toAdd)
		{
			if (!identifier)
			{
				return FD("AddRange: invalid identifier");
			}
			BSWriteLocker lock(&dictsLock);
			const auto pair = dicts.find(identifier->formID);
			if (pair == dicts.end())
				return FD("Dict %08X not found", identifier->formID);
			pair->second.AddElementsRange(key, toAdd);
			return true;
		}

		std::optional<TValue> GetElement(BGSKeyword* identifier, TKey& key, SInt32 index)
		{
			if (!identifier)
				return std::nullopt;

			BSReadLocker lock(&dictsLock);
			const auto pair = dicts.find(identifier->formID);
			if (pair == dicts.end())
			{
				D("Dict %08X not found", identifier->formID);
				return std::nullopt;
			}
			return pair->second.GetElement(key, index);
		}

		bool SetElement(BGSKeyword* identifier, TKey& key, SInt32 index, TValue& value)
		{
			if (!identifier)
				return FD("SetElement: invalid identifier");

			BSWriteLocker lock(&dictsLock);
			const auto pair = dicts.find(identifier->formID);
			if (pair == dicts.end())
				return FD("Dict %08X not found", identifier->formID);

			return pair->second.SetElement(key, index, value);
		}

		bool InsertElement(BGSKeyword* identifier, TKey& key, TValue& value, SInt32 index)
		{
			if (!identifier)
				return FD("InsertElement: invalid identifier");

			BSWriteLocker lock(&dictsLock);
			const auto pair = dicts.find(identifier->formID);
			if (pair == dicts.end())
				return FD("Dict %08X not found", identifier->formID);

			return pair->second.InsertElement(key, value, index);
		}

		void ClearElements(BGSKeyword* identifier, TKey& key)
		{
			if (!identifier)
			{
				D("ClearElements: invalid identifier");
				return;
			}

			BSWriteLocker lock(&dictsLock);
			const auto pair = dicts.find(identifier->formID);
			if (pair == dicts.end())
			{
				D("Dict %08X not found", identifier->formID);
				return;
			}

			pair->second.Clear(key);
		}

		bool RemoveAtIndex(BGSKeyword* identifier, TKey& key, SInt32 index)
		{
			if (!identifier)
				return FD("RemoveAtIndex: invalid identifier");

			BSWriteLocker lock(&dictsLock);
			const auto pair = dicts.find(identifier->formID);
			if (pair == dicts.end())
				return FD("Dict %08X not found", identifier->formID);

			return pair->second.RemoveAtIndex(key, index);
		}
	};
}