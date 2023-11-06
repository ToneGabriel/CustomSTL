#pragma once
#include "Bit.h"
#include "Limits.h"
#include "String.h"


CUSTOM_BEGIN

template<size_t Bits>
class Bitset
{
private:
	using Type = Conditional_t<Bits <= sizeof(unsigned long) * CHAR_BIT,
									unsigned long,
									unsigned long long>;

	static constexpr ptrdiff_t _BITS_PER_BLOCK 	= CHAR_BIT * sizeof(Type);
	static constexpr ptrdiff_t _BLOCKS 			= (Bits == 0) ? 0 : (Bits - 1) / _BITS_PER_BLOCK;

	// this is used in ull constructor only
	static constexpr bool _NEED_MASK 			= (Bits < CHAR_BIT * sizeof(unsigned long long));
    static constexpr unsigned long long _MASK 	= (1ULL << (_NEED_MASK ? Bits : 0)) - 1ULL;

	Type _array[_BLOCKS + 1];

public:
	class Reference;	// proxy for an element

    constexpr Bitset() noexcept : _array() { /*Empty*/ } // construct with all false values

    constexpr Bitset(unsigned long long val) noexcept
		: _array{static_cast<Type>(_NEED_MASK ? val & _MASK : val)} { /*Empty*/ }

public:
	// Operators

	constexpr bool operator[](size_t pos) const {
        return Bits <= pos ? (_validate(pos), false) : _get_bit(pos);
    }

    constexpr Reference operator[](const size_t pos) noexcept {
        _validate(pos);
        return Reference(*this, pos);
    }

    constexpr Bitset& operator&=(const Bitset& other) noexcept {
        for (size_t block = 0; block <= _BLOCKS; ++block)
            _array[block] &= other._array[block];

        return *this;
    }

    constexpr Bitset& operator|=(const Bitset& other) noexcept {
        for (size_t block = 0; block <= _BLOCKS; ++block)
            _array[block] |= other._array[block];

        return *this;
    }

    constexpr Bitset& operator^=(const Bitset& other) noexcept {
        for (size_t block = 0; block <= _BLOCKS; ++block)
            _array[block] ^= other._array[block];

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
	friend constexpr bool operator==(const Bitset<_Bits>& left, const Bitset<_Bits>& right);

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

        return _set_bit(pos, val);
    }

    constexpr Bitset& reset() noexcept { // set all bits false
        ::memset(&_array, 0, sizeof(_array));
        return *this;
    }

    constexpr Bitset& reset(const size_t pos) { // set bit at pos to false
        return set(pos, false);
    }

    constexpr Bitset& flip() noexcept { // flip all bits
        for (size_t block = 0; block <= _BLOCKS; ++block)
            _array[block] = ~_array[block];

        _trim();
        return *this;
    }

    constexpr Bitset& flip(const size_t pos) { // flip bit at pos
        if (Bits <= pos)
            throw std::out_of_range("Invalid Bitset position");

        return _flip_bit(pos);
    }

    constexpr size_t size() const noexcept {
        return Bits;
    }

    constexpr size_t count() const noexcept { // count number of set bits
		size_t count = 0;

		for (size_t block = 0; block <= _BLOCKS; ++block)
			count += custom::popcount(_array[block]);

		return count;
    }

    constexpr bool all() const noexcept {	// check all bits are set to true
        constexpr bool zeroLength = (Bits == 0);
        if constexpr (zeroLength) // must test for this, otherwise would count one full word
            return true;

        constexpr bool noPadding = (Bits % _BITS_PER_BLOCK == 0);
        for (size_t block = 0; block < _BLOCKS + noPadding; ++block)
            if (_array[block] != ~Type{0})
                return false;

        return (noPadding || _array[_BLOCKS] == (Type{1} << (Bits % _BITS_PER_BLOCK)) - 1);
    }

	constexpr bool any() const noexcept {	// check if any bits are set to true
        for (size_t block = 0; block <= _BLOCKS; ++block)
            if (_array[block] != 0)
                return true;

        return false;
    }

    constexpr bool none() const noexcept {	// check if no bits are set to true
        return !any();
    }

    constexpr bool test(const size_t pos) const {	// returns the value of the bit at the position pos (counting from 0).
        if (Bits <= pos)
            throw std::out_of_range("Invalid Bitset position");

        return _get_bit(pos);
    }

    template<class CharType = char, class Alloc = custom::Allocator<CharType>, class Traits = custom::CharTraits<CharType>>
    constexpr BasicString<CharType, Alloc, Traits> to_string(	const CharType placeholder0 = static_cast<CharType>('0'),
																const CharType placeholder1 = static_cast<CharType>('1')) const {	// convert bitset to string
        
		BasicString<CharType, Alloc, Traits> string(Bits);	// create string and reserve Bits capacity

        for (size_t pos = Bits; 0 < pos;)
            string.push_back(_get_bit(--pos) ? placeholder1 : placeholder0);

        return string;
    }

    constexpr unsigned long to_ulong() const noexcept(Bits <= 32) {
		// TODO: implement
		return 0;
	}

	constexpr unsigned long long to_ullong() const noexcept(Bits <= 64) {
		// TODO: implement
		return 0;
	}

private:
	// Helpers

    static constexpr void _validate(const size_t pos) noexcept {
        CUSTOM_ASSERT(pos < Bits, "Bitset index outside range");
    }

	constexpr void _trim() noexcept { // clear any trailing bits in last word
        constexpr bool allowTrim = Bits == 0 || Bits % _BITS_PER_BLOCK != 0;

        if constexpr (allowTrim)
            _array[_BLOCKS] &= (Type{1} << Bits % _BITS_PER_BLOCK) - 1;
    }

    constexpr bool _get_bit(size_t pos) const {
        return (_array[pos / _BITS_PER_BLOCK] & (Type{1} << pos % _BITS_PER_BLOCK)) != 0;
    }

	constexpr Bitset& _flip_bit(const size_t pos) noexcept { // flip bit at pos, no checking
        _array[pos / _BITS_PER_BLOCK] ^= Type{1} << pos % _BITS_PER_BLOCK;
        return *this;
    }

	constexpr Bitset& _set_bit(const size_t pos, const bool val) noexcept {	// set bit at pos to val, no checking
        auto& word 		= _array[pos / _BITS_PER_BLOCK];
        const auto bit 	= Type{1} << pos % _BITS_PER_BLOCK;
        
		if (val)
            word |= bit;
        else
            word &= ~bit;

        return *this;
    }
};	// END Bitset


// Bitset binary operators
template<size_t _Bits>
constexpr bool operator==(const Bitset<_Bits>& left, const Bitset<_Bits>& right) {
	return ::memcmp(&left._array[0], &right._array[0], sizeof(left._array));
}

template<size_t _Bits>
constexpr bool operator!=(const Bitset<_Bits>& left, const Bitset<_Bits>& right) {
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
std::ostream& operator<<(std::ostream& os, const Bitset<Bits>& bitset) {
	os << bitset.to_string();
	return os;
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
		_bitsetRef->_set_bit(_position, val);
		return *this;
	}

	constexpr Reference& operator=(const Reference& bitRef) noexcept {
		_bitsetRef->_set_bit(_position, static_cast<bool>(bitRef));
		return *this;
	}

	constexpr bool operator~() const noexcept {
		return !_bitsetRef->_get_bit(_position);
	}

	constexpr operator bool() const noexcept {
		return _bitsetRef->_get_bit(_position);
	}

	constexpr Reference& flip() noexcept {
		_bitsetRef->_flip_bit(_position);
		return *this;
	}
};	// END Reference

CUSTOM_END
