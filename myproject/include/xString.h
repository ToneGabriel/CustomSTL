#pragma once
#include "xMemory.h"
#include "Utility.h"
#include "Iterator.h"

#include <cstring>
#include <cwchar>


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
									const size_t count) noexcept {

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

		return static_cast<const CharType*>(::memchr(cstr, ch, count * sizeof(CharType)));
	}

	static constexpr CharType* assign(	CharType* const cstr,
										size_t count,
										const CharType ch) noexcept {	// assign count * ch to [cstr, ...)

		return static_cast<CharType*>(::memset(cstr, ch, count * sizeof(CharType)));
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
struct _WCharTraits : public _CharTraits<Type, unsigned short>	// for char16_t, wchar_t
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


template<class Type, class Alloc>
struct _BasicStringData
{
	using _AllocTraits		= AllocatorTraits<Alloc>;

	using ValueType			= typename _AllocTraits::ValueType;
	using Reference			= typename _AllocTraits::Reference;
	using ConstReference	= typename _AllocTraits::ConstReference;
	using Pointer			= typename _AllocTraits::Pointer;
	using ConstPointer		= typename _AllocTraits::ConstPointer;


	Pointer _First			= nullptr;			// Actual container array
	Pointer _Last			= nullptr;			// Pointer to end
	Pointer _Final			= nullptr;			// Pointer to capacity end
};


template<class Type, class Alloc = custom::Allocator<Type>, class Traits = custom::CharTraits<Type>>
class BasicString		// null-terminated array of elements
{
private:
	using _Data					= _BasicStringData<Type, Alloc>;
	using _AllocTraits			= typename _Data::_AllocTraits;

public:
	static_assert(IsSame_v<Type, typename Alloc::ValueType>, "Object type and Allocator type must be the same!");
	static_assert(!IsArray_v<Type> && IsTrivial_v<Type> && IsStandardLayout_v<Type>,
					"The character type of BasicString must be a non-array trivial standard-layout type.");

	using TraitsType			= Traits;
	using ValueType				= typename _Data::ValueType;
	using Reference				= typename _Data::Reference;
	using ConstReference		= typename _Data::ConstReference;
	using Pointer				= typename _Data::Pointer;
	using ConstPointer			= typename _Data::ConstPointer;
	using AllocatorType			= Alloc;

	using Iterator				= VectorIterator<_Data>;					// Iterator type
	using ConstIterator			= VectorConstIterator<_Data>;				// Const Iterator type
	using ReverseIterator		= custom::ReverseIterator<Iterator>;		// ReverseIterator type
	using ConstReverseIterator	= custom::ReverseIterator<ConstIterator>;	// Const Reverse Iterator type

	static constexpr size_t npos = static_cast<size_t>(-1);

private:
	static constexpr size_t _ISTREAM_CAPACITY = 255;
	static constexpr size_t _DEFAULT_CAPACITY = 15;

	_Data _data;
	AllocatorType _alloc;

public:
	// Constructors

	BasicString() {
		_initialize_from_cstring(nullptr);
	}

	BasicString(ConstPointer cstring) {
		_initialize_from_cstring(cstring);
	}

	BasicString(const size_t& newCapacity) {
		_alloc_empty(newCapacity);
	}

	BasicString(const BasicString& other) {
		_copy(other);
	}

	BasicString(BasicString&& other) noexcept {
		_move(custom::move(other));
	}

	~BasicString() {
		_alloc.deallocate(_data._First, capacity() + 1);
	}

public:
	// Operators

public:
	// Main functions

	const size_t capacity() const {
		return static_cast<size_t>(_data._Final - _data._First);
	}

private:
	// Helpers

	void _alloc_empty(const size_t& capacity) {
		_data._First	= _alloc.allocate(capacity + 1);
		_data._Last		= _data._First;
		_data._Final	= _data._First + capacity;
		_data._Last[0]	= TraitsType::NULLCHR;
	}

	void _initialize_from_cstring(ConstPointer cstring) {
		if (cstring == nullptr)
		{
			_data._First	= _alloc.allocate(_DEFAULT_CAPACITY);
			_data._Last		= _data._First;
			_data._Final	= _data._First;
		}
		else
		{
			size_t len		= TraitsType::length(cstring);
			_data._First	= _alloc.allocate(len + 1);
			_data._Last		= _data._First + len;
			_data._Final	= _data._First + len;
			TraitsType::copy(_data._First, cstring, len);
		}

		_data._Last[0] = TraitsType::NULLCHR;
	}

	void _copy(const String& other) {									// Generic copy function for string
		_alloc_empty(other.capacity());
		TraitsType::copy(_data._First, other._data._First, other.size());

		_data._Last		= _data._First + other.size();
		_data._Final	= _data._First + other.capacity();
		_data._Last[0]	= TraitsType::NULLCHR;
	}

	void _move(String&& other) {										// Generic move function for string
		_data._First	= other._data._First;
		_data._Last		= other._data._Last;
		_data._Final	= other._data._Final;

		other._initialize_from_cstring(nullptr);
	}
};


// All string types
//using String	= BasicString<char, custom::Allocator<char>, custom::CharTraits<char>>;
//using WString	= BasicString<wchar_t, custom::Allocator<wchar_t>, custom::CharTraits<wchar_t>>;
//using U16String = BasicString<char16_t, custom::Allocator<char16_t>, custom::CharTraits<char16_t>>;
//using U32String = BasicString<char32_t, custom::Allocator<char32_t>, custom::CharTraits<char32_t>>;
//#ifdef __cpp_lib_char8_t
//using U8String	= BasicString<char8_t, custom::Allocator<char8_t>, custom::CharTraits<char8_t>>;;
//#endif

CUSTOM_END