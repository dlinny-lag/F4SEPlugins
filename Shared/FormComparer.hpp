#pragma once
#include "f4se/GameForms.h"
#include "Shared/TypeDefs.h"
namespace std
{
	template <>
	struct equal_to<FORM_TYPE>
	{
		bool operator()(const FORM_TYPE& a, const FORM_TYPE& b) const
		{
			if (a == b)
				return true;
			if (a == nullptr)
				return false;
			return a->formID == b->formID;
		}
	};
}