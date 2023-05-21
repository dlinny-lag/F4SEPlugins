#pragma once
#include "f4se/PapyrusArgs.h"

template<typename TInner, typename TOuter = TInner>
struct ElementConverter
{
	TOuter ToOuter(TInner inner){return inner;}
	TInner FromOuter(TOuter outer){return outer;}
};

template<>
struct ElementConverter<VMValue, VMVariable>
{
	VMVariable ToOuter(VMValue inner)
	{
		VMVariable retVal;
		retVal.GetValue() = inner;
		return retVal;
	}
	VMValue FromOuter(VMVariable outer)
	{
		return outer.GetValue();
	}
};
