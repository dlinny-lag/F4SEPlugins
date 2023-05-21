#pragma once
#include "f4se/PapyrusValue.h"

struct StructValueHelper
{
	static VMStructTypeInfo* GetStructType(const VMValue& val)
	{
		IComplexType * structOrObject = val.GetComplexType();
		if (!structOrObject)
			return nullptr;
		if (VMValue::kType_Struct != structOrObject->GetType())
			return nullptr;
		return static_cast<VMStructTypeInfo*>(structOrObject);
	}
};