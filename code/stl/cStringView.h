#pragma once
#include "xString.h"
#include "cFunctional.h"


CUSTOM_BEGIN

// All string view types
using StringView	= BasicStringView<char,     custom::CharTraits<char>>;
using WStringView	= BasicStringView<wchar_t, 	custom::CharTraits<wchar_t>>;
using U16StringView = BasicStringView<char16_t, custom::CharTraits<char16_t>>;
using U32StringView = BasicStringView<char32_t, custom::CharTraits<char32_t>>;
#ifdef __cpp_lib_char8_t
using U8StringView	= BasicStringView<char8_t, 	custom::CharTraits<char8_t>>;;
#endif


// StringView out
inline std::ostream& operator<<(std::ostream& os, const StringView& stringView) {
	os << stringView.data();
	return os;
}


// WStringView inout
inline std::wostream& operator<<(std::wostream& os, const WStringView& wstringView) {
	os << wstringView.data();
	return os;
}


// BasicStringView Helpers
template<class Type, class Traits>
struct Less<BasicStringView<Type, Traits>>		// used by Map, Set
{
	constexpr bool operator()(	const BasicStringView<Type, Traits>& left,
								const BasicStringView<Type, Traits>& right) const {

		return left.compare(right) < 0;
	}
};

template<class Type, class Traits>
struct Hash<BasicStringView<Type, Traits>>
: _BaseHashEnabler<BasicStringView<Type, Traits>, IsChar_v<Type>>	// used by UnorderedMap, UnorderedSet
{
	static size_t compute_hash(const BasicStringView<Type, Traits>& key) noexcept {
		return detail::_hash_array_representation(key.data(), key.size());
	}
};

CUSTOM_END