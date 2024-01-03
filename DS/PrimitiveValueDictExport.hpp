#pragma once
#include "ValueDictExport.hpp"
#include "ValueDict.hpp"
#include "PrimitiveDictsSet.hpp"

namespace DS
{
	template<typename KEY_TYPE, typename VALUE_TYPE,
		int DICT_ID, int Version, const char* PAPYRUS_NAME,
		typename ArrayExport, typename SetExport,
		typename KeyIterator, typename KeyPointer, typename Result,
		typename DICT_TYPE = ValueDict<KEY_TYPE, VALUE_TYPE>>
	class PrimitiveValueDictExport final : public ValueDictExport<KEY_TYPE, VALUE_TYPE,
		DICT_TYPE, 	PrimitiveDictsSet<KEY_TYPE, VALUE_TYPE, DICT_TYPE>,
		DICT_ID, Version, PAPYRUS_NAME,
		ArrayExport, SetExport,
		KeyIterator, KeyPointer,
		PrimitiveValueDictExport<KEY_TYPE, VALUE_TYPE, DICT_ID, Version, PAPYRUS_NAME, ArrayExport, SetExport, KeyIterator, KeyPointer, Result, DICT_TYPE>>
	{
		typedef PrimitiveValueDictExport<KEY_TYPE, VALUE_TYPE, DICT_ID, Version, PAPYRUS_NAME, ArrayExport, SetExport, KeyIterator, KeyPointer, Result, DICT_TYPE> ThisType;
	public:
		PrimitiveValueDictExport(const char* keyTypeName, const char* valueTypeName,
				ArrayExport* arrayExport,
				SetExport* setExport,
				ThisType* s)
			:ValueDictExport(keyTypeName, valueTypeName, arrayExport, setExport, s)
		{
			
		}

		bool Create(BGSKeyword* identifier)
		{
			return dicts.CreateDict(identifier);
		}

		void Register(VirtualMachine* vm) override
		{
			ValueDictExport::Register(vm);
			REGISTER_FUNC(vm, PAPYRUS_NAME, Create, 1);
			REGISTER_FUNC(vm, PAPYRUS_NAME, Get, 2);
		}

	private:
		static bool VMCreate(StaticFunctionTag* _, BGSKeyword* identifier)
		{
			return singleton->Create(identifier);
		}
		static Result VMGet(StaticFunctionTag* _, BGSKeyword* identifier, KEY_TYPE key)
		{
			const std::optional<VALUE_TYPE> result = singleton->ValueDictExport::Get(identifier, key);
			Result retVal;
			retVal.Set<bool>("Found", result.has_value());
			if (result.has_value())
				retVal.Set<VALUE_TYPE>("Value", result.value());
			else
				retVal.Set<VALUE_TYPE>("Value", {});
			return retVal;
		}
	};
};