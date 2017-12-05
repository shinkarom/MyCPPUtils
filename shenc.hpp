#pragma once

#include <io.h>
#include <fcntl.h>
#include <iostream>
#include <codecvt>

inline void SetUTF16Output()
{
	_setmode(_fileno(stdout), _O_U16TEXT);
	_setmode(_fileno(stdin), _O_U16TEXT);
	_setmode(_fileno(stderr), _O_U16TEXT);
}

inline auto GetUTF8Locale()
{
	return std::locale(std::locale::empty(), new std::codecvt_utf8<wchar_t>);
}

inline void SetUTF8Global()
{
	std::locale::global(GetUTF8Locale());
}