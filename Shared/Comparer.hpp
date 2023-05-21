#pragma once
#include "f4se/GameTypes.h"
#include "TypeDefs.h"
#include "f4se/PapyrusValue.h"

template<typename T>
struct ComparerLess
{
	static_assert(std::is_same<float, T>::value || std::is_same<SInt32, T>::value, "Unexpected template argument type");
	bool operator()(const T& a, const T& b) const
	{
		return a < b;
	}
};
template<>
struct ComparerLess<BSFixedString>
{
	bool operator()(const BSFixedString& a, const BSFixedString& b) const
	{
		if (a.data == b.data)
			return false; // not less
		if (a.data == nullptr)
			return true; // a is null, so it is less
		if (b.data == nullptr)
			return false; // b is null and a not null, so b is less
		
		return strcmp(a.c_str(), b.c_str()) < 0;
	}
};

template<>
struct ComparerLess<FORM_TYPE>
{
	bool operator()(const FORM_TYPE& a, const FORM_TYPE& b) const
	{
		if (a == b)
			return false;
		if (a == nullptr)
			return TW("null Form in comparer");
		if (b == nullptr)
			return FW("null Form in comparer");

		return a->formID < b->formID;
	}
};

template<>
struct ComparerLess<VMValue>
{
	// false - if comparer can't work
	bool Valid;

	ComparerLess() = delete;

	ComparerLess(VMStructTypeInfo* structType, BSFixedString& fieldName)
	{
		Valid = false;
		if (structType)
		{
			auto mi = structType->m_members.Find(&fieldName);
			if (mi)
			{
				index = mi->index;
				Valid = true;
			}
		}
	}


	bool operator()(const VMValue& aVal, const VMValue& bVal) const
	{
		if (aVal.data.p == bVal.data.p)
			return false;
		// we assume: VMValue contains only Struct and None values
		if (aVal.GetTypeEnum() != VMValue::kType_Struct)
			return true; // non struct, i.e. None is less than Struct
		if (bVal.GetTypeEnum() != VMValue::kType_Struct)
			return false;

		// get field values
		VMValue a = aVal.data.strct->GetStruct()[index];
		VMValue b = bVal.data.strct->GetStruct()[index];

		// compare field values
		if (a.data.p == b.data.p)
			return false;
		if (a.type.value == VMValue::kType_None)
			return true; // None value is less than other non None value
		if (b.type.value == VMValue::kType_None)
			return false;

		// we assume: if values are non None, then type of values is same
		if (a.type.value > VMValue::kType_ArrayEnd)
		{ // both are form values
			if (a.data.id == b.data.id)
				return false;
			if (a.data.id == nullptr)
				return true;
			if (b.data.id == nullptr)
				return false;
			return a.data.id->GetHandle() < b.data.id->GetHandle();
		}

		switch (a.type.value)
		{
		case VMValue::kType_Int:
			return a.data.i < b.data.i;
		case VMValue::kType_Float:
			return a.data.f < b.data.f;
		case VMValue::kType_Bool:
			return a.data.b < b.data.b;
		case VMValue::kType_String:
			return ComparerLess<BSFixedString>()(*a.data.GetStr(), *b.data.GetStr());
		}

		E("Critical: ComparerLess - comparing values has unsupported type");
		return false; // should never happen
	}

private:
	UInt32 index;
};

template<typename T>
struct ComparerGreater
{
	static_assert(std::is_same<float, T>::value || std::is_same<SInt32, T>::value, "Unexpected template argument type");
	bool operator()(const T& a, const T& b) const
	{
		return a > b;
	}
};

template<>
struct ComparerGreater<BSFixedString>
{
	bool operator()(const BSFixedString& a, const BSFixedString& b) const
	{
		if (a.data == b.data)
			return false;
		if (a.data == nullptr)
			return false;
		if (b.data == nullptr)
			return true;

		return strcmp(a.c_str(), b.c_str()) > 0;
	}
};

template<>
struct ComparerGreater<FORM_TYPE>
{
	bool operator()(const FORM_TYPE& a, const FORM_TYPE& b) const
	{
		if (a == b)
			return false;
		if (a == nullptr)
			return false;
		if (b == nullptr)
			return true;

		return a->formID > b->formID;
	}
};


template<>
struct ComparerGreater<VMValue>
{
	// false - if comparer can't work
	bool Valid;

	ComparerGreater() = delete;

	ComparerGreater(VMStructTypeInfo* structType, BSFixedString& fieldName)
	{
		Valid = false;
		if (structType)
		{
			auto mi = structType->m_members.Find(&fieldName);
			if (mi)
			{
				index = mi->index;
				Valid = true;
			}
		}
	}


	bool operator()(const VMValue& aVal, const VMValue& bVal) const
	{ // TODO: avoid duplication
		if (aVal.data.p == bVal.data.p)
			return false;
		// we assume: VMValue contains only Struct and None values
		if (aVal.GetTypeEnum() != VMValue::kType_Struct)
			return false; // non struct, i.e. None is greater than Struct
		if (bVal.GetTypeEnum() != VMValue::kType_Struct)
			return true;

		// get field values
		VMValue a = aVal.data.strct->GetStruct()[index];
		VMValue b = bVal.data.strct->GetStruct()[index];

		// compare field values
		if (a.data.p == b.data.p)
			return false;
		if (a.type.value == VMValue::kType_None)
			return false; // None value is greater than other non None value
		if (b.type.value == VMValue::kType_None)
			return true;

		// we assume: if values are non None, then type of values is same
		if (a.type.value > VMValue::kType_ArrayEnd)
		{ // both are form values
			if (a.data.id == b.data.id)
				return false;
			if (a.data.id == nullptr)
				return false;
			if (b.data.id == nullptr)
				return true;
			return a.data.id->GetHandle() > b.data.id->GetHandle();
		}

		switch (a.type.value)
		{
		case VMValue::kType_Int:
			return a.data.i > b.data.i;
		case VMValue::kType_Float:
			return a.data.f > b.data.f;
		case VMValue::kType_Bool:
			return a.data.b > b.data.b;
		case VMValue::kType_String:
			return ComparerGreater<BSFixedString>()(*a.data.GetStr(), *b.data.GetStr());
		}

		E("Critical: ComparerGreater - comparing values has unsupported type");
		return false; // should never happen
	}

private:
	UInt32 index;
};
