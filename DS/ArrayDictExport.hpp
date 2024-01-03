#pragma once
#include "f4se/PapyrusVM.h"
#include "f4se/PapyrusNativeFunctions.h"
#include "DictExport.hpp"
#include "MyTrace.h"
#include "PapyrusExports.h"

namespace DS
{
	template<typename KEY_TYPE, typename VALUE_TYPE,
		typename DICT_TYPE, typename DictsSet,
		int DICT_ID, int Version, const char* PAPYRUS_NAME,
		typename KeysArrayExport,
		typename KeysSetExport,
		typename KeyIterator, typename KeyPointer,
		typename Derived,
		typename OUTER_VALUE_TYPE=VALUE_TYPE>
	class ArrayDictExport : public DictExport<KEY_TYPE, DICT_TYPE, DictsSet, DICT_ID, Version, KeysArrayExport, KeysSetExport, KeyIterator, KeyPointer>
	{
		const char* valueTypeName;
	protected:
		inline static Derived* singleton;
		inline static ElementConverter<VALUE_TYPE, OUTER_VALUE_TYPE> conv;
	public:

		ArrayDictExport(const char* keyTypeName, const char* valueTypeName,
				KeysArrayExport* keysArrayExport, KeysSetExport* keysSetExport,
				Derived* s)
			:DictExport(keyTypeName, keysArrayExport, keysSetExport)
			,valueTypeName(valueTypeName)
		{
			if (singleton && singleton != s)
				throw std::exception("Invalid ArrayDictExport initialization");
			singleton = s;
		}

		std::optional<VALUE_TYPE> GetElement(BGSKeyword* identifier, KEY_TYPE key, UInt32 index)
		{
			return DictExport::dicts.GetElement(identifier, key, index);
		}

		bool AddElement(BGSKeyword* identifier, KEY_TYPE key, VALUE_TYPE value)
		{
			return DictExport::dicts.AddElement(identifier, key, value);
		}

		bool AddRange(BGSKeyword* identifier, KEY_TYPE key, const std::vector<VALUE_TYPE>& toAdd)
		{
			return DictExport::dicts.AddRange(identifier, key, toAdd);
		}

		bool SetElement(BGSKeyword* identifier, KEY_TYPE key, VALUE_TYPE value, SInt32 index)
		{
			return DictExport::dicts.SetElement(identifier, key, index, value);
		}

		bool InsertElement(BGSKeyword* identifier, KEY_TYPE key, VALUE_TYPE value, SInt32 index)
		{
			return DictExport::dicts.InsertElement(identifier, key, value, index);
		}

		bool RemoveAtIndex(BGSKeyword* identifier, KEY_TYPE key, SInt32 index)
		{
			return DictExport::dicts.RemoveAtIndex(identifier, key, index);
		}

		SInt32 ArrayLength(BGSKeyword* identifier, KEY_TYPE key)
		{
			return DictExport::dicts.ArrayLength(identifier, key);
		}
		
		void DumpAll()
		{
			Dmp("%s-%sArray dicts:", DictExport::keyTypeName, valueTypeName);
			DictExport::dicts.DumpAll(2);
			Dmp("%s-%sArray dicts iterators:", DictExport::keyTypeName, valueTypeName);
			Iterable::iterators.DumpAll(2);
		}
		
		virtual void Register(VirtualMachine* vm)
		{
			REGISTER_FUNC(vm, PAPYRUS_NAME, Delete, 1);
			REGISTER_FUNC(vm, PAPYRUS_NAME, Size, 1);
			REGISTER_FUNC(vm, PAPYRUS_NAME, Contains, 2);
			REGISTER_FUNC(vm, PAPYRUS_NAME, Remove, 2);
			REGISTER_FUNC(vm, PAPYRUS_NAME, Dump, 1);
			REGISTER_FUNC(vm, PAPYRUS_NAME, CopyKeysToArray, 2);
			REGISTER_FUNC(vm, PAPYRUS_NAME, CopyKeysToSet, 2);
			REGISTER_FUNC(vm, PAPYRUS_NAME, RemoveKeysByArray, 2);
			REGISTER_FUNC(vm, PAPYRUS_NAME, RemoveKeysBySet, 2);

			REGISTER_FUNC(vm, PAPYRUS_NAME, AddElement, 3);
			REGISTER_FUNC(vm, PAPYRUS_NAME, AddRange, 3);
			REGISTER_FUNC(vm, PAPYRUS_NAME, SetElement, 4);
			REGISTER_FUNC(vm, PAPYRUS_NAME, InsertElement, 4);

			REGISTER_FUNC(vm, PAPYRUS_NAME, CreateKeyIterator, 1);
			REGISTER_FUNC(vm, PAPYRUS_NAME, DeleteKeyIterator, 1);
			REGISTER_FUNC(vm, PAPYRUS_NAME, NextKey, 1);

			REGISTER_FUNC(vm, PAPYRUS_NAME, ArrayLength, 2);
			REGISTER_FUNC(vm, PAPYRUS_NAME, RemoveAtIndex, 3);
			REGISTER_FUNC(vm, PAPYRUS_NAME, DumpAll, 0);
		}

	protected:
		void LogLoading() override
		{
			D("Loading %sDict%sArray...", DictExport::keyTypeName, valueTypeName);
		}
		void LogSaving() override
		{
			D("Saving %sDict%sArray...", DictExport::keyTypeName, valueTypeName);
		}
	private: // static methods to be invoked from VM

		static KeyIterator VMCreateKeyIterator(StaticFunctionTag* _, BGSKeyword* identifier)
		{
			return singleton->Iterable::CreateIterator(identifier);
		}
		static bool VMDeleteKeyIterator(StaticFunctionTag* _, KeyIterator iterator)
		{
			return singleton->Iterable::DeleteIterator(iterator);
		}

		static KeyPointer VMNextKey(StaticFunctionTag* _, KeyIterator iterator)
		{
			return singleton->Iterable::Next(iterator);
		}

		static bool VMDelete(StaticFunctionTag* _, BGSKeyword* identifier)
		{
			return singleton->DictExport::Delete(identifier);
		}

		static SInt32 VMSize(StaticFunctionTag* _, BGSKeyword* identifier)
		{
			return singleton->DictExport::Size(identifier);
		}

		static bool VMContains(StaticFunctionTag* _, BGSKeyword* identifier, KEY_TYPE key)
		{
			return singleton->DictExport::Contains(identifier, key);
		}

		static bool VMRemove(StaticFunctionTag* _, BGSKeyword* identifier, KEY_TYPE key)
		{
			return singleton->DictExport::Remove(identifier, key);
		}

		static void VMDump(StaticFunctionTag* _, BGSKeyword* identifier)
		{
			return singleton->DictExport::Dump(identifier);
		}

		static bool VMCopyKeysToArray(StaticFunctionTag* _, BGSKeyword* identifier, BGSKeyword* arrayId)
		{
			return singleton->DictExport::CopyKeysToArray(identifier, arrayId);
		}
		static bool VMCopyKeysToSet(StaticFunctionTag* _, BGSKeyword* identifier, BGSKeyword* setId)
		{
			return singleton->DictExport::CopyKeysToSet(identifier, setId);
		}

		static SInt32 VMRemoveKeysByArray(StaticFunctionTag* _, BGSKeyword* identifier, BGSKeyword* arrayId)
		{
			return singleton->DictExport::RemoveKeysByArray(identifier, arrayId);
		}

		static SInt32 VMRemoveKeysBySet(StaticFunctionTag* _, BGSKeyword* identifier, BGSKeyword* setId)
		{
			return singleton->DictExport::RemoveKeysBySet(identifier, setId);
		}

		static bool VMAddElement(StaticFunctionTag* _, BGSKeyword* identifier, KEY_TYPE key, OUTER_VALUE_TYPE value)
		{
			return singleton->AddElement(identifier, key, conv.FromOuter(value));
		}

		static bool VMAddRange(StaticFunctionTag* _, BGSKeyword* identifier, KEY_TYPE key, VMArray<OUTER_VALUE_TYPE> toAdd)
		{
			if (!identifier)
				return FE("AddRange: Invalid identifier");
			// TODO: move conversion to a function
			std::vector<VALUE_TYPE> arr;
			arr.reserve(toAdd.Length());
			for (UInt32 i = 0; i < toAdd.Length(); i++)
			{
				OUTER_VALUE_TYPE value;
				toAdd.Get(&value, i);
				arr.push_back(conv.FromOuter(value));
			}
			return singleton->AddRange(identifier, key, arr);
		}

		static bool VMSetElement(StaticFunctionTag* _, BGSKeyword* identifier, KEY_TYPE key, OUTER_VALUE_TYPE value, SInt32 index)
		{
			return singleton->SetElement(identifier, key, conv.FromOuter(value), index);
		}

		static bool VMInsertElement(StaticFunctionTag* _, BGSKeyword* identifier, KEY_TYPE key, OUTER_VALUE_TYPE value, SInt32 index)
		{
			return singleton->InsertElement(identifier, key, conv.FromOuter(value), index);
		}

		static bool VMRemoveAtIndex(StaticFunctionTag* _, BGSKeyword* identifier, KEY_TYPE key, SInt32 index)
		{
			return singleton->RemoveAtIndex(identifier, key, index);
		}

		static SInt32 VMArrayLength(StaticFunctionTag* _, BGSKeyword* identifier, KEY_TYPE key)
		{
			return singleton->ArrayLength(identifier, key);
		}

		static void VMDumpAll(StaticFunctionTag* _)
		{
			return singleton->DumpAll();
		}
	};
}