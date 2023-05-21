#pragma once
#include "f4se/GameForms.h"
#include <algorithm>
#include <string_view>
#include "MyTrace.h"

namespace std
{
	template <>
	struct hash<BSFixedString>
	{
		std::hash<nullptr_t> nullHash;
		std::hash<std::string_view> stringHash;
		std::hash<std::wstring_view> wstringHash;
		_NODISCARD size_t operator()(const BSFixedString& str) const noexcept
		{
			if (str.data == nullptr)
				return nullHash(nullptr);

			if (str.data->IsWide())
				return wstringHash(std::wstring_view(str.data->Get<wchar_t>()));

			return stringHash(std::string_view(str.data->Get<char>()));
		}
	};
}
