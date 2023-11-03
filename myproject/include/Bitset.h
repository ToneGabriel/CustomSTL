#pragma once
#include "Limits.h"


CUSTOM_BEGIN

template<size_t Bits>
class Bitset
{
	// TODO: implement
private:
	using Type = Conditional_t<Bits <= sizeof(unsigned long) * CHAR_BIT,
									unsigned long,
									unsigned long long>;

	static constexpr ptrdiff_t _BITS_PER_WORD = CHAR_BIT * sizeof(Type);
	static constexpr ptrdiff_t _WORDS = Bits == 0 ? 0 : (Bits - 1) / _BITS_PER_WORD;

	Type _array[_WORDS + 1];

public:
	class Reference;	// proxy for an element

    constexpr Bitset() noexcept : _array() { /*Empty*/ } // construct with all false values

public:
	// Operators

	constexpr bool operator[](size_t pos) const {
        return Bits <= pos ? (_validate(pos), false) : _subscript(pos);
    }

    constexpr Reference operator[](const size_t pos) noexcept {
        _validate(pos);
        return Reference(*this, pos);
    }

private:

    static constexpr void _validate(const size_t pos) noexcept {
        CUSTOM_ASSERT(pos < Bits, "Bitset index outside range");
    }

    constexpr bool _subscript(size_t pos) const {
        return (_array[pos / _BITS_PER_WORD] & (Type{1} << pos % _BITS_PER_WORD)) != 0;
    }

	constexpr Bitset& _flip(const size_t pos) noexcept { // flip bit at pos, no checking
        _array[pos / _BITS_PER_WORD] ^= Type{1} << pos % _BITS_PER_WORD;
        return *this;
    }

	constexpr bitset& _set(const size_t pos, const bool val) noexcept {	// set bit at pos to val, no checking
        auto& word 		= _array[pos / _BITS_PER_WORD];
        const auto bit 	= Type{1} << pos % _BITS_PER_WORD;
        
		if (val)
            word |= bit;
        else
            word &= ~bit;

        return *this;
    }

};	// END Bitset


template<size_t Bits>
class Bitset<Bits>::Reference
{
private:
	friend Bitset;

	Bitset<Bits>* _bitsetRef;
	size_t _position;

private:
	constexpr Reference() noexcept
		: _bitsetRef(nullptr), _position(0) { /*Empty*/ }

	constexpr Reference(Bitset<Bits>& bitset, const size_t pos) noexcept
		: _bitsetRef(&bitset), _position(pos) { /*Empty*/ }

public:
	constexpr Reference(const Reference&) = default;

	constexpr ~Reference() noexcept { /*Empty*/ }

	constexpr Reference& operator=(const bool val) noexcept {
		_bitsetRef->_set(_position, val);
		return *this;
	}

	constexpr Reference& operator=(const Reference& bitRef) noexcept {
		_bitsetRef->_set(_position, static_cast<bool>(bitRef));
		return *this;
	}

	constexpr bool operator~() const noexcept {
		return !_bitsetRef->_subscript(_position);
	}

	constexpr operator bool() const noexcept {
		return _bitsetRef->_subscript(_position);
	}

	constexpr Reference& flip() noexcept {
		_bitsetRef->_flip(_position);
		return *this;
	}
};	// END Reference

CUSTOM_END
