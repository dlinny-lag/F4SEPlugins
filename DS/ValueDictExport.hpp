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

		// used in derived classes, so no static method for VM export here
		virtual std::optional<VALUE_TYPE> Get(BGSKeyword* identifier, KEY_TYPE key)
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
	public: // override base methods to resolve ambiguity with static methods
		bool Delete(BGSKeyword* identifier) override
		{
			return DictExport::Delete(identifier);
		}
		SInt32 Size(BGSKeyword* identifier) override
		{
			return DictExport::Size(identifier);
		}
		bool Contains(BGSKeyword* identifier, KEY_TYPE key) override
		{
			return DictExport::Contains(identifier, key);
		}

		bool Remove(BGSKeyword* identifier, KEY_TYPE key) override
		{
			return DictExport::Remove(identifier, key);
		}

		void Dump(BGSKeyword* identifier) override
		{
			return DictExport::Dump(identifier);
		}

		bool CopyKeysToArray(BGSKeyword* identifier, BGSKeyword* arrayId) override
		{
			return DictExport::CopyKeysToArray(identifier, arrayId);
		}

		bool CopyKeysToSet(BGSKeyword* identifier, BGSKeyword* setId) override
		{
			return DictExport::CopyKeysToSet(identifier, setId);
		}

		SInt32 RemoveKeysByArray(BGSKeyword* identifier, BGSKeyword* arrayId) override
		{
			return DictExport::RemoveKeysByArray(identifier, arrayId);
		}

		SInt32 RemoveKeysBySet(BGSKeyword* identifier, BGSKeyword* setId) override
		{
			return DictExport::RemoveKeysBySet(identifier, setId);
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

		static bool Delete(StaticFunctionTag* _, BGSKeyword* identifier)
		{
			return singleton->DictExport::Delete(identifier);
		}

		static SInt32 Size(StaticFunctionTag* _, BGSKeyword* identifier)
		{
			return singleton->DictExport::Size(identifier);
		}

		static bool Contains(StaticFunctionTag* _, BGSKeyword* identifier, KEY_TYPE key)
		{
			return singleton->DictExport::Contains(identifier, key);
		}

		static bool Remove(StaticFunctionTag* _, BGSKeyword* identifier, KEY_TYPE key)
		{
			return singleton->DictExport::Remove(identifier, key);
		}

		static void Dump(StaticFunctionTag* _, BGSKeyword* identifier)
		{
			return singleton->DictExport::Dump(identifier);
		}

		static bool CopyKeysToArray(StaticFunctionTag* _, BGSKeyword* identifier, BGSKeyword* arrayId)
		{
			return singleton->DictExport::CopyKeysToArray(identifier, arrayId);
		}

		static bool CopyKeysToSet(StaticFunctionTag* _, BGSKeyword* identifier, BGSKeyword* setId)
		{
			return singleton->DictExport::CopyKeysToSet(identifier, setId);
		}

		static SInt32 RemoveKeysByArray(StaticFunctionTag* _, BGSKeyword* identifier, BGSKeyword* arrayId)
		{
			return singleton->DictExport::RemoveKeysByArray(identifier, arrayId);
		}

		static SInt32 RemoveKeysBySet(StaticFunctionTag* _, BGSKeyword* identifier, BGSKeyword* setId)
		{
			return singleton->DictExport::RemoveKeysBySet(identifier, setId);
		}

		static KeyIterator CreateKeyIterator(StaticFunctionTag* _, BGSKeyword* identifier)
		{
			return singleton->Iterable::CreateIterator(identifier);
		}
		static bool DeleteKeyIterator(StaticFunctionTag* _, KeyIterator iterator)
		{
			return singleton->Iterable::DeleteIterator(iterator);
		}

		static KeyPointer NextKey(StaticFunctionTag* _, KeyIterator iterator)
		{
			return singleton->Iterable::Next(iterator);
		}

		static bool Add(StaticFunctionTag* _, BGSKeyword* identifier, KEY_TYPE key, OUTER_VALUE_TYPE value)
		{
			return singleton->Add(identifier, key, conv.FromOuter(value));
		}

		static bool Set(StaticFunctionTag* _, BGSKeyword* identifier, KEY_TYPE key, OUTER_VALUE_TYPE value)
		{
			return singleton->Set(identifier, key, conv.FromOuter(value));
		}

		static bool Replace(StaticFunctionTag* _, BGSKeyword* identifier, KEY_TYPE key, OUTER_VALUE_TYPE newValue)
		{
			return singleton->Replace(identifier, key, conv.FromOuter(newValue));
		}

		static void DumpAll(StaticFunctionTag* _)
		{
			return singleton->DumpAll();
		}
	};
};