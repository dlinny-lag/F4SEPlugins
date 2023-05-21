#pragma once
#include "Array.hpp"
#include "StructsInfoCache.h"
#include "Shared/BSFixedStringEqual.hpp"
#include "VMValueEqual.hpp"
#include "StructSerialization.h"
#include "BSReferenceTypeUtils.h"
#include "StructHandler.h"
#include "Shared/Comparer.hpp"

namespace DS
{
	class StructArray final : public DS::Array<VMValue, StructHandler>
	{
	    BSFixedString structName;
	    std::equal_to<BSFixedString> str_equal;
	    std::equal_to<VMValue> val_equal;
	public:
	    StructArray(BGSKeyword* identifier, BSFixedString structName)
	        :Array(identifier)
	        ,structName(structName)
	    {
	        valueHandler.SetStructName(structName);
	    }
	    StructArray()
	        :Array()
	        ,structName("")
	    {

	    }

	    const char* StructName() const
	    {
	        return structName.c_str();
	    }

	    bool Add(const VMValue& value) override
	    {
	        if (value.GetTypeEnum() == 0) // None
	        {
	            D("Adding None value where %s structure is expected", structName.c_str());
	            return Array::Add(value);
	        }

	        if (!StructInfo::ValidateObject(value, structName))
	            return FW("StructArray.Add: Invalid structure object. Expected object of %s", structName.c_str());
	        return Array::Add(value);
	    }
	    void AddRange(const std::vector<VMValue>& toAdd) override
	    {
	        for (auto& val : toAdd)
	        {
	            if (!Add(val))
	            {
	                D("Failed to add some element in range. Adding None");
	                VMValue none;
	                none.SetNone();
	                Add(none);
	            }
	        }
	    }
	    bool Set(SInt32 index, VMValue& value) override
	    {
	        if (value.GetTypeEnum() == 0) // None
	            return Array::Set(index, value);

    		if (!StructInfo::ValidateObject(value, structName))
	            return FW("StructArray.Set: Invalid structure object. Expected object of %s", structName.c_str());
	        return Array::Set(index, value);
	    }
	    bool Insert(VMValue& value, SInt32 index) override
	    {
	        if (value.GetTypeEnum() == 0) // None
	            return Array::Insert(value, index);

    		if (!StructInfo::ValidateObject(value, structName))
	            return FW("StructArray.Insert: Invalid structure object. Expected object of %s", structName.c_str());

	        return Array::Insert(value, index);
	    }

	    SInt32 IndexOf(BSFixedString& fieldName, VMValue fieldValue, SInt32 startIndex)
	    {
	        if (startIndex < 0)
	        {
	            D("Invalid startIndex %d", startIndex);
	            return -1;
	        }
	        StructInfo myStruct;
	        if (!StructsInfoCache::Cache.GetInfoByName(structName, myStruct))
	        {
	            E("Critical: can't get structure info for %s", structName.c_str());
	            return -1;
	        }

	        StructField::FieldType foundFieldType = StructField::FieldType::None;
	        VMObjectTypeInfo* foundComplexType = nullptr;
	        for (auto& field : myStruct.Fields)
	        {
	            if (str_equal(field.Name, fieldName))
	            {
	                foundFieldType = field.Type;
	                foundComplexType = field.ComplexType;
	                break;
	            }
	        }
	        if (StructField::FieldType::None == foundFieldType)
	        {
	            W("Field %s not found in struct %s", fieldName.c_str(), structName.c_str());
	            return -1;
	        }

	        if (foundFieldType != StructField::FieldType::Form)
	        { 
	            if (fieldValue.GetTypeEnum() != foundFieldType)
	            {
	                W("Field type mismatch. Expected %" PRIu8", but got %" PRIu8, foundFieldType, fieldValue.GetTypeEnum());
	                return -1;
	            }
	        }
	        else 
	        { // Form

	            if (fieldValue.GetTypeEnum() == 0)
	            {
	                // None is allowed
	            }
	            else
	            { // validate type
	                VMObjectTypeInfo* fieldType = (VMObjectTypeInfo*)fieldValue.GetComplexType();
	                if (!fieldType)
	                {
	                    E("Failed to get value's type");
	                    return -1;
	                }
	                if (!BSReferenceTypeUtils::Is(fieldType, foundComplexType))
	                {
	                    W("Field type mismatch. Passed value has unexpected type");
	                    return -1;
	                }
	            }
	        }

	        
	        const auto memberPtr = myStruct.Type->m_members.Find(&fieldName);
	        if (!memberPtr)
	        {
	            E("Critical: Can't find field index");
	            return -1;
	        }
	        const UInt32 structFieldIndex = memberPtr->index;
	        BSReadLocker lock(&dataLock);
	        for (SInt32 i = startIndex; i < data.size(); i++)
	        {
	            if (data[i].GetTypeEnum() == 0) // No struct object
	            {
	                D("%d element is None. Skip", i);
	                continue;
	            }

	            if (val_equal(data[i].data.strct->GetStruct()[structFieldIndex], fieldValue))
	                return i;
	        }
	        D("Not found");
	        return -1;
	    }

	    bool Sort(BSFixedString& fieldName, UInt32 mode)
	    {
	        StructInfo myStruct;
	        if (!StructsInfoCache::Cache.GetInfoByName(structName, myStruct))
	        {
	            return FE("Critical: can't get structure info for %s", structName.c_str());
	        }
	        BSReadLocker lock(&dataLock);
	        if (mode % 2 == 0) // ascending
	        {
		        const ComparerLess<VMValue> cmp(myStruct.Type, fieldName);
	            if (cmp.Valid)
	            {
	                std::sort(data.begin(), data.end(), cmp);
	                 version++;
	                return true;
	            }
	            return FE("Critical: Failed to initialize less comparer");
	        }
	        else // descending
	        {
	            ComparerGreater<VMValue> cmp(myStruct.Type, fieldName);
	            if (cmp.Valid)
	            {
	                std::sort(data.begin(), data.end(), cmp);
	                 version++;
	                return true;
	            }
	            return FE("Critical: Failed to initialize greater comparer");
	        }
	    }
	};
}