#pragma once
#include "f4se/GameForms.h"
#include "f4se/PapyrusVM.h"
struct BSReferenceTypeUtils
{
	static bool IsForm(const VMObjectTypeInfo* type);
	static bool Is(const VMObjectTypeInfo* type, const VMObjectTypeInfo* basetype);
};

