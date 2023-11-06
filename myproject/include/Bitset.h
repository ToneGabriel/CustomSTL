#pragma once
#include "Bit.h"
#include "Limits.h"


CUSTOM_BEGIN

template<size_t Bits>
class Bitset
{
private:
	using Type = Conditional_t<Bits <= sizeof(unsigned long) * CHAR_BIT,
									unsigned long,
									unsigned long long>;

	static constexpr ptrdiff_t _BITS_PER_WORD 	= CHAR_BIT * sizeof(Type);
	static constexpr ptrdiff_t _WORDS 			= Bits == 0 ? 0 : (Bits - 1) / _BITS_PER_WORD;

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

    constexpr Bitset& operator&=(const Bitset& other) noexcept {
        for (size_t wordPos = 0; wordPos <= _WORDS; ++wordPos)
            _array[wordPos] &= other._array[wordPos];

        return *this;
    }

    constexpr Bitset& operator|=(const Bitset& other) noexcept {
        for (size_t wordPos = 0; wordPos <= _WORDS; ++wordPos)
            _array[wordPos] |= other._array[wordPos];

        return *this;
    }

    constexpr Bitset& operator^=(const Bitset& other) noexcept {
        for (size_t wordPos = 0; wordPos <= _WORDS; ++wordPos)
            _array[wordPos] ^= other._array[wordPos];

        return *this;
    }

	constexpr Bitset& operator<<=(size_t pos) noexcept {
		// TODO: implement
		return *this;
	}

	constexpr Bitset& operator>>=(size_t pos) noexcept {
		// TODO: implement
		return *this;
	}

    constexpr Bitset operator<<(const size_t pos) const noexcept {
        Bitset temp = *this;
        temp <<= pos;
        return temp;
    }

    constexpr Bitset operator>>(const size_t pos) const noexcept {
        Bitset temp = *this;
        temp >>= pos;
        return temp;
    }

    constexpr Bitset operator~() const noexcept { // flip all bits
        Bitset temp = *this;
        temp.flip();
        return temp;
    }

	template<size_t _Bits>
	friend bool operator==(const Bitset<_Bits>& left, const Bitset<_Bits>& right);

public:
	// Main functions

	constexpr Bitset& set() {	// set all bits true
		::memset(&_array, 0xFF, sizeof(_array));
		_trim();
		return *this;
	}

    constexpr Bitset& set(const size_t pos, bool val = true) { // set bit at pos to val
        if (Bits <= pos)
            throw std::out_of_range("Invalid Bitset position");

        return _set(pos, val);
    }

    constexpr Bitset& reset() noexcept { // set all bits false
        ::memset(&_array, 0, sizeof(_array));
        return *this;
    }

    constexpr Bitset& reset(const size_t pos) { // set bit at pos to false
        return set(pos, false);
    }

    constexpr Bitset& flip() noexcept { // flip all bits
        for (size_t wordPos = 0; wordPos <= _WORDS; ++wordPos)
            _array[wordPos] = ~_array[wordPos];

        _trim();
        return *this;
    }

    constexpr Bitset& flip(const size_t pos) { // flip bit at pos
        if (Bits <= pos)
            throw std::out_of_range("Invalid Bitset position");

        return _flip(pos);
    }

    constexpr size_t size() const noexcept {
        return Bits;
    }

    constexpr size_t count() const noexcept { // count number of set bits
		size_t count = 0;

		for (size_t wordPos = 0; wordPos <= _WORDS; ++wordPos)
			count += custom::popcount(_array[wordPos]);

		return count;
    }

    constexpr bool all() const noexcept {	// check all bits are set to true
        constexpr bool zeroLength = Bits == 0;
        if constexpr (zeroLength) // must test for this, otherwise would count one full word
            return true;

        constexpr bool noPadding = Bits % _BITS_PER_WORD == 0;
        for (size_t wordPos = 0; wordPos < _WORDS + noPadding; ++wordPos)
            if (_array[wordPos] != ~Type{0})
                return false;

        return (noPadding || _array[_WORDS] == (Type{1} << (Bits % _BITS_PER_WORD)) - 1);
    }

	constexpr bool any() const noexcept {	// check if any bits are set to true
        for (size_t wordPos = 0; wordPos <= _WORDS; ++wordPos)
            if (_array[wordPos] != 0)
                return true;

        return false;
    }

    constexpr bool none() const noexcept {	// check if no bits are set to true
        return !any();
    }

    constexpr bool test(const size_t pos) const {	// returns the value of the bit at the position pos (counting from 0).
        if (Bits <= pos)
            throw std::out_of_range("Invalid Bitset position");

        return _subscript(pos);
    }

private:
	// Helpers

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

	constexpr Bitset& _set(const size_t pos, const bool val) noexcept {	// set bit at pos to val, no checking
        auto& word 		= _array[pos / _BITS_PER_WORD];
        const auto bit 	= Type{1} << pos % _BITS_PER_WORD;
        
		if (val)
            word |= bit;
        else
            word &= ~bit;

        return *this;
    }

	constexpr void _trim() noexcept { // clear any trailing bits in last word
        constexpr bool allowTrim = Bits == 0 || Bits % _BITS_PER_WORD != 0;

        if constexpr (allowTrim)
            _array[_WORDS] &= (Type{1} << Bits % _BITS_PER_WORD) - 1;
    }

};	// END Bitset


// Bitset binary operators
template<size_t Bits>
constexpr bool operator==(const Bitset<Bits>& left, const Bitset<Bits>& right) {
	return ::memcmp(&left._array[0], &right._array[0], sizeof(left._array));
}

template<size_t Bits>
constexpr bool operator!=(const Bitset<Bits>& left, const Bitset<Bits>& right) {
	return !(left == right);
}

template<size_t Bits>
constexpr Bitset<Bits> operator&(const Bitset<Bits>& left, const Bitset<Bits>& right) noexcept {
    Bitset<Bits> res = left;
    res &= right;
    return res;
}

template<size_t Bits>
constexpr Bitset<Bits> operator|(const Bitset<Bits>& left, const Bitset<Bits>& right) noexcept {
    Bitset<Bits> res = left;
    res |= right;
    return res;
}

template<size_t Bits>
constexpr Bitset<Bits> operator^(const Bitset<Bits>& left, const Bitset<Bits>& right) noexcept {
    Bitset<Bits> res = left;
    res ^= right;
    return res;
}


template<size_t Bits>
class Bitset<Bits>::Reference		// proxy for an element
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
