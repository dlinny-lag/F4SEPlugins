#pragma once
#include "f4se/Serialization.h"
#include <functional>

namespace DS
{
	class LoadManager
	{
	public:
		static void RegisterRecordLoader(UInt32 recordTag, std::function<bool(const F4SESerializationInterface* serializer, UInt32 version, UInt32 length)> recordLoader);
		static void LoadRecords(const F4SESerializationInterface* serializer);
	};
}