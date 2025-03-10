#pragma once
#include <cstring>
#include <cwchar>
#include <c_utility.h>


CUSTOM_BEGIN

CUSTOM_DETAIL_BEGIN

// char traits implementation
template<class Type, class Integer>
struct _Char_Traits
{
	using char_type	= Type;
	using int_type	= Integer;

	static constexpr char_type NULLCHR = char_type();

	static constexpr char_type* copy(char_type* const first1,
									const char_type* const first2,
									const size_t count) noexcept
	{
		return static_cast<char_type*>(::memcpy(first1, first2, count * sizeof(char_type)));	// return first
	}

	// copy with overlap check
	static constexpr char_type* move(char_type* const first1,
									const char_type* const first2,
									const size_t count) noexcept
	{
		return static_cast<char_type*>(::memmove(first1, first2, count * sizeof(char_type)));	// return first
	}

	static constexpr int compare(	const char_type* first1,
									const char_type* first2,
									size_t count) noexcept
	{
		// the sign of the result is the sign of
		// the difference between the values of the first pair of bytes
		// that differ in the objects being compared.
		return ::memcmp(first1, first2, count * sizeof(char_type));
	}

	static constexpr size_t length(const char_type* cstr) noexcept
	{
		size_t count = 0;
		for (/*Empty*/; *cstr != NULLCHR; ++count, ++cstr) { /*do nothing*/ }
		return count;
	}

	// look for ch in [cstr, cstr + count)
	static constexpr const char_type* find(	const char_type* cstr,
											size_t count,
											const char_type& ch) noexcept
	{
		for (/*Empty*/; 0 < count; --count, ++cstr)
            if (*cstr == ch)
                return cstr;

        return nullptr;
	}

	// assign count * ch to [cstr, ...)
	static constexpr char_type* assign(	char_type* const cstr,
										size_t count,
										const char_type ch) noexcept
	{
		for (char_type* next = cstr; count > 0; --count, ++next)
			*next = ch;

        return cstr;
	}

	static constexpr bool eq(const char_type& left, const char_type& right) noexcept
	{
		return left == right;
	}

	static constexpr bool lt(const char_type& left, const char_type& right) noexcept
	{
		return left < right;
	}

	static constexpr char_type to_char_type(const int_type& val) noexcept
	{
		return static_cast<char_type>(val);
	}

	static constexpr int_type to_int_type(const char_type& ch) noexcept
	{
		return static_cast<int_type>(ch);
	}

	static constexpr bool eq_int_type(const int_type& left, const int_type& right) noexcept
	{
		return left == right;
	}

	static constexpr int_type not_eof(const int_type& val) noexcept
	{
		return val != eof() ? val : !eof();
	}

	static constexpr int_type eof() noexcept
	{
		return static_cast<int_type>(EOF);
	}
};	// END _Char_Traits

template<class Type>
struct _WChar_Traits : _Char_Traits<Type, unsigned short>	// for char16_t, wchar_t
{
private:
	using _Base = _Char_Traits<Type, unsigned short>;

public:
	using char_type	= typename _Base::char_type;
	using int_type	= typename _Base::int_type;

	static constexpr int_type eof() noexcept
	{
		return static_cast<int_type>(WEOF);
	}
};	// END _WChar_Traits

// helper functions
template<class Traits>
constexpr int _traits_cstring_compare(	const typename Traits::char_type* cstringLeft,
										size_t pos, size_t len,
										const typename Traits::char_type* cstringRight,
										size_t subpos, size_t sublen) noexcept
{	
	if (len != sublen)
		return len - sublen;

	return Traits::compare(cstringLeft + pos, cstringRight + subpos, len);	// same length
}

template<class Traits>
constexpr size_t _traits_cstring_find(	const typename Traits::char_type* cstringLeft,
										const typename Traits::char_type* cstringRight,
										size_t pos, size_t len) noexcept
{
	// search in [cstringLeft + pos, end) the string [cstringRight, cstringRight + len]

	// last pos from which a substring of length len can be formed
	size_t lastSubstrPos = Traits::length(cstringLeft) - len;

	for (size_t i = pos; i <= lastSubstrPos; ++i)
		if (Traits::compare(cstringLeft + i, cstringRight, len) == 0)
			return i;

    return static_cast<size_t>(-1);	// npos
}

template<class Traits>
constexpr size_t _traits_cstring_rfind(	const typename Traits::char_type* cstringLeft,
										const typename Traits::char_type* cstringRight,
										size_t pos, size_t len) noexcept
{
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


// char traits specialization
template<class Type>
struct char_traits : detail::_Char_Traits<Type, long> {};

template<>
struct char_traits<char> : detail::_Char_Traits<char, int> {};

template<>
struct char_traits<wchar_t> : detail::_WChar_Traits<wchar_t> {};

template<>
struct char_traits<char16_t> : detail::_WChar_Traits<char16_t> {};

template<>
struct char_traits<char32_t> : detail::_Char_Traits<char32_t, unsigned int> {};

#ifdef __cpp_char8_t
template<>
struct char_traits<char8_t> : detail::_Char_Traits<char8_t, unsigned int> {};
#endif


CUSTOM_END