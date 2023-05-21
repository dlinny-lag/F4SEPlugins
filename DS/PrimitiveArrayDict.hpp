#pragma once
#include "ArrayDict.hpp"
#include "PrimitiveArray.hpp"

namespace DS
{
	template <typename TKey, typename TValue, typename ArrayHandler>
	class PrimitiveArrayDict final : public ArrayDict<TKey, TValue, ArrayHandler, PrimitiveArray<TValue>>
	{
	public:
		PrimitiveArrayDict(BGSKeyword* identifier) :
			ArrayDict(identifier)
		{

		}
		PrimitiveArray<TValue> CreateArray() override
		{
			return PrimitiveArray<TValue>();
		}
		SInt32 IndexOf(TKey & key, TValue & val, SInt32 startIndex)
		{
			BSReadLocker lock(&dataLock);
			const auto iter = data.find(key);
			if (iter == data.end())
				return -1;
			return iter->second.IndexOf(val, startIndex);
		}
		
		bool SortArray(TKey& key, UInt32 mode)
		{
			BSWriteLocker lock(&dataLock);
			const auto iter = data.find(key);
			if (iter == data.end())
				return FD("SortArray: no array found");
			iter->second.Sort(mode);
			return true;
		}
		bool CopyToArray(TKey& key, BGSKeyword* arrayId, std::function<bool(BGSKeyword*, std::vector<TValue>&)>& assign)
		{
			std::vector<TValue> elements;
			{
				BSReadLocker lock(&dataLock);
				const auto iter = data.find(key);
				if (iter == data.end())
					return FD("CopyToArray: no array found by key");
				UInt64 version;
				iter->second.GetState(elements, version);
			}
			return assign(arrayId, elements);
		}

	};
}