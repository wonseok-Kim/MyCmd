#pragma once

#include <cassert>
#include <span>
#include <string>
#include <string_view>
#include <tchar.h>
#include <vector>
#include <Windows.h>

namespace std
{
#ifdef UNICODE
	using tstring = wstring;
	using tstring_view = wstring_view;
#else
	using tstring = std::string;
	using tstring_view = string_view;
#endif // !UNICODE
}