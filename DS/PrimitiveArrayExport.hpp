#pragma once

#include "ArrayExport.hpp"
#include <optional>
namespace DS
{
	template <typename ELEMENT_TYPE, UInt32 ARRAY_ID, const char* PAPYRUS_NAME>
	class PrimitiveArrayExport final: public ArrayExport<ELEMENT_TYPE, PrimitiveArraysSet<ELEMENT_TYPE, PrimitiveArray<ELEMENT_TYPE>, ARRAY_ID>, PAPYRUS_NAME, PrimitiveArrayExport<ELEMENT_TYPE, ARRAY_ID, PAPYRUS_NAME>>
	{
		typedef PrimitiveArrayExport<ELEMENT_TYPE, ARRAY_ID, PAPYRUS_NAME> ThisType;
	public:
		PrimitiveArrayExport(const char* elementTypeName, ThisType* s)
			:ArrayExport(elementTypeName, s)
		{

		}

		bool Create(BGSKeyword* identifier)
		{
			return arrays.CreateArray(identifier);
		}

		SInt32 IndexOf(BGSKeyword* identifier, ELEMENT_TYPE value, SInt32 startIndex)
		{
			return this->arrays.IndexOf(identifier, value, startIndex);
		}

		bool Sort(BGSKeyword* identifier, UInt32 mode)
		{
			return this->arrays.Sort(identifier, mode);
		}
		
		std::optional<std::vector<ELEMENT_TYPE>> ToArray(BGSKeyword* identifier)
		{
			std::vector<ELEMENT_TYPE> result;
			if (!arrays.ExecuteOn<std::vector<ELEMENT_TYPE>>(identifier, GetAsArray,result))
			{
				D("Failed to get as array");
				return std::nullopt;
			}
			return std::optional<std::vector<ELEMENT_TYPE>>(std::move(result));
		}

		void Register(VirtualMachine* vm) override
		{
			ArrayExport::Register(vm);
			REGISTER_FUNC(vm, PAPYRUS_NAME, Create, 1);
			REGISTER_FUNC(vm, PAPYRUS_NAME, IndexOf, 3);
			REGISTER_FUNC(vm, PAPYRUS_NAME, Sort, 2);
			REGISTER_FUNC(vm, PAPYRUS_NAME, ToArray, 1);
		}


	private: // tools
		static std::vector<ELEMENT_TYPE> GetAsArray(PrimitiveArray<ELEMENT_TYPE>& array)
		{
			std::vector<ELEMENT_TYPE> retVal;
			UInt64 curVersion;
			array.GetState(retVal, curVersion);
			return retVal;
		}

	private: // static methods for calling from VM
		static bool VMCreate(StaticFunctionTag* _, BGSKeyword* identifier)
		{
			return singleton->Create(identifier);
		}
		static SInt32 VMIndexOf(StaticFunctionTag* _, BGSKeyword* identifier, ELEMENT_TYPE value, SInt32 startIndex)
		{
			return singleton->IndexOf(identifier, value, startIndex);
		}
		static bool VMSort(StaticFunctionTag* _, BGSKeyword* identifier, UInt32 mode)
		{
			return singleton->Sort(identifier, mode);
		}
		
		static VMArray<ELEMENT_TYPE> VMToArray(StaticFunctionTag* _, BGSKeyword* identifier)
		{
			// TODO: move conversion to a function
			VMArray<ELEMENT_TYPE> retVal;
			retVal.SetNone(true);
			std::optional<std::vector<ELEMENT_TYPE>> result = singleton->ToArray(identifier);
			if (!result.has_value())
				return retVal;
			retVal.SetNone(false);
			for(auto& val : result.value())
			{
				retVal.Push(&val);
			}
			return retVal;
		}
	};
}