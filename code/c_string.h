#pragma once
#include <x_string.h>
#include <c_functional.h>


CUSTOM_BEGIN

// All string types
using string	= basic_string<char,		custom::allocator<char>,		custom::char_traits<char>>;
using wstring	= basic_string<wchar_t,		custom::allocator<wchar_t>,		custom::char_traits<wchar_t>>;
using u16string = basic_string<char16_t,	custom::allocator<char16_t>,	custom::char_traits<char16_t>>;
using u32string = basic_string<char32_t,	custom::allocator<char32_t>,	custom::char_traits<char32_t>>;
#ifdef __cpp_lib_char8_t
using u8string	= basic_string<char8_t,		custom::allocator<char8_t>,		custom::char_traits<char8_t>>;;
#endif


// string inout
inline std::ostream& operator<<(std::ostream& os, const string& string)
{
	os << string.c_str();
	return os;
}

inline std::istream& operator>>(std::istream& is, string& string)
{
	auto al 	= string::allocator_type();
	auto aux 	= al.allocate(255);			// create cstring
	is.getline(aux, 255);					// get input to cstring
	string 		= custom::move(aux);		// construct custom::string from cstring implicitly, then assign as &&
	al.deallocate(aux, 255);				// free cstring
	return is;
}

// wstring inout
inline std::wostream& operator<<(std::wostream& os, const wstring& wstring)
{
	os << wstring.c_str();
	return os;
}

inline std::wistream& operator>>(std::wistream& is, wstring& wstring)
{
	auto al 	= wstring::allocator_type();
	auto aux 	= al.allocate(255);			// create cstring
	is.getline(aux, 255);					// get input to cstring
	wstring 	= custom::move(aux);		// construct custom::string from cstring implicitly, then assign as &&
	al.deallocate(aux, 255);				// free cstring
	return is;
}

// (optional): implement custom::basic_ios and replace above (won't ever happen)
// template<class Type, class Alloc, class Traits>
// inline std::ostream& operator<<(std::ostream& os, const basic_string<Type, Alloc, Traits>& string) {
// }

// template<class Type, class Alloc, class Traits>
// inline std::istream& operator>>(std::istream& is, basic_string<Type, Alloc, Traits>& string) {
// }


// basic_string Helpers
template<class Type, class Alloc, class Traits>
struct less<basic_string<Type, Alloc, Traits>>		// used by map, set
{
	constexpr bool operator()(	const basic_string<Type, Alloc, Traits>& left,
								const basic_string<Type, Alloc, Traits>& right) const
	{
		return left.compare(right) < 0;
	}
};

template<class Type, class Alloc, class Traits>
struct hash<basic_string<Type, Alloc, Traits>>
: detail::_Base_Hash_Enabler<basic_string<Type, Alloc, Traits>, is_char_v<Type>>	// used by unordered_map, unordered_set
{
	static size_t compute_hash(const basic_string<Type, Alloc, Traits>& key) noexcept
	{
		return detail::_hash_array_representation(key.c_str(), key.size());
	}
};

CUSTOM_END