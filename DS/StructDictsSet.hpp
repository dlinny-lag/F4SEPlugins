#pragma once
#include "DictsSet.hpp"
#include "StructsInfoCache.h"
namespace DS
{
	template <typename TKey, class DictType>
	class StructDictsSet : public DictsSet<TKey, VMValue, DictType>
	{
	public:
		StructDictsSet(UInt32 structId, UInt32 structVersion)
			:DictsSet(structId, structVersion)
		{

		}
		bool CreateDict(BGSKeyword* identifier, BSFixedString& structName)
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
	};
};