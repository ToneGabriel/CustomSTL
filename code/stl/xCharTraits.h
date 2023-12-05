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
										size_t subpos, size_t sublen) {
	
	if (len < sublen)
		return -1;
	
	if (len > sublen)
		return 1;

	return Traits::compare(cstringLeft + pos, cstringRight + subpos, len);	// same length
}

CUSTOM_DETAIL_END


CUSTOM_END