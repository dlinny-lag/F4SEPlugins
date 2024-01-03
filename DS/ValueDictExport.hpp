#pragma once
#include "f4se/PapyrusVM.h"
#include "f4se/PapyrusNativeFunctions.h"
#include "ElementConverter.hpp"
#include "DictExport.hpp"
#include "MyTrace.h"
#include <optional>
#include "PapyrusExports.h"

namespace DS
{
	template <typename KEY_TYPE, typename VALUE_TYPE,
		typename DICT_TYPE, typename DictsSet,
		int DICT_ID, int Version, const char* PAPYRUS_NAME,
		typename KeysArrayExport,
		typename KeysSetExport,
		typename KeyIterator, typename KeyPointer,
		typename Derived,
		typename OUTER_VALUE_TYPE = VALUE_TYPE>
	class ValueDictExport : public DictExport<KEY_TYPE, DICT_TYPE, DictsSet, DICT_ID, Version, KeysArrayExport, KeysSetExport, KeyIterator, KeyPointer>
	{
	private:
		const char* valueTypeName;
	protected:
		inline static Derived* singleton;
		inline static ElementConverter<VALUE_TYPE, OUTER_VALUE_TYPE> conv;
	public:
		ValueDictExport(const char* keyTypeName, const char* valueTypeName,
				KeysArrayExport* keysArrayExport, KeysSetExport* keysSetExport,
				Derived* s)
			:DictExport(keyTypeName, keysArrayExport, keysSetExport)
			,valueTypeName(valueTypeName)
		{
			if (singleton && singleton != s)
				throw std::exception("Invalid ValueDictExport initialization");
			singleton = s;
		}

		bool Add(BGSKeyword* identifier, KEY_TYPE key, VALUE_TYPE value)
		{
			const bool retVal = DictExport::dicts.Add(identifier, key, value);
			if (retVal)
				Iterable::iterators.RemoveAllIterators(identifier->formID);
			return retVal;
		}

		bool Set(BGSKeyword* identifier, KEY_TYPE key, VALUE_TYPE value)
		{
			const bool retVal = DictExport::dicts.Set(identifier, key, value);
			if (retVal)
				Iterable::iterators.RemoveAllIterators(identifier->formID);
			return retVal;
		}

		std::optional<VALUE_TYPE> Get(BGSKeyword* identifier, KEY_TYPE key)
		{
			return DictExport::dicts.Get(identifier, key);
		}

		bool Replace(BGSKeyword* identifier, KEY_TYPE key, VALUE_TYPE newValue)
		{
			const bool retVal = DictExport::dicts.Replace(identifier, key, newValue);
			if (retVal)
				Iterable::iterators.RemoveAllIterators(identifier->formID);
			return retVal;
		}

		void DumpAll()
		{
			Dmp("%s-%s dicts:", DictExport::keyTypeName, valueTypeName);
			DictExport::dicts.DumpAll(2);
			Dmp("%s-%s dicts iterators:", DictExport::keyTypeName, valueTypeName);
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

			REGISTER_FUNC(vm, PAPYRUS_NAME, CreateKeyIterator, 1);
			REGISTER_FUNC(vm, PAPYRUS_NAME, DeleteKeyIterator, 1);
			REGISTER_FUNC(vm, PAPYRUS_NAME, NextKey, 1);

			REGISTER_FUNC(vm, PAPYRUS_NAME, Add, 3);
			REGISTER_FUNC(vm, PAPYRUS_NAME, Set, 3);
			REGISTER_FUNC(vm, PAPYRUS_NAME, Replace, 3);
			REGISTER_FUNC(vm, PAPYRUS_NAME, DumpAll, 0);
		}
	protected:
		void LogLoading() override
		{
			D("Loading %sDict%s...", DictExport::keyTypeName, valueTypeName);
		}
		void LogSaving() override
		{
			D("Saving %sDict%s...", DictExport::keyTypeName, valueTypeName);
		}
	private: // static methods to be called from VM

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

		static bool VMAdd(StaticFunctionTag* _, BGSKeyword* identifier, KEY_TYPE key, OUTER_VALUE_TYPE value)
		{
			return singleton->Add(identifier, key, conv.FromOuter(value));
		}

		static bool VMSet(StaticFunctionTag* _, BGSKeyword* identifier, KEY_TYPE key, OUTER_VALUE_TYPE value)
		{
			return singleton->Set(identifier, key, conv.FromOuter(value));
		}

		static bool VMReplace(StaticFunctionTag* _, BGSKeyword* identifier, KEY_TYPE key, OUTER_VALUE_TYPE newValue)
		{
			return singleton->Replace(identifier, key, conv.FromOuter(newValue));
		}

		static void VMDumpAll(StaticFunctionTag* _)
		{
			return singleton->DumpAll();
		}
	};
};