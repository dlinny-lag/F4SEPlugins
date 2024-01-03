#pragma once
#include "ArrayDictExport.hpp"
#include "PrimitiveArrayDictsSet.hpp"

namespace DS
{
	template<typename KEY_TYPE, typename VALUE_TYPE,
		int DICT_ID, int Version, const char* PAPYRUS_NAME,
		typename KeysArrayExport,
		typename KeysSetExport,
		typename ValuesArrayExport,
		typename KeyIterator, typename KeyPointer, typename Result,
		typename DICT_TYPE=PrimitiveArrayDict<KEY_TYPE, VALUE_TYPE, PrimitiveArrayHandler<VALUE_TYPE>>>
	class PrimitiveArrayDictExport final :
		public ArrayDictExport<KEY_TYPE, VALUE_TYPE,
			DICT_TYPE, PrimitiveArrayDictsSet<KEY_TYPE, VALUE_TYPE, DICT_TYPE>,
			DICT_ID, Version, PAPYRUS_NAME,
			KeysArrayExport, KeysSetExport,
			KeyIterator, KeyPointer,
		PrimitiveArrayDictExport<KEY_TYPE, VALUE_TYPE, DICT_ID, Version, PAPYRUS_NAME, KeysArrayExport, KeysSetExport, ValuesArrayExport, KeyIterator, KeyPointer, Result, DICT_TYPE>>
	{
	private:
		std::function<bool(BGSKeyword*, std::vector<VALUE_TYPE>&)> assignArray;
		std::function<bool(BGSKeyword*, std::vector<VALUE_TYPE>&)> getArray;
		typedef PrimitiveArrayDictExport<KEY_TYPE, VALUE_TYPE, DICT_ID, Version, PAPYRUS_NAME, KeysArrayExport, KeysSetExport, ValuesArrayExport, KeyIterator, KeyPointer, Result, DICT_TYPE> ThisType;
	public:
		PrimitiveArrayDictExport(const char* keyTypeName, const char* valueTypeName,
				KeysArrayExport* keysArrayExport,
				KeysSetExport* keysSetExport,
				ValuesArrayExport* valuesArrayExport,
				ThisType* s)
			:ArrayDictExport(keyTypeName, valueTypeName, keysArrayExport, keysSetExport, s)
			,assignArray([valuesArrayExport](auto identifier, auto values){return valuesArrayExport->SetArray(identifier, values);})
			,getArray([valuesArrayExport](auto identifier, auto values){return valuesArrayExport->GetArray(identifier, values);})
		{
			
		}

		bool Create(BGSKeyword* identifier)
		{
			return DictExport::dicts.CreateDict(identifier);
		}

		SInt32 IndexOf(BGSKeyword* identifier, KEY_TYPE key, VALUE_TYPE value, SInt32 startIndex)
		{
			return DictExport::dicts.IndexOf(identifier, key, value, startIndex);
		}

		bool SortArray(BGSKeyword* identifier, KEY_TYPE key, UInt32 mode)
		{
			return DictExport::dicts.SortArray(identifier, key, mode);
		}

		bool AppendArray(BGSKeyword* identifier, KEY_TYPE key, BGSKeyword* arrayId)
		{
			return DictExport::dicts.AppendArray(identifier, key, arrayId, getArray);
		}

		bool CopyToArray(BGSKeyword* identifier, KEY_TYPE key, BGSKeyword* targetArrayId)
		{
			return DictExport::dicts.CopyToArray(identifier, key, targetArrayId, assignArray);
		}

		void Register(VirtualMachine* vm) override
		{
			ArrayDictExport::Register(vm);
			REGISTER_FUNC(vm, PAPYRUS_NAME, Create, 1);
			REGISTER_FUNC(vm, PAPYRUS_NAME, GetElement, 3);
			REGISTER_FUNC(vm, PAPYRUS_NAME, IndexOf, 4);
			REGISTER_FUNC(vm, PAPYRUS_NAME, SortArray, 3);
			REGISTER_FUNC(vm, PAPYRUS_NAME, AppendArray, 3);
			REGISTER_FUNC(vm, PAPYRUS_NAME, CopyToArray, 3);
		}
	private: // static methods to be invoked from VM
		static bool VMCreate(StaticFunctionTag* _, BGSKeyword* identifier)
		{
			return ArrayDictExport::singleton->Create(identifier);
		}
		
		static Result VMGetElement(StaticFunctionTag* _, BGSKeyword* identifier, KEY_TYPE key, SInt32 index)
		{
			const std::optional<VALUE_TYPE> result = ArrayDictExport::singleton->ArrayDictExport::GetElement(identifier, key, index);
			Result retVal;
			retVal.Set<bool>("Found", result.has_value());
			if (result.has_value())
				retVal.Set<VALUE_TYPE>("Value", result.value());
			else
				retVal.Set<VALUE_TYPE>("Value", {});
			return retVal;
		}

		static SInt32 VMIndexOf(StaticFunctionTag* _, BGSKeyword* identifier, KEY_TYPE key, VALUE_TYPE value, SInt32 startIndex)
		{
			return  ArrayDictExport::singleton->IndexOf(identifier, key, value, startIndex);
		}

		static bool VMSortArray(StaticFunctionTag* _, BGSKeyword* identifier, KEY_TYPE key, UInt32 mode)
		{
			return  ArrayDictExport::singleton->SortArray(identifier, key, mode);
		}

		static bool VMAppendArray(StaticFunctionTag* _, BGSKeyword* identifier, KEY_TYPE key, BGSKeyword* arrayId)
		{
			return  ArrayDictExport::singleton->AppendArray(identifier, key, arrayId);
		}

		static bool VMCopyToArray(StaticFunctionTag* _, BGSKeyword* identifier, KEY_TYPE key, BGSKeyword* targetArrayId)
		{
			return  ArrayDictExport::singleton->CopyToArray(identifier, key, targetArrayId);
		}

	};
}