#pragma once
#include "xString.h"
#include "cFunctional.h"


CUSTOM_BEGIN

// All string types
using String	= BasicString<char, 	custom::Allocator<char>, 		custom::CharTraits<char>>;
using WString	= BasicString<wchar_t, 	custom::Allocator<wchar_t>, 	custom::CharTraits<wchar_t>>;
using U16String = BasicString<char16_t, custom::Allocator<char16_t>, 	custom::CharTraits<char16_t>>;
using U32String = BasicString<char32_t, custom::Allocator<char32_t>, 	custom::CharTraits<char32_t>>;
#ifdef __cpp_lib_char8_t
using U8String	= BasicString<char8_t, 	custom::Allocator<char8_t>, 	custom::CharTraits<char8_t>>;;
#endif


// String inout
inline std::ostream& operator<<(std::ostream& os, const String& string) {
	os << string.c_str();
	return os;
}

inline std::istream& operator>>(std::istream& is, String& string) {
	auto al 	= String::AllocatorType();
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
	auto al 	= WString::AllocatorType();
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
struct Less<BasicString<Type, Alloc, Traits>>		// used by Map, Set
{
	constexpr bool operator()(	const BasicString<Type, Alloc, Traits>& left,
								const BasicString<Type, Alloc, Traits>& right) const {

		return left.compare(right) < 0;
	}
};

template<class Type, class Alloc, class Traits>
struct Hash<BasicString<Type, Alloc, Traits>>
: _BaseHashEnabler<BasicString<Type, Alloc, Traits>, IsChar_v<Type>>	// used by UnorderedMap, UnorderedSet
{
	static size_t compute_hash(const BasicString<Type, Alloc, Traits>& key) noexcept {
		return detail::_hash_array_representation(key.c_str(), key.size());
	}
};

CUSTOM_END