#pragma once
#include "f4se/Serialization.h"
#include <functional>
#include "IRecordLoader.h"

namespace DS
{
	class LoadManager
	{
	public:
		static void RegisterRecordLoader(IRecordLoader* loader);
		static void LoadRecords(const F4SESerializationInterface* serializer);
	};
}