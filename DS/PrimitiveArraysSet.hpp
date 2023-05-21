#pragma once

#include "ArraysSet.hpp"

namespace DS
{
	template <typename T, class ArrayType, UInt32 structId, UInt32 structVersion = 1>
	class PrimitiveArraysSet final : public ArraysSet<T, ArrayType, structId, structVersion>
	{
	public:

		bool CreateArray(BGSKeyword* identifier)
		{
			if (!identifier)
				return FD("CreateArray: invalid identifier");
			BSWriteLocker lock(&arraysLock);
			ArrayType array(identifier);
			return arrays.insert(std::make_pair(identifier->formID, array)).second;
		}

		SInt32 IndexOf(BGSKeyword* identifier, T& value, SInt32 startIndex)
		{
			if (!identifier)
			{
				D("IndexOf: invalid identifier");
				return -1;
			}
			BSReadLocker lock(&arraysLock);
			const auto pair = arrays.find(identifier->formID);
			if (pair == arrays.end())
				return -1;
			return pair->second.IndexOf(value, startIndex);
		}

		bool Sort(BGSKeyword* identifier, UInt32 mode)
		{
			if (!identifier)
			{
				return FD("Sort: invalid identifier");
			}
			BSWriteLocker lock(&arraysLock);
			const auto pair = arrays.find(identifier->formID);
			if (pair == arrays.end())
			{
				return FD("Sort: array not found");
			}
			pair->second.Sort(mode);
			return true;
		}

		bool SetArray(BGSKeyword* identifier, std::vector<T>& elements)
		{
			if (!identifier)
				return FD("SetArray: invalid identifier");
			BSWriteLocker lock(&arraysLock);
			const auto pair = arrays.find(identifier->formID);
			if (pair == arrays.end())
			{
				// create new and populate
				ArrayType array(identifier);
				array.AddRange(elements);
				arrays.insert(std::make_pair(identifier->formID, array));
				return true;
			}
			if (pair->second.Length() > 0)
				return FW("SetArray: target is not empty");
			pair->second.AddRange(elements);
			return true;
		}

		bool GetArray(BGSKeyword* identifier, std::vector<T>& elements)
		{
			if (!identifier)
				return FD("GetArray: invalid identifier");
			BSReadLocker lock(&arraysLock);
			const auto pair = arrays.find(identifier->formID);
			if (pair != arrays.end())
			{
				UInt64 version;
				pair->second.GetState(elements, version);
				return true;
			}
			return FD("GetArray: no array found");
		}
	};
}