#pragma once
#include "ArrayDict.hpp"
#include "StructArray.hpp"
#include "StructArrayHandler.hpp"

namespace DS
{
	template <typename TKey>
	class StructArrayDict final : public ArrayDict<TKey, VMValue, StructArrayHandler, StructArray>
	{
		BSFixedString structName;
		std::equal_to<BSFixedString> str_equal;
	public:
		StructArrayDict(BGSKeyword* identifier, BSFixedString structName)
			:ArrayDict(identifier)
			,structName(structName)
		{
			valueHandler.SetStructName(structName);
		}

		const char* StructName() const
		{
			return structName.c_str();
		}

		StructArray CreateArray() override
		{
			return StructArray(nullptr, structName);
		}

		UInt32 IndexOf(TKey& key, BSFixedString& fieldName, VMValue fieldValue, SInt32 startIndex)
		{
			BSReadLocker lock(&dataLock);
			const auto iter = data.find(key);
			if (iter == data.end())
				return -1;
			return iter->second.IndexOf(fieldName, fieldValue, startIndex);
		}

		bool AddElement(TKey& key, VMValue& value) override
		{
			if (value.GetTypeEnum() == 0) // None
			{
				D("Adding None value where %s structure is expected", structName.c_str());
				return ArrayDict::AddElement(key, value);
				
			}
	        if (!StructInfo::ValidateObject(value, structName))
	            return FW("StructArrayDict.AddElement: Invalid structure object. Expected object of %s", structName.c_str());

			return ArrayDict::AddElement(key, value);
		}

		bool SetElement(TKey& key, SInt32 index, VMValue& value) override
		{
			if (value.GetTypeEnum() == 0) // None
			{
				D("Adding None value where %s structure is expected", structName.c_str());
				ArrayDict::SetElement(key, index, value);
				return true;
			}
	        if (!StructInfo::ValidateObject(value, structName))
	            return FW("StructArrayDict.SetElement: Invalid structure object. Expected object of %s", structName.c_str());
			return ArrayDict::SetElement(key, index, value);
		}

		bool InsertElement(TKey& key, VMValue& value, SInt32 index) override
		{
			if (value.GetTypeEnum() == 0) // None
			{
				D("Adding None value where %s structure is expected", structName.c_str());
				ArrayDict::InsertElement(key, value, index);
				return true;
			}
	        if (!StructInfo::ValidateObject(value, structName))
	            return FW("StructArrayDict.InsertElement: Invalid structure object. Expected object of %s", structName.c_str());
			return ArrayDict::InsertElement(key, value, index);
		}

		bool SortArray(TKey& key, BSFixedString& fieldName, UInt32 mode)
		{
			BSWriteLocker lock(&dataLock);
			const auto iter = data.find(key);
			if (iter == data.end())
				return FD("SortArray no array found");
			return iter->second.Sort(fieldName, mode);
		}

		bool CopyToArray(TKey& key, BGSKeyword* arrayId, const std::function<bool(BGSKeyword*, BSFixedString, std::vector<VMValue>&)>& assign)
		{
			BSReadLocker lock(&dataLock);
			const auto iter = data.find(key);
			if (iter == data.end())
				return FD("CopyToArray: no array found by key");
			std::vector<VMValue> elements;
			UInt64 version;
			iter->second.GetState(elements, version);
			return assign(arrayId, structName, elements);
		}
	};
}