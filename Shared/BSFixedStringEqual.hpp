#pragma once
#include "f4se/GameForms.h"
#include "MyTrace.h"

namespace std
{
	template <>
	struct equal_to<BSFixedString>
	{
		bool operator()(const BSFixedString& a, const BSFixedString& b) const
		{
			if (a.data == b.data)
				return true;
			if (a.data == nullptr)
				return false;

			// TODO: ability to compare different types of strings

			if (a.data->IsWide() != b.data->IsWide())
				return FD("Wide-ness of strings is different!");

			if (a.data->IsWide())
				return wcscmp(a.data->Get<wchar_t>(), b.data->Get<wchar_t>()) == 0;

			return strcmp(a.data->Get<char>(), b.data->Get<char>()) == 0;
		}
	};
}
