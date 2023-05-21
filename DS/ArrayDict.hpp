#pragma once
#include <unordered_map>
#include <vector>
#include "Dict.hpp"
#include "Array.hpp"
#include <optional>

namespace DS
{
	template <typename TKey, typename TValue, typename ArrayHandler, typename ArrayType>
	class ArrayDict : public Dict<TKey, ArrayType, ArrayHandler, std::unordered_map<TKey, ArrayType>>
	{
	public:
		ArrayDict(BGSKeyword* identifier) :
			Dict(identifier)
		{

		}

		UInt32 ArrayLength(TKey& key)
		{
			BSReadLocker lock(&dataLock);
			auto iter = data.find(key);
			if (iter == data.end())
				return -1;
			return iter->second.Length();
		}

		virtual ArrayType CreateArray() = 0;

		// if no key found then new array will be created for this key
		virtual bool AddElement(TKey& key, TValue& value)
		{
			if (!KeyValidator<TKey>::IsValid(key))
				return FD("AddElement: Invalid key");

			BSWriteLocker lock(&dataLock);
			const auto iter = data.find(key);
			if (iter == data.end())
			{
				ArrayType arr = CreateArray();
				arr.Add(value);
				version++;
				data.insert(std::make_pair(key, arr));
				return true;
			}
			if (!iter->second.Add(value))
				return FD("AddElement: Failed to add");
			version++;
			return true;
		}

		// if no key found then new array will be created for key
		void AddElementsRange(TKey& key, const std::vector<TValue>& toAdd)
		{
			if (!KeyValidator<TKey>::IsValid(key))
			{
				D("AddElementsRange: Invalid key");
				return;
			}
			BSWriteLocker lock(&dataLock);
			const auto iter = data.find(key);
			if (iter == data.end())
			{
				ArrayType arr = CreateArray();
				arr.AddRange(toAdd);
				data.insert(std::make_pair(key, arr));
				version++;
				return;
			}
			iter->second.AddRange(toAdd);
			version++;
		}

		UInt64 ArrayVersion(TKey& key)
		{
			BSReadLocker lock(&dataLock);
			const auto iter = data.find(key);
			if (iter == data.end())
				return -1;
			return iter->second.Version();
		}

		std::optional<TValue> GetElement(TKey& key, SInt32 index)
		{
			BSReadLocker lock(&dataLock);
			const auto iter = data.find(key);
			if (iter == data.end())
				return std::nullopt;
			return iter->second.Get(index);
		}

		virtual bool SetElement(TKey& key, SInt32 index, TValue& value)
		{
			BSWriteLocker lock(&dataLock);
			const auto iter = data.find(key);
			if (iter == data.end())
				return false;
			const bool retVal = iter->second.Set(index, value);
			if (retVal)
				version++;
			return retVal;
		}

		virtual bool InsertElement(TKey& key, TValue& val, SInt32 index)
		{
			BSWriteLocker lock(&dataLock);
			const auto iter = data.find(key);
			if (iter == data.end())
				return false;
			const bool retVal = iter->second.Insert(val, index);
			if (retVal)
				version++;
			return retVal;
		}

		void Clear(TKey& key)
		{
			BSWriteLocker lock(&dataLock);
			const auto iter = data.find(key);
			if (iter == data.end())
				return;
			iter->second.Clear();
			version++;
		}

		bool RemoveAtIndex(TKey& key, SInt32 index)
		{
			BSWriteLocker lock(&dataLock);
			auto iter = data.find(key);
			if (iter == data.end())
				return false;
			const bool retVal = iter->second.Remove(index);
			if (retVal)
				version++;
			return retVal;
		}

	};
};