#pragma once

template <class T>
struct DefaultValue
{
	static T Get() {return {};}
};

#include "TypeDefs.h"

template<>
struct DefaultValue<FORM_TYPE>
{
	static FORM_TYPE Get() { return nullptr;}
};

#include "f4se/PapyrusArgs.h"

template<>
struct DefaultValue<VMVariable>
{
	static VMVariable Get()
	{
		VMVariable retVal;
		retVal.GetValue().SetNone();
		return retVal;
	}
};
