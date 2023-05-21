#include "LoadManager.h"
#include <unordered_map>
#include <unordered_set>

#include "Indent.h"
#include "Shared/MyTrace.h"

namespace DS
{
	std::unordered_map<UInt32, std::function<bool(const F4SESerializationInterface*, UInt32, UInt32)>> registeredLoaders;

	std::unordered_set<UInt32> GetKeys()
	{
		std::unordered_set<UInt32> retVal;
		retVal.reserve(registeredLoaders.size());
		for(const auto& pair : registeredLoaders)
			retVal.insert(pair.first);
		return retVal;
	}

	void LoadManager::RegisterRecordLoader(UInt32 recordTag, std::function<bool(const F4SESerializationInterface*, UInt32, UInt32)> recordLoader)
	{
		// TODO: check overwriting
		registeredLoaders[recordTag] = std::move(recordLoader);
	}
	void LoadManager::LoadRecords(const F4SESerializationInterface* serializer)
	{
		std::unordered_set<UInt32> unusedLoaders = GetKeys();

		UInt32 recType, length, curVersion;
		while (serializer->GetNextRecordInfo(&recType, &curVersion, &length))
		{
			auto loaderPtr = registeredLoaders.find(recType);
			if (loaderPtr == registeredLoaders.end())
			{
				E("No data loader registered for tag '%.4s'", &recType);
				continue;
			}
			unusedLoaders.erase(recType);
			if (!loaderPtr->second(serializer, curVersion, length))
			{
				W("Record loader for tag '%.4s' failed", &recType);
			}
		}
		if (!unusedLoaders.empty())
		{
			W("Following registered loaders was not called");
			for(auto tag : unusedLoaders)
			{
				W("%s%.4s", Indent::Get(2), &tag);
			}
		}
	}

}