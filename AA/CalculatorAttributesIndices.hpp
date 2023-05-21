#pragma once
#include <unordered_map>

namespace CalculatorAttributesIndices
{
	std::unordered_map<std::string, std::vector<UInt32>> indicesCache;
	BSReadWriteLock indicesLock;

	static void Reset()
	{
		BSWriteLocker lock(&indicesLock);
		indicesCache.clear();
	}

	constexpr int InvalidIndex = -1;

	static std::vector<UInt32> Get(const ScriptHandle& script, const std::vector<ActorValueInfo*>& values, const std::vector<ActorValueInfo*>& allAttributes)
	{
		const char* name = script.Name.c_str();
		if (name == nullptr || name[0] == 0)
			return std::vector<UInt32>(); // do not try to cache invalid calculator
		{// read
			BSReadLocker lock(&indicesLock);
			const auto iter = indicesCache.find(name);
			if (iter != indicesCache.end())
				return iter->second;
		}
		{ // write
			BSWriteLocker lock(&indicesLock);
			std::vector<UInt32> retVal;
			retVal.reserve(values.size());
			for (const auto attribute : values)
			{
				if (!attribute)
				{
					retVal.push_back(InvalidIndex);
					continue;
				}
				const UInt32 formId = attribute->formID;
				bool found = false;
				for (UInt32 i = 0; i < allAttributes.size(); i++)
				{
					if (formId == allAttributes[i]->formID)
					{
						retVal.push_back(i);
						found = true;
						break;
					}
				}
				if (!found)
					retVal.push_back(-1);
			}
			indicesCache.insert(std::make_pair(name, retVal));
			return retVal;
		}
	}
}