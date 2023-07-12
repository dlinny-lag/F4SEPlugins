#pragma once
#include "Iterable.hpp"
#include "IRecordLoader.h"

namespace DS
{
	// base functionality that doesn't touch values of dict
	template<typename KEY_TYPE,
			typename DICT_TYPE, typename DictsSet,
			UInt32 DICT_ID, int Version, 
			typename KeysArrayExport, typename KeysSetExport,
			typename KeyIterator, typename KeyPointer>
	class DictExport : public Iterable<KEY_TYPE, DICT_TYPE, DictsSet, KeyIterator, KeyPointer>, public IRecordLoader
	{
	private:
		std::function<bool(BGSKeyword*, std::vector<KEY_TYPE>&)> assignArray;
		std::function<bool(BGSKeyword*, std::vector<KEY_TYPE>&)> assignSet;
		std::function<bool(BGSKeyword*, std::vector<KEY_TYPE>&)> getArray;
		std::function<bool(BGSKeyword*, std::vector<KEY_TYPE>&)> getSet;
	protected:
		const char* keyTypeName;
		DictsSet dicts;
		virtual void LogLoading() = 0;
		virtual void LogSaving() = 0;
	public:
		DictExport(const char* keyTypeName, KeysArrayExport* keysArrayExport, KeysSetExport* keysSetExport)
			:Iterable(&dicts, DICT_ID)
			,assignArray([keysArrayExport](auto identifier, auto keys){return keysArrayExport->SetArray(identifier, keys);})
			,assignSet([keysSetExport](auto identifier, auto keys){return keysSetExport->SetSet(identifier, keys);})
			,getArray([keysArrayExport](auto identifier, auto keys){return keysArrayExport->GetArray(identifier, keys);})
			,getSet([keysSetExport](auto identifier, auto keys){return keysSetExport->GetSet(identifier, keys);})
			,keyTypeName(keyTypeName)
			,dicts(DICT_ID, Version)
		{}

		virtual bool Delete(BGSKeyword* identifier)
		{
			const bool retVal = dicts.DeleteDict(identifier);
			if (retVal)
				Iterable::iterators.RemoveAllIterators(identifier->formID);
			return retVal;
		}

		virtual SInt32 Size(BGSKeyword* identifier)
		{
			return dicts.Size(identifier);
		}
		virtual bool Contains(BGSKeyword* identifier, KEY_TYPE key)
		{
			return dicts.Contains(identifier, key);
		}
		virtual bool Remove(BGSKeyword* identifier, KEY_TYPE key)
		{
			const bool retVal = dicts.Remove(identifier, key);
			if (retVal)
				Iterable::iterators.RemoveAllIterators(identifier->formID);
			return retVal;
		}
		virtual void Dump(BGSKeyword* identifier)
		{
			dicts.Dump(identifier, 0);
			if (identifier)
				Iterable::iterators.Dump(identifier->formID, 0);
		}

		virtual bool CopyKeysToArray(BGSKeyword* identifier, BGSKeyword* arrayId)
		{
			return dicts.CopyKeysToArray(identifier, arrayId, assignArray);
		}

		virtual bool CopyKeysToSet(BGSKeyword* identifier, BGSKeyword* setId)
		{
			return dicts.CopyKeysToSet(identifier, setId, assignSet);
		}

		virtual SInt32 RemoveKeysByArray(BGSKeyword* identifier, BGSKeyword* arrayId)
		{
			return dicts.RemoveKeysByArray(identifier, arrayId, getArray);
		}

		virtual SInt32 RemoveKeysBySet(BGSKeyword* identifier, BGSKeyword* setId)
		{
			return dicts.RemoveKeysBySet(identifier, setId, getSet);
		}

		// TODO: export to Papyrus
		std::optional<std::vector<KEY_TYPE>> KeysToArray(BGSKeyword* identifier)
		{
			return dicts.KeysToArray(identifier);
		}

		void Clear()
		{
			dicts.Clear();
			Iterable::iterators.Clear();
		}

		bool Save(const F4SESerializationInterface* serializer)
		{
			LogSaving();
			if (dicts.Save(serializer))
			{
				return Iterable::iterators.Save(serializer);
			}
			return true;
		}

		UInt32 DataId() const
		{
			return dicts.DataId;
		}

		bool Load(const F4SESerializationInterface* serializer, UInt32 version, UInt32 length)
		{
			LogLoading();
			if (dicts.Load(serializer, version, length))
				return Iterable::iterators.Load(serializer, dicts.GetVersionsMap());
			return false;
		}
	};
}