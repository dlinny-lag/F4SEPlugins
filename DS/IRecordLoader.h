#pragma once
#include "f4se/Serialization.h"

namespace DS
{
	struct IRecordLoader
	{
		virtual UInt32 DataId() const = 0;
		virtual bool Load(const F4SESerializationInterface* serializer, UInt32 version, UInt32 length) = 0;
		virtual ~IRecordLoader() = default;
	};
}