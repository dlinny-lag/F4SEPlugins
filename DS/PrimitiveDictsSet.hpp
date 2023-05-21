#pragma once
#include "DictsSet.hpp"

namespace DS
{
	template <typename TKey, typename TValue, class DictType>
	class PrimitiveDictsSet final : public DictsSet<TKey, TValue, DictType>
	{
	public:
		PrimitiveDictsSet(UInt32 structId, UInt32 structVersion)
			:DictsSet(structId, structVersion)
		{

		}
		bool CreateDict(BGSKeyword* identifier)
		{
			if (!identifier)
				return FD("CreateDict: invalid identifier");
			BSWriteLocker lock(&dictsLock);
			DictType dict(identifier);
			return dicts.insert(std::make_pair(identifier->formID, dict)).second;
		}
	};
}