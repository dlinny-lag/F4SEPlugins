#pragma once
#include "ArraysSet.hpp"
#include "StructArray.hpp"
#include "CollectionTypeId.h"
#include "StructsInfoCache.h"

namespace DS
{
	class StructArraysSet final : public ArraysSet<VMValue, StructArray, StructArrayTypeId, 1>
	{
	public:
		bool CreateArray(BGSKeyword* identifier, BSFixedString& structName)
		{
			if (!identifier)
				return FD("CreateArray: invalid identifier");

			StructInfo found;
			if (!StructsInfoCache::Cache.GetInfoByName(structName, found))
				return FW("Can't find struct [%s]", structName.c_str());

			BSWriteLocker lock(&arraysLock);
			StructArray array(identifier, structName);
			return arrays.insert(std::make_pair(identifier->formID, array)).second;
		}

		SInt32 IndexOf(BGSKeyword* identifier, BSFixedString& fieldName,  VMValue& value, SInt32 startIndex)
		{
			if (!identifier)
			{
				D("IndexOf: invalid identifier");
				return -1;
			}
			BSReadLocker lock(&arraysLock);
			const auto pair = arrays.find(identifier->formID);
			if (pair == arrays.end())
			{
				D("IndexOf: no array found with id=%08X", identifier->formID);
				return -1;
			}
			return pair->second.IndexOf(fieldName, value, startIndex);
		}

		bool Sort(BGSKeyword* identifier, BSFixedString& fieldName, UInt32 mode)
		{
			if (!identifier)
				return FD("Sort: invalid identifier");

			BSWriteLocker lock(&arraysLock);
			const auto pair = arrays.find(identifier->formID);
			if (pair == arrays.end())
				return FD("Sort: no array found with id=%08X", identifier->formID);

			return pair->second.Sort(fieldName, mode);
		}
		
		bool SetArray(BGSKeyword* identifier, BSFixedString& structName, std::vector<VMValue>& elements)
		{
			if (!identifier)
				return FD("SetArray: invalid identifier");
			BSWriteLocker lock(&arraysLock);
			const auto pair = arrays.find(identifier->formID);
			if (pair == arrays.end())
			{
				// create new and populate
				StructArray array(identifier, structName);
				array.AddRange(elements);
				arrays.insert(std::make_pair(identifier->formID, array));
				return true;
			}
			if (pair->second.Length() > 0)
				return FW("SetArray: target is not empty");

			if (strcmp(pair->second.StructName(), structName.c_str()) != 0)
				return FW("Wrong structure name of target array. Expected %s, but got %s", structName.c_str(), pair->second.StructName());

			pair->second.AddRange(elements);
			return true;
		}

		bool GetArray(BGSKeyword* identifier, BSFixedString& structName, std::vector<VMValue>& elements)
		{
			if (!identifier)
				return FD("SetArray: invalid identifier");
			BSWriteLocker lock(&arraysLock);
			const auto pair = arrays.find(identifier->formID);
			if (pair == arrays.end())
				return FD("Array %08X not found", identifier->formID);

			if (strcmp(pair->second.StructName(), structName.c_str()) != 0)
				return FW("Wrong structure name of array. Expected %s, but got %s", structName.c_str(), pair->second.StructName());

			UInt64 version;
			pair->second.GetState(elements, version);
			return true;
		}
	};
}