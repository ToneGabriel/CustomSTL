#pragma once
#include "xString.h"


CUSTOM_BEGIN

// All string view types
using StringView	= BasicStringView<char,     custom::CharTraits<char>>;
using WStringView	= BasicStringView<wchar_t, 	custom::CharTraits<wchar_t>>;
using U16StringView = BasicStringView<char16_t, custom::CharTraits<char16_t>>;
using U32StringView = BasicStringView<char32_t, custom::CharTraits<char32_t>>;
#ifdef __cpp_lib_char8_t
using U8StringView	= BasicStringView<char8_t, 	custom::CharTraits<char8_t>>;;
#endif

CUSTOM_END