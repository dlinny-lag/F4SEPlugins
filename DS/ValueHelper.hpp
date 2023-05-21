#pragma once
#include "f4se/GameForms.h"
#include "f4se/PluginAPI.h"
#include "MyTrace.h"
#include "Indent.h"
#include "SerializationHelper.h"
#include "Shared/TypeDefs.h"
#include <cinttypes>
#include "KeyValidator.hpp"

struct ValueHelper
{

	static inline bool Save(const F4SESerializationInterface* serializer, const SInt32* val)
	{
		if (!serializer->WriteRecordData(val, sizeof(SInt32)))
			return FE("Failed to write int value");
		return true;
	}

	static inline bool Save(const F4SESerializationInterface* serializer, const UInt32* val)
	{
		if (!serializer->WriteRecordData(val, sizeof(UInt32)))
			return FE("Failed to write int value");
		return true;
	}

	static inline bool Save(const F4SESerializationInterface* serializer, const float* val)
	{
		if (!serializer->WriteRecordData(val, sizeof(float)))
			return FE("Failed to write float value");
		return true;
	}
	static inline bool Save(const F4SESerializationInterface* serializer, const BSFixedString* val)
	{
		if (!SerializationHelper::WriteString(serializer, val))
			return FE("Failed to write string value");
		return true;
	}
	static inline bool Save(const F4SESerializationInterface* serializer, const FORM_TYPE* val)
	{
		TESForm* form = *val;
		UInt32 formId = form ? form->formID : 0;
		if (!serializer->WriteRecordData(&formId, sizeof(UInt32)))
			return FE("Failed to write form ID");
		return true;
	}
	static inline bool Save(const F4SESerializationInterface* serializer, const UInt8* val)
	{
		if (!serializer->WriteRecordData(val, sizeof(UInt8)))
			return FE("Failed to write UInt8 value");
		return true;
	}
	static inline bool Save(const F4SESerializationInterface* serializer, const bool* val)
	{
		if (!serializer->WriteRecordData(val, sizeof(bool)))
			return FE("Failed to write UInt8 value");
		return true;
	}
	static inline bool Save(const F4SESerializationInterface* serializer, const UInt64* val)
	{
		if (!serializer->WriteRecordData(val, sizeof(UInt64)))
			return FE("Failed to write UInt64 value");
		return true;
	}

	static inline bool Load(const F4SESerializationInterface* serializer, SInt32* outVal)
	{
		if (!serializer->ReadRecordData(outVal, sizeof(SInt32)))
			return FE("Failed to read int value");
		return true;
	}

	static inline bool Load(const F4SESerializationInterface* serializer, UInt32* outVal)
	{
		if (!serializer->ReadRecordData(outVal, sizeof(UInt32)))
			return FE("Failed to read int value");
		return true;
	}

	static inline bool Load(const F4SESerializationInterface* serializer, float* outVal)
	{
		if (!serializer->ReadRecordData(outVal, sizeof(float)))
			return FE("Failed to read float value");
		return true;
	}
	static inline bool Load(const F4SESerializationInterface* serializer, BSFixedString* outVal)
	{
		if (!SerializationHelper::ReadString(serializer, outVal))
			return FE("Failed to read string value");
		return true;
	}
	static inline bool Load(const F4SESerializationInterface* serializer, FORM_TYPE* outVal)
	{
		UInt32 formId = 0, actualFormId = 0;

		if (!serializer->ReadRecordData(&formId, sizeof(UInt32)))
			return FE("Failed to read form ID");

		if (!serializer->ResolveFormId(formId, &actualFormId))
		{
			W("Form %08X not resolved ", formId);
			*outVal = nullptr;
			return true; // empty value still value
		}
		const FORM_TYPE f = LookupFormByID(actualFormId);
		if (!f)
		{
			W("Form with id %08X not found", actualFormId);
			// empty value still value
		}
		*outVal = f;
		return true;
	}

	static inline bool Load(const F4SESerializationInterface* serializer, UInt8* outVal)
	{
		if (!serializer->ReadRecordData(outVal, sizeof(UInt8)))
			return FE("Failed to read UInt8 value");
		return true;
	}

	static inline bool Load(const F4SESerializationInterface* serializer, bool* outVal)
	{
		if (!serializer->ReadRecordData(outVal, sizeof(bool)))
			return FE("Failed to read UInt8 value");
		return true;
	}

	static inline bool Load(const F4SESerializationInterface* serializer, UInt64* outVal)
	{
		if (!serializer->ReadRecordData(outVal, sizeof(UInt64)))
			return FE("Failed to read UInt64 value");
		return true;
	}

	static inline void Dump(int indent, const SInt32& value)
	{
		Dmp("%s%d", Indent::Get(indent), value);
	}
	static inline void Dump(int indent, const UInt32& value)
	{
		Dmp("%s%u", Indent::Get(indent), value);
	}
	static inline void Dump(int indent, const float& value)
	{
		Dmp("%s%f", Indent::Get(indent), value);
	}
	static inline void Dump(int indent, const BSFixedString& value)
	{
		const char* str = value.c_str();
		if (!str)
			str = "";
		Dmp("%s%s", Indent::Get(indent), str);
	}
	static inline void Dump(int indent, const FORM_TYPE& value)
	{
		if (KeyValidator<FORM_TYPE>::IsValid(value))
			Dmp("%s%08X-%02X", Indent::Get(indent), value ? value->formID : 0, value ? value->GetFormType():0);
		else
			Dmp("%sInvalidForm", Indent::Get(indent));
	}

	static inline void Dump(int indent, const UInt8& value)
	{
		Dmp("%s%" PRIu8, Indent::Get(indent), value);
	}
	static inline void Dump(int indent, const bool& value)
	{
		Dmp("%s%s", Indent::Get(indent), S(value));
	}
	static inline void Dump(int indent, const UInt64& value)
	{
		Dmp("%s%016X", Indent::Get(indent), value);
	}
};

