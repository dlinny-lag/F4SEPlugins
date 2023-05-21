#pragma once
#include "ValueDict.hpp"
#include "StructHandler.h"
namespace DS
{
	template <typename TKey>
	class StructValueDict : public ValueDict<TKey, VMValue, StructHandler>
	{
		BSFixedString structName;
	    std::equal_to<BSFixedString> str_equal;

	public:
	    StructValueDict(BGSKeyword* identifier, BSFixedString structName) :
	        ValueDict(identifier)
	       ,structName(structName)
	    {
	        valueHandler.SetStructName(structName);
	    }

	    const char* StructName() const
	    {
	        return structName.c_str();
	    }

	    bool Add(TKey& key, VMValue& value) override
	    {
	        if (value.GetTypeEnum() == 0) // None
	        {
	            D("Adding None value when %s structure is expected", structName.c_str());
	            return ValueDict::Add(key, value);
	        }
	        if (!StructInfo::ValidateObject(value, structName))
	            return FW("StructValueDict.Add: Invalid structure object. Expected object of %s", structName.c_str());
	        return ValueDict::Add(key, value);
	    }

	    bool Set(TKey& key, VMValue& value) override
	    {
	        if (value.GetTypeEnum() == 0) // None
	        {
	            D("Assigning None value when %s structure is expected", structName.c_str());
	            return ValueDict::Set(key, value);
	        }
	        if (!StructInfo::ValidateObject(value, structName))
	            return FW("StructValueDict.Set: Invalid structure object. Expected object of %s", structName.c_str());
	        return ValueDict::Set(key, value);
	    }

	    bool Replace(TKey& key, VMValue& newValue) override
	    {
	        if (newValue.GetTypeEnum() == 0) // None
	        {
	            D("Replacing by None value when %s structure is expected", structName.c_str());
	            return ValueDict::Replace(key, newValue);
	        }
	        if (!StructInfo::ValidateObject(newValue, structName))
	            return FW("StructValueDict.Replace: Invalid structure object. Expected object of %s", structName.c_str());
	        return ValueDict::Replace(key, newValue);
	    }
	};
};