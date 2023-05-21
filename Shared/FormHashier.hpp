#pragma once
#include "f4se/GameForms.h"
#include <algorithm>
#include "Shared/TypeDefs.h"

namespace std
{
	template <>
	struct hash<FORM_TYPE>
	{
		std::hash<nullptr_t> nullHash;
		std::hash<UInt32> formHash;

		size_t operator()(const FORM_TYPE& form) const noexcept
		{
			if (form == nullptr)
				return nullHash(nullptr);

			return formHash(form->formID);
		}
	};
}