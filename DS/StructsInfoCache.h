#pragma once
#include <unordered_map>
#include "Shared/BSFixedStringEqual.hpp"
#include "Shared/BSFixedStringHashier.hpp"
#include "StructInfo.h"

class StructsInfoCache
{
	std::unordered_map<BSFixedString, StructInfo> cache;
	BSReadWriteLock cacheLock;

public:
	bool GetInfoByName(BSFixedString& structName, StructInfo& foundInfo);
	void Dump();
	void Clear();

	static StructsInfoCache Cache;
};


