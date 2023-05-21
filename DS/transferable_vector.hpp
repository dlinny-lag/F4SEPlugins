#pragma once
#include <vector>
#include <f4se/GameAPI.h>

namespace DSAPI
{
#pragma pack(push)
#pragma pack(8)
	template <typename T>
	class transferable_vector final
	{
	private:
		size_t size;
		T* data;
	public:
		transferable_vector(const std::vector<T>& data) : size(data.size()) ,data(nullptr)
		{
			if (size > 0)
			{
				this->data = static_cast<T*>(Heap_Allocate(sizeof(T) * size));
				std::copy(data.begin(), data.end(), this->data);
			}
		}
		operator std::vector<T>()
		{
			if (size == 0)
				return std::vector<T>();
			return std::vector<T> (data, data+size);
		}

		~transferable_vector()
		{
			if (data)
				Heap_Free(data);
		}

		transferable_vector(const transferable_vector& ) = delete;
		transferable_vector& operator=(const transferable_vector& ) = delete;

		transferable_vector& operator=(transferable_vector&& other) noexcept
		{
			data=other.data;
			size=other.size;
			other.data = nullptr;
			other.size = 0;
			return *this;
		}

		transferable_vector(transferable_vector&& other) noexcept
			: size(other.size), data(other.data), other.data(nullptr), other.size(0) {}
	};
#pragma pack(pop)
	namespace verification
	{
		constexpr int Expected = 8+8;
		template <int> struct SizeTest;
		template<> struct SizeTest<Expected>{};
		template struct SizeTest<sizeof(transferable_vector<void*>)>;
		template struct SizeTest<sizeof(transferable_vector<double>)>;
		template struct SizeTest<sizeof(transferable_vector<float>)>;
		static_assert(sizeof(transferable_vector<void*>) == Expected);
		static_assert(sizeof(transferable_vector<double>) == Expected);
		static_assert(sizeof(transferable_vector<float>) == Expected);
		static_assert(sizeof(transferable_vector<BSFixedString>) == Expected);
		
	};
}