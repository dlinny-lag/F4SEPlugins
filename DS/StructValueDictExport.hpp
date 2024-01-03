#pragma once
#include "ValueDictExport.hpp"
#include "StructValueDictCreator.hpp"
#include "StructDictsSet.hpp"

namespace DS
{
	template<typename KEY_TYPE,
		int DICT_ID, int Version, const char* PAPYRUS_NAME,
		typename ArrayExport,
		typename SetExport,
		typename KeyIterator, typename KeyPointer,
		typename DICT_TYPE = StructValueDict<KEY_TYPE>>
	class StructValueDictExport final : public ValueDictExport<KEY_TYPE, VMValue,
		DICT_TYPE, StructDictsSet<KEY_TYPE, DICT_TYPE>,
		DICT_ID, Version, PAPYRUS_NAME,
		ArrayExport, SetExport,
		KeyIterator, KeyPointer,
		StructValueDictExport<KEY_TYPE, DICT_ID, Version, PAPYRUS_NAME, ArrayExport, SetExport, KeyIterator, KeyPointer, DICT_TYPE>,
		VMVariable>
	{
		typedef StructValueDictExport<KEY_TYPE, DICT_ID, Version, PAPYRUS_NAME, ArrayExport, SetExport, KeyIterator, KeyPointer, DICT_TYPE> ThisType;
	public:
		StructValueDictExport(const char* keyTypeName,
				ArrayExport* arrayExport,
				SetExport* setExport,
				ThisType* s)
			:ValueDictExport(keyTypeName, "Struct", arrayExport, setExport, s)
		{
			
		}

		bool Create(BGSKeyword* identifier, BSFixedString structName)
		{
			const bool retVal = DictExport::dicts.CreateDict(identifier, structName);
			D("CreateStructDict returns %s", S(retVal));
			return retVal;
		}

		void Register(VirtualMachine* vm) override
		{
			ValueDictExport::Register(vm);
			REGISTER_FUNC(vm, PAPYRUS_NAME, Create, 2);
			REGISTER_FUNC(vm, PAPYRUS_NAME, Get, 2);
		}
	protected:
		void LogLoading() override
		{
			D("Loading %sDictStruct", DictExport::keyTypeName);
		}
		void LogSaving() override
		{
			D("Saving %sDictStruct", DictExport::keyTypeName);
		}
	private: // static methods to be called from VM
		static bool VMCreate(StaticFunctionTag* _, BGSKeyword* identifier, BSFixedString structName)
		{
			return ValueDictExport::singleton->Create(identifier, structName);
		}

		static VMVariable VMGet(StaticFunctionTag* _, BGSKeyword* identifier, KEY_TYPE key)
		{
			const std::optional<VMValue> result = ValueDictExport::singleton->ValueDictExport::Get(identifier, key);
			if (result.has_value())
				return ElementConverter<VMValue, VMVariable>().ToOuter(result.value());
			VMValue none;
			none.SetNone();
			return ElementConverter<VMValue, VMVariable>().ToOuter(none);
		}

	};
};