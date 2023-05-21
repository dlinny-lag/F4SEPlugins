#pragma once
#include "ArrayExport.hpp"
#include "StructArraysSet.hpp"

namespace DS
{
	template <const char* PAPYRUS_NAME>
	class StructArrayExportTemplate final : public ArrayExport<VMValue, StructArraysSet, PAPYRUS_NAME, StructArrayExportTemplate<PAPYRUS_NAME>, VMVariable>
	{
	public:
		typedef StructArrayExportTemplate<PAPYRUS_NAME> ThisType;
		StructArrayExportTemplate(const char* elementTypeName, ThisType* s)
			:ArrayExport(elementTypeName, s)
		{
			
		}

		bool Create(BGSKeyword* identifier, BSFixedString structName)
		{
			return arrays.CreateArray(identifier, structName);
		}

		SInt32 IndexOf(BGSKeyword* identifier, BSFixedString fieldName, VMValue value, SInt32 startIndex)
		{
			const SInt32 retVal = arrays.IndexOf(identifier, fieldName, value, startIndex);
			D("StructArray:IndexOf returned %d", retVal);
			return retVal;
		}

		bool Sort(BGSKeyword* identifier, BSFixedString fieldName, UInt32 mode)
		{
			return arrays.Sort(identifier, fieldName, mode);
		}

		void Register(VirtualMachine* vm) override
		{
			ArrayExport::Register(vm);
			REGISTER_FUNC(vm, PAPYRUS_NAME, Create, 2);
			REGISTER_FUNC(vm, PAPYRUS_NAME, IndexOf, 4);
			REGISTER_FUNC(vm, PAPYRUS_NAME, Sort, 3);
		}

		bool GetStructArray(BGSKeyword* identifier, BSFixedString& structName, std::vector<VMValue>& elements)
		{
			return arrays.GetArray(identifier, structName, elements);
		}
		bool SetStructArray(BGSKeyword* identifier, BSFixedString& structName, std::vector<VMValue>& elements)
		{
			return arrays.SetArray(identifier, structName, elements);
		}

	private: // static methods for calling from VM

		static bool Create(StaticFunctionTag* _, BGSKeyword* identifier, BSFixedString structName)
		{
			return singleton->Create(identifier, structName);
		}

		static SInt32 IndexOf(StaticFunctionTag* _, BGSKeyword* identifier, BSFixedString fieldName, VMVariable value, SInt32 startIndex)
		{
			return singleton->IndexOf(identifier, fieldName, value.GetValue(), startIndex);
		}

		static bool Sort(StaticFunctionTag* _, BGSKeyword* identifier, BSFixedString fieldName, UInt32 mode)
		{
			return singleton->Sort(identifier, fieldName, mode);
		}
	};
}
