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

		// used in derived types so no static method here
		virtual std::optional<VALUE_TYPE> GetElement(BGSKeyword* identifier, KEY_TYPE key, UInt32 index)
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
			D("Loading %sDict%sArray...", DictExport::keyTypeName, valueTypeName);
		}
		void LogSaving() override
		{
			D("Saving %sDict%sArray...", DictExport::keyTypeName, valueTypeName);
		}
	private: // static methods to be invoked from VM

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

		static bool AddElement(StaticFunctionTag* _, BGSKeyword* identifier, KEY_TYPE key, OUTER_VALUE_TYPE value)
		{
			return singleton->AddElement(identifier, key, conv.FromOuter(value));
		}

		static bool AddRange(StaticFunctionTag* _, BGSKeyword* identifier, KEY_TYPE key, VMArray<OUTER_VALUE_TYPE> toAdd)
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

		static bool SetElement(StaticFunctionTag* _, BGSKeyword* identifier, KEY_TYPE key, OUTER_VALUE_TYPE value, SInt32 index)
		{
			return singleton->SetElement(identifier, key, conv.FromOuter(value), index);
		}

		static bool InsertElement(StaticFunctionTag* _, BGSKeyword* identifier, KEY_TYPE key, OUTER_VALUE_TYPE value, SInt32 index)
		{
			return singleton->InsertElement(identifier, key, conv.FromOuter(value), index);
		}

		static bool RemoveAtIndex(StaticFunctionTag* _, BGSKeyword* identifier, KEY_TYPE key, SInt32 index)
		{
			return singleton->RemoveAtIndex(identifier, key, index);
		}

		static SInt32 ArrayLength(StaticFunctionTag* _, BGSKeyword* identifier, KEY_TYPE key)
		{
			return singleton->ArrayLength(identifier, key);
		}

		static void DumpAll(StaticFunctionTag* _)
		{
			return singleton->DumpAll();
		}
	};
}