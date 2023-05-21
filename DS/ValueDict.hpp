#pragma once
#include "Dict.hpp"
#include <unordered_map>
#include "PrimitiveHandler.hpp"
#include <optional>

namespace DS
{
	template <typename TKey, typename TValue, typename ValueHandler=PrimitiveHandler<TValue>>
	class ValueDict : public Dict<TKey, TValue, ValueHandler, std::unordered_map<TKey, TValue>>
	{
	public :
		ValueDict(BGSKeyword* identifier) : 
			Dict(identifier) // TODO: clarify namespace 
		{
			
		}

		std::optional<TValue> Get(TKey& key)
		{
			BSReadLocker lock(&dataLock);
			const auto iter = data.find(key);
			if (iter == data.end())
				return std::nullopt;
			return iter->second;
		}

		virtual bool Add(TKey& key, TValue& value)
		{
			if (!KeyValidator<TKey>::IsValid(key))
				return false;
			BSWriteLocker lock(&dataLock);
			const bool retVal = data.insert(std::make_pair(key, value)).second;
			if (retVal)
				version++;
			return retVal;
		}
		
		virtual bool Set(TKey& key, TValue& value)
		{
			if (!KeyValidator<TKey>::IsValid(key))
				return false;
			BSWriteLocker lock(&dataLock);
			data[key] = value;
			version++;
			return true;
		}

		virtual bool Replace(TKey& key, TValue& newValue)
		{
			if (!KeyValidator<TKey>::IsValid(key))
				return false;
			BSWriteLocker lock(&dataLock);
			if (data.find(key) != data.end())
			{
				data[key] = newValue;
				version++;
				return true;
			}
			return false;
		}
	};
};