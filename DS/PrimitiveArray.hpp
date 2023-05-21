#pragma once

#include "Array.hpp"
#include "PrimitiveHandler.hpp"
#include "Shared/Comparer.hpp"
#include "FormComparer.hpp"

namespace DS
{
	template <typename T>
	class PrimitiveArray final : public Array<T, PrimitiveHandler<T>>
	{
		std::equal_to<T> comparer;
	public:
		PrimitiveArray(BGSKeyword* identifier) : Array(identifier)
		{

		}
		PrimitiveArray() = default;

		SInt32 IndexOf(T& value, SInt32 startIndex)
		{
			if (startIndex < 0)
				return -1;
			BSReadLocker lock(&dataLock);

			for (SInt32 i = startIndex; i < data.size(); i++)
			{
				if (comparer(data[i],value))
					return i;
			}
			return -1;
		}

		void Sort(UInt32 mode)
		{
			BSWriteLocker lock(&dataLock);
			if (mode % 2 == 0) // ascending
			{
				std::sort(data.begin(), data.end(), ComparerLess<T>());
			}
			else // descending
			{
				std::sort(data.begin(), data.end(), ComparerGreater<T>());
			}
			version++;
		}
	};
}