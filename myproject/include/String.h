#pragma once
#include "xString.h"
#include "Functional.h"


CUSTOM_BEGIN

// All string types
using String	= BasicString<char, custom::Allocator<char>, custom::CharTraits<char>>;
using WString	= BasicString<wchar_t, custom::Allocator<wchar_t>, custom::CharTraits<wchar_t>>;
using U16String = BasicString<char16_t, custom::Allocator<char16_t>, custom::CharTraits<char16_t>>;
using U32String = BasicString<char32_t, custom::Allocator<char32_t>, custom::CharTraits<char32_t>>;
#ifdef __cpp_lib_char8_t
using U8String	= BasicString<char8_t, custom::Allocator<char8_t>, custom::CharTraits<char8_t>>;;
#endif


// TODO: fix ops
template<class Type, class Alloc, class Traits>
constexpr std::ostream& operator<<(std::ostream& os, const BasicString<Type, Alloc, Traits>& string) {
	os << string.data();
	return os;
}

template<class Type, class Alloc, class Traits>
constexpr std::istream& operator>>(std::istream& is, BasicString<Type, Alloc, Traits>& string) {
	string.clear();
	string.reserve(255);
	is.getline(string.data(), 255);
	return is;
}


// BasicString Helpers
template<class Type, class Alloc, class Traits>
struct Less<BasicString<Type, Alloc, Traits>>
{
	constexpr bool operator()(	const BasicString<Type, Alloc, Traits>& left,
								const BasicString<Type, Alloc, Traits>& right) const {

		return left.compare(right) < 0;
	}
};

template<class Type, class Alloc, class Traits>
struct Hash<BasicString<Type, Alloc, Traits>> : _BaseHashEnabler<BasicString<Type, Alloc, Traits>, IsChar_v<Type>>
{
	static size_t compute_hash(const BasicString<Type, Alloc, Traits>& key) noexcept {
		return _hash_array_representation(key.c_str(), key.size());
	}
};

CUSTOM_END