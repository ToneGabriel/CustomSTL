#pragma once
#include "xString.h"
#include "c_functional.h"


CUSTOM_BEGIN

// All string types
using String	= BasicString<char, 	custom::allocator<char>, 		custom::char_traits<char>>;
using WString	= BasicString<wchar_t, 	custom::allocator<wchar_t>, 	custom::char_traits<wchar_t>>;
using U16String = BasicString<char16_t, custom::allocator<char16_t>, 	custom::char_traits<char16_t>>;
using U32String = BasicString<char32_t, custom::allocator<char32_t>, 	custom::char_traits<char32_t>>;
#ifdef __cpp_lib_char8_t
using U8String	= BasicString<char8_t, 	custom::allocator<char8_t>, 	custom::char_traits<char8_t>>;;
#endif


// String inout
inline std::ostream& operator<<(std::ostream& os, const String& string) {
	os << string.c_str();
	return os;
}

inline std::istream& operator>>(std::istream& is, String& string) {
	auto al 	= String::allocator_type();
	auto aux 	= al.allocate(255);			// create cstring
	is.getline(aux, 255);					// get input to cstring
	string 		= custom::move(aux);		// construct custom::String from cstring implicitly, then assign as &&
	al.deallocate(aux, 255);				// free cstring
	return is;
}

// WString inout
inline std::wostream& operator<<(std::wostream& os, const WString& wstring) {
	os << wstring.c_str();
	return os;
}

inline std::wistream& operator>>(std::wistream& is, WString& wstring) {
	auto al 	= WString::allocator_type();
	auto aux 	= al.allocate(255);			// create cstring
	is.getline(aux, 255);					// get input to cstring
	wstring 	= custom::move(aux);		// construct custom::String from cstring implicitly, then assign as &&
	al.deallocate(aux, 255);				// free cstring
	return is;
}

// (optional): implement custom::basic_ios and replace above (won't ever happen)
// template<class Type, class Alloc, class Traits>
// inline std::ostream& operator<<(std::ostream& os, const BasicString<Type, Alloc, Traits>& string) {
// }

// template<class Type, class Alloc, class Traits>
// inline std::istream& operator>>(std::istream& is, BasicString<Type, Alloc, Traits>& string) {
// }


// BasicString Helpers
template<class Type, class Alloc, class Traits>
struct less<BasicString<Type, Alloc, Traits>>		// used by map, set
{
	constexpr bool operator()(	const BasicString<Type, Alloc, Traits>& left,
								const BasicString<Type, Alloc, Traits>& right) const {

		return left.compare(right) < 0;
	}
};

template<class Type, class Alloc, class Traits>
struct hash<BasicString<Type, Alloc, Traits>>
: _BaseHashEnabler<BasicString<Type, Alloc, Traits>, is_char_v<Type>>	// used by unordered_map, unordered_set
{
	static size_t compute_hash(const BasicString<Type, Alloc, Traits>& key) noexcept {
		return detail::_hash_array_representation(key.c_str(), key.size());
	}
};

CUSTOM_END