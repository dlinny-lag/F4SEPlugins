#pragma once
#include "ArrayDictExport.hpp"
#include "StructArrayDictsSet.hpp"
#include "StructArrayDictCreator.hpp"

namespace DS
{
	template<typename KEY_TYPE, 
		int DICT_ID, int Version, const char* PAPYRUS_NAME,
		typename KeysArrayExport, 
		typename KeysSetExport,
		typename ValuesArrayExport,
		typename KeyIterator, typename KeyPointer, 
		typename DICT_TYPE=StructArrayDict<KEY_TYPE>>
	class StructArrayDictExport final :
		public ArrayDictExport<KEY_TYPE, VMValue,
			DICT_TYPE, StructArrayDictsSet<KEY_TYPE, DICT_TYPE>,
			DICT_ID, Version, PAPYRUS_NAME,
			KeysArrayExport, KeysSetExport,
			KeyIterator, KeyPointer,
		StructArrayDictExport<KEY_TYPE, DICT_ID, Version, PAPYRUS_NAME, KeysArrayExport, KeysSetExport, ValuesArrayExport, KeyIterator, KeyPointer, DICT_TYPE>,
		VMVariable>
	{
		typedef StructArrayDictExport<KEY_TYPE, DICT_ID, Version, PAPYRUS_NAME, KeysArrayExport, KeysSetExport, ValuesArrayExport, KeyIterator, KeyPointer, DICT_TYPE> ThisType;
		std::function<bool(BGSKeyword*, BSFixedString, std::vector<VMValue>&)> assignArray;
		std::function<bool(BGSKeyword*, BSFixedString, std::vector<VMValue>&)> getArray;

	public:
		StructArrayDictExport(const char* keyTypeName,
				KeysArrayExport* keysArrayExport,
				KeysSetExport* keysSetExport,
				ValuesArrayExport* valuesArrayExport,
				ThisType* s)
			:ArrayDictExport(keyTypeName, "Struct", keysArrayExport, keysSetExport, s)
			,assignArray([valuesArrayExport](auto identifier, auto structName, auto values){return valuesArrayExport->SetStructArray(identifier, structName, values);})
			,getArray([valuesArrayExport](auto identifier, auto structName, auto values){return valuesArrayExport->GetStructArray(identifier, structName, values);})
		{
			
		}

		bool Create(BGSKeyword* identifier, BSFixedString structName)
		{
			return DictExport::dicts.CreateDict(identifier, structName);
		}

		SInt32 IndexOf(BGSKeyword* identifier, KEY_TYPE key, BSFixedString fieldName, VMValue value, SInt32 startIndex)
		{
			return DictExport::dicts.IndexOf(identifier, key, fieldName, value, startIndex);
		}

		bool SortArray(BGSKeyword* identifier, KEY_TYPE key, BSFixedString fieldName, UInt32 mode)
		{
			return DictExport::dicts.SortArray(identifier, key, fieldName, mode);
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
			REGISTER_FUNC(vm, PAPYRUS_NAME, Create, 2);
			REGISTER_FUNC(vm, PAPYRUS_NAME, GetElement, 3);
			REGISTER_FUNC(vm, PAPYRUS_NAME, IndexOf, 5);
			REGISTER_FUNC(vm, PAPYRUS_NAME, SortArray, 4);
			REGISTER_FUNC(vm, PAPYRUS_NAME, AppendArray, 3);
			REGISTER_FUNC(vm, PAPYRUS_NAME, CopyToArray, 3);
		}
	public: // override base methods to resolve ambiguity with static methods
		std::optional<VMValue> GetElement(BGSKeyword* identifier, KEY_TYPE key, UInt32 index) override
		{
			return ArrayDictExport::GetElement(identifier, key, index);
		}
	protected:
		void LogLoading() override
		{
			D("Loading %sDictStructArray", DictExport::keyTypeName);
		}
		void LogSaving() override
		{
			D("Saving %sDictStructArray", DictExport::keyTypeName);
		}
	private: // static methods to be invoked from VM
		static bool Create(StaticFunctionTag* _, BGSKeyword* identifier, BSFixedString structName)
		{
			return ArrayDictExport::singleton->Create(identifier, structName);
		}

		static VMVariable GetElement(StaticFunctionTag* _, BGSKeyword* identifier, KEY_TYPE key, UInt32 index)
		{
			std::optional<VMValue> result = ArrayDictExport::singleton->ArrayDictExport::GetElement(identifier, key, index);
			VMVariable retVal;
			if (!result.has_value())
			{
				bool f = false;
				retVal.Set(&f); // return false
			}
			else
			{
				retVal.GetValue() = result.value(); // return found value
			}
			return retVal;
		}

		static SInt32 IndexOf(StaticFunctionTag* _, BGSKeyword* identifier, KEY_TYPE key, BSFixedString fieldName, VMVariable value, SInt32 startIndex)
		{
			return ArrayDictExport::singleton->IndexOf(identifier, key, fieldName, value.GetValue(), startIndex);
		}

		static bool SortArray(StaticFunctionTag* _, BGSKeyword* identifier, KEY_TYPE key, BSFixedString fieldName, UInt32 mode)
		{
			return ArrayDictExport::singleton->SortArray(identifier, key, fieldName, mode);
		}

		static bool AppendArray(StaticFunctionTag* _, BGSKeyword* identifier, KEY_TYPE key, BGSKeyword* arrayId)
		{
			return ArrayDictExport::singleton->AppendArray(identifier, key, arrayId);
		}

		static bool CopyToArray(StaticFunctionTag* _, BGSKeyword* identifier, KEY_TYPE key, BGSKeyword* targetArrayId)
		{
			return ArrayDictExport::singleton->CopyToArray(identifier, key, targetArrayId);
		}
	};
}