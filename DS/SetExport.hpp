#pragma once
#include "f4se/PapyrusVM.h"
#include "f4se/PapyrusNativeFunctions.h"
#include "f4se/PapyrusStruct.h"
#include "MyTrace.h"
#include "SetsSet.hpp"
#include "Iterator.hpp"
#include "PapyrusExports.h"
#include "Iterable.hpp"
#include "IRecordLoader.h"

namespace DS
{
	template<typename ELEMENT_TYPE, UInt32 SET_ID, const char* PAPYRUS_NAME, typename Iterator, typename Pointer, typename ArrayExport,
	typename SET_TYPE=Set<ELEMENT_TYPE, std::unordered_set<ELEMENT_TYPE>>>
	class SetExport final : public Iterable<ELEMENT_TYPE, SET_TYPE, SetsSet<ELEMENT_TYPE, SET_TYPE>, Iterator, Pointer>, public IRecordLoader
	{
		typedef SetExport<ELEMENT_TYPE, SET_ID, PAPYRUS_NAME, Iterator, Pointer, ArrayExport, SET_TYPE> ThisType;
	private:
		const char* elementTypeName;
		inline static ThisType* singleton = nullptr;
		SetsSet<ELEMENT_TYPE, SET_TYPE> sets;
		std::function<bool(BGSKeyword*, std::vector<ELEMENT_TYPE>&)> assignArray;
		std::function<bool(BGSKeyword*, std::vector<ELEMENT_TYPE>&)> getArray;
		std::function<std::vector<ELEMENT_TYPE>(SET_TYPE&)> getAsArray;
	public:
		SetExport(const char* elementTypeName, ThisType* s, ArrayExport* arrayExport)
			:Iterable(&sets, SET_ID)
			,elementTypeName(elementTypeName)
			,sets(SET_ID, 1)
			,assignArray([arrayExport](auto identifier, auto elements){return arrayExport->SetArray(identifier, elements);})
			,getArray([arrayExport](auto identifier, auto elements){return arrayExport->GetArray(identifier, elements);})
			,getAsArray(GetAsArray)
		{
			if (singleton && singleton != s)
				throw std::exception("Invalid SetExport initialization");
			singleton = s;
		}


		bool Create(BGSKeyword* identifier)
		{
			return sets.CreateSet(identifier);
		}
		bool Delete(BGSKeyword* identifier)
		{
			const bool retVal = sets.DeleteSet(identifier);
			if (retVal)
				Iterable::iterators.RemoveAllIterators(identifier->formID);
			return retVal;
		}
		UInt32 Size(BGSKeyword* identifier)
		{
			return sets.Size(identifier);
		}
		bool Add(BGSKeyword* identifier, ELEMENT_TYPE value)
		{
			const bool retVal = sets.Add(identifier, value);
			if (retVal)
				Iterable::iterators.RemoveAllIterators(identifier->formID);
			return retVal;
		}
		SInt32 AddRange(BGSKeyword* identifier, std::vector<ELEMENT_TYPE> toAdd)
		{
			const SInt32 retVal = sets.AddRange(identifier, toAdd);
			if (retVal > 0)
				Iterable::iterators.RemoveAllIterators(identifier->formID);
			return retVal;
		}
		SInt32 RemoveRange(BGSKeyword* identifier, std::vector<ELEMENT_TYPE> toRemove)
		{
			const SInt32 retVal = sets.RemoveRange(identifier, toRemove);
			if (retVal > 0)
				Iterable::iterators.RemoveAllIterators(identifier->formID);
			return retVal;
		}
		bool Intersect(BGSKeyword* identifier, std::vector<ELEMENT_TYPE> other) // TODO: optimize: vector -> set
		{
			std::unordered_set<ELEMENT_TYPE> arr(other.begin(), other.end());
			const bool retVal = sets.Intersect(identifier, arr);
			if (retVal)
				Iterable::iterators.RemoveAllIterators(identifier->formID);
			return retVal;
		}
		bool Contains(BGSKeyword* identifier, ELEMENT_TYPE value)
		{
			return sets.Contains(identifier, value);
		}
		bool Remove(BGSKeyword* identifier, ELEMENT_TYPE value)
		{
			const bool retVal = sets.Remove(identifier, value);
			if (retVal)
				Iterable::iterators.RemoveAllIterators(identifier->formID);
			return retVal;
		}

		bool CopyToArray(BGSKeyword* identifier, BGSKeyword* arrayId)
		{
			return sets.CopyToArray(identifier, arrayId, assignArray);
		}

		SInt32 AddArray(BGSKeyword* identifier, BGSKeyword* arrayId)
		{
			return sets.AddArray(identifier, arrayId, getArray);
		}
		SInt32 AddSet(BGSKeyword* identifier, BGSKeyword* setId)
		{
			return sets.AddSet(identifier, setId);
		}

		bool IntersectArray(BGSKeyword* identifier, BGSKeyword* arrayId)
		{
			return sets.IntersectArray(identifier, arrayId, getArray);
		}
		bool IntersectSet(BGSKeyword* identifier, BGSKeyword* setId)
		{
			return sets.IntersectSet(identifier, setId);
		}

		SInt32 RemoveArray(BGSKeyword* identifier, BGSKeyword* arrayId)
		{
			return sets.RemoveArray(identifier, arrayId, getArray);
		}

		SInt32 RemoveSet(BGSKeyword* identifier, BGSKeyword* setId)
		{
			return sets.RemoveSet(identifier, setId);
		}
		UInt32 Clear(BGSKeyword* identifier)
		{
			return sets.Clear(identifier);
		}

		std::optional<std::vector<ELEMENT_TYPE>> ToArray(BGSKeyword* identifier)
		{
			std::vector<ELEMENT_TYPE> result;
			if (!sets.ExecuteOn<std::vector<ELEMENT_TYPE>>(identifier, getAsArray,result))
			{
				D("Failed to get as array");
				return std::nullopt;
			}
			return result;
		}
		
		void DumpAll()
		{
			Dmp("%s sets:", elementTypeName);
			sets.DumpAll(2);
			Dmp("%s sets iterators:", elementTypeName);
			Iterable::iterators.DumpAll(2);
		}
		void Dump(BGSKeyword* identifier)
		{
			sets.Dump(identifier, 0);
			if (identifier)
				Iterable::iterators.Dump(identifier->formID, 0);
		}

		void Clear()
		{
			sets.Clear();
			iterators.Clear();
		}

		bool Save(const F4SESerializationInterface* serializer)
		{
			if (sets.Save(serializer))
			{
				return Iterable::iterators.Save(serializer);
			}
			return true;
		}

		
		UInt32 DataId() const
		{
			return sets.DataId;
		}
		bool Load(const F4SESerializationInterface* serializer, UInt32 version, UInt32 length)
		{
			D("Loading %s sets...", elementTypeName);
			if (sets.Load(serializer, version, length))
			{
				D("Loading %s sets iterators...", elementTypeName);
				Iterable::iterators.Load(serializer, sets.GetVersionsMap());
			}
			return true;
		}


		bool SetSet(BGSKeyword* identifier, std::vector<ELEMENT_TYPE>& elements)
		{
			return sets.SetSet(identifier, elements);
		}
		bool GetSet(BGSKeyword* identifier, std::vector<ELEMENT_TYPE>& elements)
		{
			return sets.GetSet(identifier, elements);
		}

		void Register(VirtualMachine* vm)
		{
			REGISTER_FUNC(vm, PAPYRUS_NAME, Create, 1);
			REGISTER_FUNC(vm, PAPYRUS_NAME, Delete, 1);
			REGISTER_FUNC(vm, PAPYRUS_NAME, Size, 1);
			REGISTER_FUNC(vm, PAPYRUS_NAME, Add, 2);
			REGISTER_FUNC(vm, PAPYRUS_NAME, AddRange, 2);
			REGISTER_FUNC(vm, PAPYRUS_NAME, RemoveRange, 2);
			REGISTER_FUNC(vm, PAPYRUS_NAME, Intersect, 2);
			REGISTER_FUNC(vm, PAPYRUS_NAME, Contains, 2);
			REGISTER_FUNC(vm, PAPYRUS_NAME, Remove, 2);
			REGISTER_FUNC(vm, PAPYRUS_NAME, Clear, 1);
			REGISTER_FUNC(vm, PAPYRUS_NAME, Dump, 1);
			REGISTER_FUNC(vm, PAPYRUS_NAME, DumpAll, 0);
			REGISTER_FUNC(vm, PAPYRUS_NAME, CopyToArray, 2);
			REGISTER_FUNC(vm, PAPYRUS_NAME, ToArray, 1);
			REGISTER_FUNC(vm, PAPYRUS_NAME, AddArray, 2);
			REGISTER_FUNC(vm, PAPYRUS_NAME, AddSet, 2);
			REGISTER_FUNC(vm, PAPYRUS_NAME, IntersectArray, 2);
			REGISTER_FUNC(vm, PAPYRUS_NAME, IntersectSet, 2);
			REGISTER_FUNC(vm, PAPYRUS_NAME, RemoveArray, 2);
			REGISTER_FUNC(vm, PAPYRUS_NAME, RemoveSet, 2);

			REGISTER_FUNC(vm, PAPYRUS_NAME, CreateIterator, 1);
			REGISTER_FUNC(vm, PAPYRUS_NAME, DeleteIterator, 1);
			REGISTER_FUNC(vm, PAPYRUS_NAME, Next, 1);
		}

	private: // utility
		
		static std::vector<ELEMENT_TYPE> GetAsArray(SET_TYPE& set)
		{
			std::vector<ELEMENT_TYPE> retVal;
			UInt64 curVersion;
			set.GetState(retVal, curVersion);
			return retVal;
		}

	private: // static methods to call from VM
		static Iterator VMCreateIterator(StaticFunctionTag* _, BGSKeyword* identifier)
		{
			return singleton->Iterable::CreateIterator(identifier);
		}
		static bool VMDeleteIterator(StaticFunctionTag* _, Iterator iterator)
		{
			return singleton->Iterable::DeleteIterator(iterator);
		}

		static Pointer VMNext(StaticFunctionTag* _, Iterator iterator)
		{
			return singleton->Iterable::Next(iterator);
		}

		static bool VMCreate(StaticFunctionTag* _, BGSKeyword* identifier)
		{
			return singleton->Create(identifier);
		}

		static bool VMDelete(StaticFunctionTag* _, BGSKeyword* identifier)
		{
			return singleton->Delete(identifier);
		}

		static UInt32 VMSize(StaticFunctionTag* _, BGSKeyword* identifier)
		{
			return singleton->Size(identifier);
		}

		static bool VMAdd(StaticFunctionTag* _, BGSKeyword* identifier, ELEMENT_TYPE value)
		{
			return singleton->Add(identifier, value);
		}

		static SInt32 VMAddRange(StaticFunctionTag* _, BGSKeyword* identifier, VMArray<ELEMENT_TYPE> toAdd)
		{ // TODO: move conversion to a function
			std::vector<ELEMENT_TYPE> arr;
			arr.reserve(toAdd.Length());
			for (UInt32 i = 0; i < toAdd.Length(); i++)
			{
				ELEMENT_TYPE value;
				toAdd.Get(&value, i);
				arr.emplace_back(value);
			}

			return singleton->AddRange(identifier, arr);
		}

		static SInt32 VMRemoveRange(StaticFunctionTag* _, BGSKeyword* identifier, VMArray<ELEMENT_TYPE> toRemove)
		{// TODO: move conversion to a function
			std::vector<ELEMENT_TYPE> arr;
			arr.reserve(toRemove.Length());
			for (UInt32 i = 0; i < toRemove.Length(); i++)
			{
				ELEMENT_TYPE value;
				toRemove.Get(&value, i);
				arr.emplace_back(value);
			}
			return singleton->RemoveRange(identifier, arr);
		}

		static bool VMIntersect(StaticFunctionTag* _, BGSKeyword* identifier, VMArray<ELEMENT_TYPE> other)
		{
			// TODO: move conversion to a function
			std::vector<ELEMENT_TYPE> arr;
			arr.reserve(other.Length());
			for (UInt32 i = 0; i < other.Length(); i++)
			{
				ELEMENT_TYPE value;
				other.Get(&value, i);
				arr.emplace_back(value);
			}
			return singleton->Intersect(identifier, arr);
		}

		static bool VMContains(StaticFunctionTag* _, BGSKeyword* identifier, ELEMENT_TYPE value)
		{
			return singleton->Contains(identifier, value);
		}

		static bool VMRemove(StaticFunctionTag* _, BGSKeyword* identifier, ELEMENT_TYPE value)
		{
			return singleton->Remove(identifier, value);
		}

		static bool VMCopyToArray(StaticFunctionTag* _, BGSKeyword* identifier, BGSKeyword* arrayId)
		{
			return singleton->CopyToArray(identifier, arrayId);
		}

		static SInt32 VMAddArray(StaticFunctionTag* _, BGSKeyword* identifier, BGSKeyword* arrayId)
		{
			return singleton->AddArray(identifier, arrayId);
		}

		static SInt32 VMAddSet(StaticFunctionTag* _, BGSKeyword* identifier, BGSKeyword* setId)
		{
			return singleton->AddSet(identifier, setId);
		}

		static bool VMIntersectArray(StaticFunctionTag* _, BGSKeyword* identifier, BGSKeyword* arrayId)
		{
			return singleton->IntersectArray(identifier, arrayId);
		}

		static bool VMIntersectSet(StaticFunctionTag* _, BGSKeyword* identifier, BGSKeyword* setId)
		{
			return singleton->IntersectSet(identifier, setId);
		}

		static SInt32 VMRemoveArray(StaticFunctionTag* _, BGSKeyword* identifier, BGSKeyword* arrayId)
		{
			return singleton->RemoveArray(identifier, arrayId);
		}

		static SInt32 VMRemoveSet(StaticFunctionTag* _, BGSKeyword* identifier, BGSKeyword* setId)
		{
			return singleton->RemoveSet(identifier, setId);
		}

		static UInt32 VMClear(StaticFunctionTag* _, BGSKeyword* identifier)
		{
			return singleton->Clear(identifier);
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
		
		static void VMDumpAll(StaticFunctionTag* _)
		{
			return singleton->DumpAll();
		}
		static void VMDump(StaticFunctionTag* _, BGSKeyword* identifier)
		{
			return singleton->Dump(identifier);
		}
	};
}