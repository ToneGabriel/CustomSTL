#pragma once
#include "x_string.h"
#include "c_functional.h"


CUSTOM_BEGIN

// All string view types
using string_view		= basic_string_view<char,		custom::char_traits<char>>;
using wstring_view		= basic_string_view<wchar_t,	custom::char_traits<wchar_t>>;
using u16string_view	= basic_string_view<char16_t,	custom::char_traits<char16_t>>;
using u32string_view	= basic_string_view<char32_t,	custom::char_traits<char32_t>>;
#ifdef __cpp_lib_char8_t
using u8string_view		= basic_string_view<char8_t,	custom::char_traits<char8_t>>;;
#endif


// string_view out
inline std::ostream& operator<<(std::ostream& os, const string_view& stringView) {
	os << stringView.data();
	return os;
}


// wstring_view inout
inline std::wostream& operator<<(std::wostream& os, const wstring_view& wstringView) {
	os << wstringView.data();
	return os;
}


// basic_string_view Helpers
template<class Type, class Traits>
struct less<basic_string_view<Type, Traits>>		// used by map, set
{
	constexpr bool operator()(	const basic_string_view<Type, Traits>& left,
								const basic_string_view<Type, Traits>& right) const {

		return left.compare(right) < 0;
	}
};

template<class Type, class Traits>
struct hash<basic_string_view<Type, Traits>>
: detail::_Base_Hash_Enabler<basic_string_view<Type, Traits>, is_char_v<Type>>	// used by unordered_map, unordered_set
{
	static size_t compute_hash(const basic_string_view<Type, Traits>& key) noexcept {
		return detail::_hash_array_representation(key.data(), key.size());
	}
};

CUSTOM_END