#pragma once
#include "Shared/TypeDefs.h"
#include "MyTrace.h"

template <typename T>
struct Validator
{
	static_assert(std::is_same<float, T>::value || std::is_same<SInt32, T>::value, "Unexpected template argument type");

	static bool IsValid(const T& key)
	{
		return true;
	}
	static bool IsValid(const T* key)
	{
		return true;
	}
	static T Validate(const T& key)
	{
		return key;
	}
};

template<int MaxLen>
struct StringValidator
{
	static bool IsValid(const BSFixedString& key)
	{
		return key.c_str() != nullptr && strlen(key.c_str()) <= MaxLen;
	}
	static bool IsValid(const BSFixedString* key)
	{
		return key->c_str() != nullptr && strlen(key->c_str()) <= MaxLen;
	}
	static BSFixedString Validate(const BSFixedString& key)
	{
		return key;
	}
};

template<>
struct Validator<FORM_TYPE>
{
	static bool IsValid(const FORM_TYPE& key)
	{
		return Validate(key) != nullptr;
	}
	static bool IsValid(const FORM_TYPE* key)
	{
		return Validate(*key) != nullptr;
	}
	static FORM_TYPE Validate(const FORM_TYPE& key)
	{
		if (key == nullptr)
			return key;
		__try
		{
			if (key->formID && (key->flags & TESForm::kFlag_IsDeleted) == 0 && key->GetFormType() < FormType::kFormType_Max)
				return key;
			return nullptr;
		}
		__except(true)
		{
			E("Invalid address %016X", key);
			return nullptr;
		}
	}
};

template<>
struct Validator<VMValue>
{
	static bool IsValid(const VMValue& key)
	{
		return true;
	}
	static bool IsValid(const VMValue* key)
	{
		return true;
	}
	static VMValue Validate(const VMValue& key)
	{
		return key;
	}
};

#define STRING_VALUE_MAX_LENGTH 0x80000000
template<>
struct Validator<BSFixedString> : StringValidator<STRING_VALUE_MAX_LENGTH>
{
	
};


template <typename TKey>
struct KeyValidator : Validator<TKey>
{
	
};

#define STRING_KEY_MAX_LENGTH 4096

template<>
struct KeyValidator<BSFixedString> : StringValidator<STRING_KEY_MAX_LENGTH>
{
	
};


template<>
struct KeyValidator<FORM_TYPE> : Validator<FORM_TYPE>
{
	
};

