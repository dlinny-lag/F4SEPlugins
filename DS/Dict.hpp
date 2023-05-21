#pragma once
#include "f4se/GameForms.h"
#include "f4se/PluginAPI.h"
#include <vector>
#include "Identifiable.h"

#include "MyTrace.h"
#include "Indent.h"
#include "KeyValidator.hpp"

namespace DS
{
	template <typename TKey, typename TValue, typename ValueHandler, typename MapType>
	class Dict : public Identifiable
	{
	protected:
		UInt64 version;
		MapType data;
		BSReadWriteLock dataLock;
		ValueHandler valueHandler;

	public:
		Dict(BGSKeyword* identifier) 
			:Identifiable(identifier)
			,version(0)
		{

		}
		UInt64 Version()
		{
			BSReadLocker lock(&dataLock);
			return version;
		}

		SInt32 Size()
		{
			BSReadLocker lock(&dataLock);
			return data.size();
		}

		bool Contains(TKey& key)
		{
			BSWriteLocker lock(&dataLock);
			return data.find(key) != data.end();
		}
		bool Remove(TKey& key)
		{
			BSWriteLocker lock(&dataLock);
			return data.erase(key) > 0;
		}
		void Clear()
		{
			BSWriteLocker lock(&dataLock);
			data.clear();
		}

		bool SaveData(const F4SESerializationInterface* serializer, bool allowNoId)
		{
			Validate();
			const UInt32 len = data.size();
			if (!serializer->WriteRecordData(&len, sizeof(len)))
				return FE("Dict: Failed to write length (%08X)", FormId());

			if (!serializer->WriteRecordData(&version, sizeof(version)))
				return FE("Dict: Failed to write data version (%08X)", FormId());

			for (auto& pair : data)
			{
				if (!SaveKey(serializer, &pair.first))
					return FE("Dict: Failed to save key (%08X)", FormId());
				if (!valueHandler.SaveValue(serializer, &pair.second))
					return FE("Dict: Failed to save value (%08X)", FormId());
			}
			return true;
		}

		bool LoadData(const F4SESerializationInterface* serializer, bool allowNoId)
		{
			UInt32 len;

			if (!serializer->ReadRecordData(&len, sizeof(len)))
				return FE("Dict: Failed to read length (%08X)", FormId());

			if (!serializer->ReadRecordData(&version, sizeof(version)))
				return FE("Dict: Failed to read data version (%08X)", FormId());

			for (UInt32 i = 0; i < len; i++)
			{
				TKey key;
				bool keyValid = false;
				if (!LoadKey(serializer, &key, keyValid))
					return FW("Dict (%08X): Failed to load key", FormId());

				TValue val;
				bool valueValid = false;
				if (!valueHandler.LoadValue(serializer, &val, valueValid))
					return FW("Dict (%08X): Failed to load value ", FormId());

				if (!valueValid) // invalid value still value
				{
					W("Dict (%08X):Invalid value read", FormId());
					version++;  // thin thing - collection is actually changed, so we have to change data version to be sure that iterators will fail to continue
				}

				if (keyValid)
					data.insert(std::make_pair(key, val));
				else
					W("Dict (%08X): Invalid key read", FormId());
			}

			return true;
		}

		void DumpAll(int indent)
		{
			BSReadLocker lock(&dataLock);
			Dmp("%sDict (%08X). Version %d", Indent::Get(indent), FormId(), version);
			indent += 2;
			for (auto& pair : data)
			{
				DumpKey(indent, pair.first);
				valueHandler.DumpValue(indent, pair.second);
			}
		}

		void Validate()
		{
			BSReadLocker lock(&dataLock);
			std::vector<TKey> toRemove;
			for (auto& pair : data)
			{
				if (!KeyValidator<TKey>::IsValid(pair.first))
				{
					toRemove.emplace_back(pair.first);
					++version;
					W("Invalid value found in Dict %08X", FormId());
				}
			}
			for (const auto invalid : toRemove)
			{
				data.erase(invalid);
			}
		}

		void GetState(std::vector<TKey>& keys, UInt64& curVersion)
		{
			BSWriteLocker lock(&dataLock);
			std::vector<TKey> toRemove;
			for (auto& pair : data)
			{
				if (KeyValidator<TKey>::IsValid(pair.first))
					keys.push_back(pair.first);
				else
				{
					toRemove.emplace_back(pair.first);
					++version;
					W("Invalid value found in Dict %08X", FormId());
				}
			}
			for (const auto invalid : toRemove)
			{
				data.erase(invalid);
			}
			curVersion = version;
		}

	private:
		bool SaveKey(const F4SESerializationInterface* serializer, const TKey* val) const
		{
			return ValueHelper::Save(serializer, val);
		}
		bool LoadKey(const F4SESerializationInterface* serializer, TKey* outVal, bool& keyValid)
		{
			if (!ValueHelper::Load(serializer, outVal))
				return FE("Failed to read key");

			keyValid = KeyValidator<TKey>::IsValid(outVal);
			return true;
		}
		void DumpKey(int indent, const TKey& value)
		{
			ValueHelper::Dump(indent, value);
		}
	};
};