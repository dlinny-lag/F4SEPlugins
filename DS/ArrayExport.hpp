#pragma once
#include "f4se/PapyrusVM.h"
#include "f4se/PapyrusNativeFunctions.h"
#include "MyTrace.h"
#include "PrimitiveArraysSet.hpp"
#include "ElementConverter.hpp"
#include "PapyrusExports.h"
#include <optional>
#include "IRecordLoader.h"
#include "Shared/DefaultValue.hpp"

namespace DS
{
	template <typename ELEMENT_TYPE, typename ArraySet, const char* PAPYRUS_NAME, typename Derived, typename OUTER_ELEMENT_TYPE=ELEMENT_TYPE>
	class ArrayExport : public IRecordLoader
	{
	private:
		const char* elementTypeName;
	protected:
		inline static Derived* singleton = nullptr;
		inline static ElementConverter<ELEMENT_TYPE, OUTER_ELEMENT_TYPE> conv;
		ArraySet arrays;
	public:
		ArrayExport(const char* elementTypeName, Derived* s)
			:elementTypeName(elementTypeName)
		{
			if (singleton && singleton != s)
				throw std::exception("Invalid ArrayExport initialization");
			singleton = s;
		}

		virtual ~ArrayExport() = default;
		ArrayExport() = delete;

		bool Delete(BGSKeyword* identifier)
		{
			return arrays.DeleteArray(identifier);
		}

		UInt32 Size(BGSKeyword* identifier)
		{
			return arrays.Size(identifier);
		}

		bool Add(BGSKeyword* identifier, ELEMENT_TYPE value)
		{
			return arrays.Add(identifier, value);
		}

		bool AddRange(BGSKeyword* identifier, const std::vector<ELEMENT_TYPE>& toAdd)
		{
			return arrays.AddRange(identifier, toAdd);
		}

		std::optional<ELEMENT_TYPE> Get(BGSKeyword* identifier, SInt32 index)
		{
			return arrays.Get(identifier, index);
		}

		bool Set(BGSKeyword* identifier, SInt32 index, ELEMENT_TYPE value)
		{
			return arrays.Set(identifier, index, value);
		}

		bool Insert(BGSKeyword* identifier, ELEMENT_TYPE value, SInt32 index)
		{
			return arrays.Insert(identifier, value, index);
		}
			
		bool AppendArray(BGSKeyword* identifier, BGSKeyword* toAdd)
		{
			return arrays.AppendArray(identifier, toAdd);
		}

		bool Remove(BGSKeyword* identifier, SInt32 index)
		{
			return arrays.Remove(identifier, index);
		}
		UInt32 Clear(BGSKeyword* identifier)
		{
			return arrays.Clear(identifier);
		}

		void Clear()
		{
			arrays.Clear();
		}

		bool Save(const F4SESerializationInterface* serializer)
		{
			arrays.Save(serializer);
			return true;
		}

		UInt32 DataId() const
		{
			return arrays.DataId;
		}
		bool Load(const F4SESerializationInterface* serializer, UInt32 version, UInt32 length)
		{
			D("Loading %s arrays...", elementTypeName);
			return arrays.Load(serializer, version, length);
		}

		void DumpAll()
		{
			Dmp("%s arrays:", elementTypeName);
			arrays.DumpAll(2);
		}
		void Dump(BGSKeyword* identifier)
		{
			arrays.Dump(identifier, 0);
		}

		bool SetArray(BGSKeyword* identifier, std::vector<ELEMENT_TYPE>& elements)
		{ 
			return arrays.SetArray(identifier, elements);
		}

		bool GetArray(BGSKeyword* identifier, std::vector<ELEMENT_TYPE>& elements)
		{
			return arrays.GetArray(identifier, elements);
		}

		virtual void Register(VirtualMachine* vm)
		{
			REGISTER_FUNC(vm, PAPYRUS_NAME, Delete, 1);
			REGISTER_FUNC(vm, PAPYRUS_NAME, Size, 1);
			REGISTER_FUNC(vm, PAPYRUS_NAME, Add, 2);
			REGISTER_FUNC(vm, PAPYRUS_NAME, AddRange, 2);
			REGISTER_FUNC(vm, PAPYRUS_NAME, AppendArray, 2);
			REGISTER_FUNC(vm, PAPYRUS_NAME, Get, 2);
			REGISTER_FUNC(vm, PAPYRUS_NAME, Set, 3);
			REGISTER_FUNC(vm, PAPYRUS_NAME, Insert, 3);
			REGISTER_FUNC(vm, PAPYRUS_NAME, Remove, 2);
			REGISTER_FUNC(vm, PAPYRUS_NAME, Clear, 1);
			REGISTER_FUNC(vm, PAPYRUS_NAME, Dump, 1);
			REGISTER_FUNC(vm, PAPYRUS_NAME, DumpAll, 0);
		}

	private: // static methods for calling from VM


		static bool VMDelete(StaticFunctionTag* _, BGSKeyword* identifier)
		{
			return singleton->Delete(identifier);
		}

		static UInt32 VMSize(StaticFunctionTag* _, BGSKeyword* identifier)
		{
			return singleton->Size(identifier);
		}

		static bool VMAdd(StaticFunctionTag* _, BGSKeyword* identifier, OUTER_ELEMENT_TYPE value)
		{
			return singleton->Add(identifier, conv.FromOuter(value));
		}

		static bool VMAddRange(StaticFunctionTag* _, BGSKeyword* identifier, VMArray<OUTER_ELEMENT_TYPE> toAdd)
		{
			// TODO: move conversion to a function
			std::vector<ELEMENT_TYPE> arr;
			arr.reserve(toAdd.Length());
			for (UInt32 i = 0; i < toAdd.Length(); i++)
			{
				OUTER_ELEMENT_TYPE value;
				toAdd.Get(&value, i);
				arr.push_back(conv.FromOuter(value));
			}
			return singleton->AddRange(identifier, arr);
		}

		static bool VMAppendArray(StaticFunctionTag* _, BGSKeyword* identifier, BGSKeyword* toAdd)
		{
			return singleton->AppendArray(identifier, toAdd);
		}

		static OUTER_ELEMENT_TYPE VMGet(StaticFunctionTag* _, BGSKeyword* identifier, SInt32 index)
		{
			const std::optional<ELEMENT_TYPE> val = singleton->Get(identifier, index);
			if (val.has_value())
				return conv.ToOuter(val.value());
			return DefaultValue<OUTER_ELEMENT_TYPE>::Get();
		}

		static bool VMSet(StaticFunctionTag* _, BGSKeyword* identifier, SInt32 index, OUTER_ELEMENT_TYPE value)
		{
			return singleton->Set(identifier, index, conv.FromOuter(value));
		}

		static bool VMInsert(StaticFunctionTag* _, BGSKeyword* identifier, OUTER_ELEMENT_TYPE value, SInt32 index)
		{
			return singleton->Insert(identifier, conv.FromOuter(value), index);
		}
		
		static bool VMRemove(StaticFunctionTag* _, BGSKeyword* identifier, SInt32 index)
		{
			return singleton->Remove(identifier, index);
		}

		static UInt32 VMClear(StaticFunctionTag* _, BGSKeyword* identifier)
		{
			return singleton->Clear(identifier);
		}

		static void VMDumpAll(StaticFunctionTag* _)
		{
			 singleton->DumpAll();
		}

		static void VMDump(StaticFunctionTag* _, BGSKeyword* identifier)
		{
			 singleton->Dump(identifier);
		}
	};
}