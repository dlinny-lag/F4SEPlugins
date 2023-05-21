#pragma once
#include "f4se/PapyrusValue.h"
#include "BSFixedStringEqual.hpp"
#include "MyTrace.h"
#include "StructValueHelper.hpp"

namespace std
{
	template <>
	struct equal_to<VMValue>
	{
		bool operator()(const VMValue& a, const VMValue& b) const
		{
			if (a.type.value != b.type.value) // strong type check
				return FD("a.type(%016X) != b.type(%016X)", a.type.value, b.type.value);

			if (a.type.value > VMValue::kType_ArrayEnd && b.type.value > VMValue::kType_ArrayEnd)
			{
				if (a.data.id == b.data.id)
					return true; // same object
				// probably we comparing struct objects. TODO: optimize
				VMStructTypeInfo* aStruct = StructValueHelper::GetStructType(a);
				VMStructTypeInfo* bStruct = StructValueHelper::GetStructType(b);
				if (aStruct == nullptr || bStruct == nullptr)
				{ // Form object
					if (a.data.id == nullptr || b.data.id == nullptr)
						return false;
					// TODO: race condition - m_handle might be wrong
					// both should be locked before comparing
					return a.data.id->m_handle == b.data.id->m_handle; 
				}

				// we comparing struct instances
				if (aStruct != bStruct)
					return false;
				// compare fields
				const VMValue* aValues = a.data.strct->GetStruct();
				const VMValue* bValues = b.data.strct->GetStruct();
				// aValues and bValues can't point to the same block
				const UInt32 membersCount = aStruct->m_members.Size();
				for(UInt32 i = 0; i < membersCount; i++)
				{
					if (!equal_to<VMValue>()(aValues[i], bValues[i]))
						return false;
				}
				return true;
			}
			
			D("Compare values of type %016X", a.type.value);
			switch (a.type.value)
			{
			case VMValue::kType_None:
				return true; // both are None
			case VMValue::kType_Int:
				return a.data.i == b.data.i;
			case VMValue::kType_Float:
				return a.data.f == b.data.f;
			case VMValue::kType_Bool:
				return a.data.b == b.data.b;
			case VMValue::kType_String:
				return std::equal_to()(*a.data.GetStr(), *b.data.GetStr());
			}

			// TODO: arrays are not supported yet

			return false;
		}
	};
}