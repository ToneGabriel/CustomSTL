#pragma once
#include <cstring>
#include <cwchar>
#include "cUtility.h"


CUSTOM_BEGIN


// char traits implementation
template<class Type, class Integer>
struct _CharTraits
{
	using CharType	= Type;
	using IntType	= Integer;

	static constexpr CharType NULLCHR = CharType();

	static constexpr CharType* copy(CharType* const first1,
									const CharType* const first2,
									const size_t count) noexcept {

		return static_cast<CharType*>(::memcpy(first1, first2, count * sizeof(CharType)));	// return first
	}

	static constexpr CharType* move(CharType* const first1,
									const CharType* const first2,
									const size_t count) noexcept {	// copy with overlap check

		return static_cast<CharType*>(::memmove(first1, first2, count * sizeof(CharType)));	// return first
	}

	static constexpr int compare(	const CharType* first1,
									const CharType* first2,
									size_t count) noexcept {

		// the sign of the result is the sign of
		// the difference between the values of the first pair of bytes
		// that differ in the objects being compared.
		return ::memcmp(first1, first2, count * sizeof(CharType));
	}

	static constexpr size_t length(const CharType* cstr) noexcept {
		size_t count = 0;
		for (; *cstr != NULLCHR; ++count, ++cstr) { /*do nothing*/ }
		return count;
	}

	static constexpr const CharType* find(	const CharType* cstr,
											size_t count,
											const CharType& ch) noexcept {	// look for ch in [cstr, cstr + count)

		for (; 0 < count; --count, ++cstr)
            if (*cstr == ch)
                return cstr;

        return nullptr;
	}

	static constexpr CharType* assign(	CharType* const cstr,
										size_t count,
										const CharType ch) noexcept {	// assign count * ch to [cstr, ...)

		for (CharType* next = cstr; count > 0; --count, ++next)
			*next = ch;

        return cstr;
	}

	static constexpr bool eq(const CharType& left, const CharType& right) noexcept {
		return left == right;
	}

	static constexpr bool lt(const CharType& left, const CharType& right) noexcept {
		return left < right;
	}

	static constexpr CharType to_char_type(const IntType& val) noexcept {
		return static_cast<CharType>(val);
	}

	static constexpr IntType to_int_type(const CharType& ch) noexcept {
		return static_cast<IntType>(ch);
	}

	static constexpr bool eq_int_type(const IntType& left, const IntType& right) noexcept {
		return left == right;
	}

	static constexpr IntType not_eof(const IntType& val) noexcept {
		return val != eof() ? val : !eof();
	}

	static constexpr IntType eof() noexcept {
		return static_cast<IntType>(EOF);
	}
};

template<class Type>
struct _WCharTraits : _CharTraits<Type, unsigned short>	// for char16_t, wchar_t
{
private:
	using _Base = _CharTraits<Type, unsigned short>;

public:
	using CharType	= typename _Base::CharType;
	using IntType	= typename _Base::IntType;

	static constexpr IntType eof() noexcept {
		return static_cast<IntType>(WEOF);
	}
};

// char traits specialization
template<class Type>
struct CharTraits : _CharTraits<Type, long> {};

template<>
struct CharTraits<char> : _CharTraits<char, int> {};

template<>
struct CharTraits<wchar_t> : _WCharTraits<wchar_t> {};

template<>
struct CharTraits<char16_t> : _WCharTraits<char16_t> {};

template<>
struct CharTraits<char32_t> : _CharTraits<char32_t, unsigned int> {};

#ifdef __cpp_char8_t
template<>
struct CharTraits<char8_t> : _CharTraits<char8_t, unsigned int> {};
#endif


CUSTOM_DETAIL_BEGIN

template<class Traits>
constexpr int _traits_cstring_compare(	const typename Traits::CharType* cstringLeft,
										size_t pos, size_t len,
										const typename Traits::CharType* cstringRight,
										size_t subpos, size_t sublen) noexcept {
	
	if (len != sublen)
		return len - sublen;

	return Traits::compare(cstringLeft + pos, cstringRight + subpos, len);	// same length
}

template<class Traits>
constexpr size_t _traits_cstring_find(	const typename Traits::CharType* cstringLeft,
										const typename Traits::CharType* cstringRight,
										size_t pos, size_t len) noexcept {

	// search in [cstringLeft + pos, end) the string [cstringRight, cstringRight + len]

	// last pos from which a substring of length len can be formed
	size_t lastSubstrPos = Traits::length(cstringLeft) - len;

	for (size_t i = pos; i <= lastSubstrPos; ++i)
		if (Traits::compare(cstringLeft + i, cstringRight, len) == 0)
			return i;

    return static_cast<size_t>(-1);	// npos
}

template<class Traits>
constexpr size_t _traits_cstring_rfind(	const typename Traits::CharType* cstringLeft,
										const typename Traits::CharType* cstringRight,
										size_t pos, size_t len) noexcept {

	// search in [cstringLeft, cstringLeft + pos) the string [cstringRight, cstringRight + len]
	// from right to left

	size_t leftSize 		= Traits::length(cstringLeft);
	size_t lastSubstrPos 	= (pos > leftSize) ? leftSize - len : pos;
	// last pos from which a substring of length len can be formed

	for (size_t i = lastSubstrPos; i > 0; --i)
		if (Traits::compare(cstringLeft + i, cstringRight, len) == 0)
			return i;

	if (Traits::compare(cstringLeft, cstringRight, len) == 0)	// case for i == 0
		return static_cast<size_t>(0);

    return static_cast<size_t>(-1);	// npos
}

CUSTOM_DETAIL_END


CUSTOM_END