#include "StructsInfoCache.h"
#include "StructInfo.h"

StructsInfoCache StructsInfoCache::Cache;
void StructsInfoCache::Dump()
{
	BSReadLocker lock(&cacheLock);
	for (auto& pair : cache)
	{
		Dmp("%s", pair.first.c_str());
	}
}
bool StructsInfoCache::GetInfoByName(BSFixedString& structName, StructInfo& foundInfo)
{
	{
		BSReadLocker lock(&cacheLock);
		const auto ptr = cache.find(structName);
		if (ptr != cache.end())
		{
			foundInfo = ptr->second;
			return true;
		}
	}

	// not in cache. get value, but do not enter to critical section
	if (!StructInfo::ExtractStructInfoFromName(structName, &foundInfo))
		return false;

	// check cache once again as we spent some time outside of critical section
	// and value may be added in other thread
	BSWriteLocker lock(&cacheLock); 
	const auto ptr = cache.find(structName);
	if (ptr != cache.end())
	{  // should very rarely happen
		foundInfo = ptr->second;
		return true;
	}

	cache.insert(std::make_pair(structName, foundInfo));
	return true;
}

// TODO: it must be called somewhere!
void StructsInfoCache::Clear()
{
	BSWriteLocker lock(&cacheLock);
	cache.clear();
}