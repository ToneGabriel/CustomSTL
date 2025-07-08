#pragma once
#include "stl_cpp/c_bit.h"
#include "stl_cpp/c_limits.h"
#include "stl_cpp/c_string.h"


CUSTOM_BEGIN

template<size_t Bits>
class bitset
{
private:
	using type = conditional_t<Bits <= sizeof(unsigned long) * CHAR_BIT,
									unsigned long,
									unsigned long long>;

	static constexpr ptrdiff_t _BITS_PER_BLOCK 	= CHAR_BIT * sizeof(type);
	static constexpr ptrdiff_t _BLOCKS 			= (Bits == 0) ? 0 : (Bits - 1) / _BITS_PER_BLOCK;

	// this is used in ull constructor only
	static constexpr bool _NEED_MASK 			= (Bits < CHAR_BIT * sizeof(unsigned long long));
    static constexpr unsigned long long _MASK 	= (1ULL << (_NEED_MASK ? Bits : 0)) - 1ULL;

	type _array[_BLOCKS + 1];   // set of blocks

public:
	class reference;	// proxy for an element

    constexpr bitset() noexcept : _array() { /*Empty*/ } // construct with all false values

    constexpr bitset(unsigned long long val) noexcept
		: _array{static_cast<type>(_NEED_MASK ? val & _MASK : val)} { /*Empty*/ }

    // construct from [pos, pos + count) elements in string
    template<class CharType, class Alloc, class Traits>
    constexpr explicit bitset(  const basic_string<CharType, Alloc, Traits>& string, size_t pos = 0,
                                size_t count = basic_string<CharType, Alloc, Traits>::npos,
                                CharType placeholder0 = static_cast<CharType>('0'),
                                CharType placeholder1 = static_cast<CharType>('1'))
    {    
        if (string.size() < pos)
            throw std::out_of_range("Position out of range!");

        if (string.size() - pos < count)
            count = string.size() - pos; // trim count to size

        _construct_from_cstring<Traits>(string.data() + pos, count, placeholder0, placeholder1);
    }

    template<class CharType>
    constexpr explicit bitset(  const CharType* cstring,
                                size_t count = basic_string<CharType>::npos,
                                CharType placeholder0 = static_cast<CharType>('0'),
                                CharType placeholder1 = static_cast<CharType>('1'))
    {
        if (count == basic_string<CharType>::npos)
            count = char_traits<CharType>::length(cstring);

        _construct_from_cstring<char_traits<CharType>>(cstring, count, placeholder0, placeholder1);
    }

public:
	// Operators

	constexpr bool operator[](size_t pos) const
    {
        return Bits <= pos ? (_validate(pos), false) : _get_bit(pos);
    }

    constexpr reference operator[](const size_t pos) noexcept
    {
        _validate(pos);
        return reference(*this, pos);
    }

    constexpr bitset& operator&=(const bitset& other) noexcept
    {
        for (size_t block = 0; block <= _BLOCKS; ++block)
            _array[block] &= other._array[block];

        return *this;
    }

    constexpr bitset& operator|=(const bitset& other) noexcept
    {
        for (size_t block = 0; block <= _BLOCKS; ++block)
            _array[block] |= other._array[block];

        return *this;
    }

    constexpr bitset& operator^=(const bitset& other) noexcept
    {
        for (size_t block = 0; block <= _BLOCKS; ++block)
            _array[block] ^= other._array[block];

        return *this;
    }

    // shift left by pos, first by blocks then by bits
	constexpr bitset& operator<<=(size_t pos) noexcept
    {
        const ptrdiff_t blockShift = static_cast<ptrdiff_t>(pos / _BITS_PER_BLOCK);

        if (blockShift != 0)
            for (ptrdiff_t block = _BLOCKS; 0 <= block; --block)
                _array[block] = blockShift <= block ? _array[block - blockShift] : 0;

        if ((pos %= _BITS_PER_BLOCK) != 0)  // 0 < pos < _BITS_PER_BLOCK, shift by bits
        {
            for (ptrdiff_t block = _BLOCKS; 0 < block; --block)
                _array[block] = (_array[block] << pos) | (_array[block - 1] >> (_BITS_PER_BLOCK - pos));

            _array[0] <<= pos;
        }

        _trim();
        return *this;
	}

    // shift right by pos, first by blocks then by bits
	constexpr bitset& operator>>=(size_t pos) noexcept
    {
        const ptrdiff_t blockShift = static_cast<ptrdiff_t>(pos / _BITS_PER_BLOCK);

        if (blockShift != 0)
            for (ptrdiff_t block = 0; block <= _BLOCKS; ++block)
                _array[block] = blockShift <= _BLOCKS - block ? _array[block + blockShift] : 0;

        if ((pos %= _BITS_PER_BLOCK) != 0)  // 0 < pos < _BITS_PER_BLOCK, shift by bits
        {
            for (ptrdiff_t block = 0; block < _BLOCKS; ++block)
                _array[block] = (_array[block] >> pos) | (_array[block + 1] << (_BITS_PER_BLOCK - pos));

            _array[_BLOCKS] >>= pos;
        }

        return *this;
	}

    constexpr bitset operator<<(const size_t pos) const noexcept
    {
        bitset temp = *this;
        temp <<= pos;
        return temp;
    }

    constexpr bitset operator>>(const size_t pos) const noexcept
    {
        bitset temp = *this;
        temp >>= pos;
        return temp;
    }

    // flip all bits
    constexpr bitset operator~() const noexcept
    {
        bitset temp = *this;
        temp.flip();
        return temp;
    }

	template<size_t _Bits>
	friend constexpr bool operator==(const bitset<_Bits>& left, const bitset<_Bits>& right);

public:
	// Main functions

    // set all bits true
	constexpr bitset& set()
    {
		::memset(&_array, 0xFF, sizeof(_array));
		_trim();
		return *this;
	}

    // set bit at pos to val
    constexpr bitset& set(const size_t pos, bool val = true)
    {
        if (Bits <= pos)
            throw std::out_of_range("Invalid bitset position");

        return _set_bit(pos, val);
    }

    // set all bits false
    constexpr bitset& reset() noexcept
    {
        ::memset(&_array, 0, sizeof(_array));
        return *this;
    }

    // set bit at pos to false
    constexpr bitset& reset(const size_t pos)
    {
        return set(pos, false);
    }

    // flip all bits
    constexpr bitset& flip() noexcept
    {
        for (size_t block = 0; block <= _BLOCKS; ++block)
            _array[block] = ~_array[block];

        _trim();
        return *this;
    }

    // flip bit at pos
    constexpr bitset& flip(const size_t pos)
    {
        if (Bits <= pos)
            throw std::out_of_range("Invalid bitset position");

        return _flip_bit(pos);
    }

    constexpr size_t size() const noexcept
    {
        return Bits;
    }

    // count number of set bits
    constexpr size_t count() const noexcept
    {
		size_t count = 0;

		for (size_t block = 0; block <= _BLOCKS; ++block)
			count += custom::popcount(_array[block]);

		return count;
    }

    // check all bits are set to true
    constexpr bool all() const noexcept
    {
        constexpr bool zeroLength = (Bits == 0);
        if constexpr (zeroLength) // must test for this, otherwise would count one full block
            return true;

        constexpr bool noPadding = (Bits % _BITS_PER_BLOCK == 0);
        for (size_t block = 0; block < _BLOCKS + noPadding; ++block)
            if (_array[block] != ~type{0})
                return false;

        return (noPadding || _array[_BLOCKS] == (type{1} << (Bits % _BITS_PER_BLOCK)) - 1);
    }

    // check if any bits are set to true
	constexpr bool any() const noexcept
    {
        for (size_t block = 0; block <= _BLOCKS; ++block)
            if (_array[block] != 0)
                return true;

        return false;
    }

    // check if no bits are set to true
    constexpr bool none() const noexcept
    {
        return !any();
    }

    // returns the value of the bit at the position pos (counting from 0)
    constexpr bool test(const size_t pos) const
    {
        if (Bits <= pos)
            throw std::out_of_range("Invalid bitset position");

        return _get_bit(pos);
    }

    // convert bitset to string
    template<class CharType = char, class Alloc = custom::allocator<CharType>, class Traits = custom::char_traits<CharType>>
    constexpr basic_string<CharType, Alloc, Traits> to_string(	const CharType placeholder0 = static_cast<CharType>('0'),
																const CharType placeholder1 = static_cast<CharType>('1')) const
    {    
		basic_string<CharType, Alloc, Traits> string(Bits);	// create string and reserve Bits capacity

        for (size_t pos = Bits; 0 < pos; /*Empty*/)
            string.push_back(_get_bit(--pos) ? placeholder1 : placeholder0);

        return string;
    }

    constexpr unsigned long to_ulong() const noexcept(Bits <= 32)
    {
        constexpr bool bitsZero  = (Bits == 0);
        constexpr bool bitsSmall = (Bits <= 32);
        constexpr bool bitsLarge = (Bits > 64);

        if constexpr (bitsZero)
            return 0;
        else if constexpr (bitsSmall)
            return static_cast<unsigned long>(_array[0]);
        else
        {
            if constexpr (bitsLarge)
                for (size_t block = 1; block <= _BLOCKS; ++block)
                    if (_array[block] != 0)
                        throw std::overflow_error("Overflow!");   // fail if any high-order blocks are nonzero

            if (_array[0] > ULONG_MAX)
                throw std::overflow_error("Overflow!");

            return static_cast<unsigned long>(_array[0]);
        }
	}

	constexpr unsigned long long to_ullong() const noexcept(Bits <= 64)
    {
        constexpr bool bitsZero  = (Bits == 0);
        constexpr bool bitsLarge = (Bits > 64);

        if constexpr (bitsZero)
            return 0;
        else
        {
            if constexpr (bitsLarge)
                for (size_t block = 1; block <= _BLOCKS; ++block)
                    if (_array[block] != 0)
                        throw std::overflow_error("Overflow!");   // fail if any high-order blocks are nonzero

            return _array[0];
        }
	}

private:
	// Helpers

    static constexpr void _validate(const size_t pos) noexcept
    {
        CUSTOM_ASSERT(pos < Bits, "bitset index outside range");
    }

    // clear any trailing bits in last block
	constexpr void _trim() noexcept
    {
        constexpr bool allowTrim = Bits == 0 || Bits % _BITS_PER_BLOCK != 0;

        if constexpr (allowTrim)
            _array[_BLOCKS] &= (type{1} << Bits % _BITS_PER_BLOCK) - 1;
    }

    template<class Traits, class CharType>
    constexpr void _construct_from_cstring( const CharType* const cstring, size_t count,
                                            const CharType placeholder0, const CharType placeholder1)
    {
        if (count > Bits)
        {
            for (size_t pos = Bits; pos < count; ++pos)
                if (!Traits::eq(placeholder1, cstring[pos]) && !Traits::eq(placeholder0, cstring[pos]))
                    throw std::invalid_argument("Placeholders not recognized!");

            count = Bits;
        }

        size_t block = 0;
        if (count != 0)
        {
            size_t bitsInBlock  = 0;
            type thisBlock      = 0;
            auto last           = cstring + count;

            do
            {
                --last;
                thisBlock |= static_cast<type>(Traits::eq(placeholder1, *last)) << bitsInBlock;

                if (!Traits::eq(placeholder1, *last) && !Traits::eq(placeholder0, *last))
                    throw std::invalid_argument("Placeholders not recognized!");

                if (++bitsInBlock == _BITS_PER_BLOCK)
                {
                    _array[block++] = thisBlock;
                    thisBlock       = 0;
                    bitsInBlock     = 0;
                }

            } while (cstring != last);

            if (bitsInBlock != 0)
                _array[block++] = thisBlock;
        }

        for (/*Empty*/; block <= _BLOCKS; ++block)
            _array[block] = 0;
    }

    constexpr bool _get_bit(size_t pos) const
    {
        return (_array[pos / _BITS_PER_BLOCK] & (type{1} << pos % _BITS_PER_BLOCK)) != 0;
    }

    // flip bit at pos, no checking
	constexpr bitset& _flip_bit(const size_t pos) noexcept
    {
        _array[pos / _BITS_PER_BLOCK] ^= type{1} << pos % _BITS_PER_BLOCK;
        return *this;
    }

    // set bit at pos to val, no checking
	constexpr bitset& _set_bit(const size_t pos, const bool val) noexcept
    {
        type& block     = _array[pos / _BITS_PER_BLOCK];
        const type bit  = type{1} << (pos % _BITS_PER_BLOCK);
        
		if (val)
            block |= bit;
        else
            block &= ~bit;

        return *this;
    }
};	// END bitset


// bitset binary operators
template<size_t Bits>
constexpr bool operator==(const bitset<Bits>& left, const bitset<Bits>& right)
{
	return ::memcmp(&left._array[0], &right._array[0], sizeof(left._array));
}

template<size_t Bits>
constexpr bool operator!=(const bitset<Bits>& left, const bitset<Bits>& right)
{
	return !(left == right);
}

template<size_t Bits>
constexpr bitset<Bits> operator&(const bitset<Bits>& left, const bitset<Bits>& right) noexcept
{
    bitset<Bits> res = left;
    res &= right;
    return res;
}

template<size_t Bits>
constexpr bitset<Bits> operator|(const bitset<Bits>& left, const bitset<Bits>& right) noexcept
{
    bitset<Bits> res = left;
    res |= right;
    return res;
}

template<size_t Bits>
constexpr bitset<Bits> operator^(const bitset<Bits>& left, const bitset<Bits>& right) noexcept
{
    bitset<Bits> res = left;
    res ^= right;
    return res;
}

template<size_t Bits>
std::ostream& operator<<(std::ostream& os, const bitset<Bits>& bitset)
{
	os << bitset.to_string();
	return os;
}



template<size_t Bits>
class bitset<Bits>::reference		// proxy for an element
{
private:
	friend bitset;

	bitset<Bits>*   _bitsetRef;
	size_t          _position;

private:
	constexpr reference() noexcept
		: _bitsetRef(nullptr), _position(0) { /*Empty*/ }

	constexpr reference(bitset<Bits>& bitset, const size_t pos) noexcept
		: _bitsetRef(&bitset), _position(pos) { /*Empty*/ }

public:
	constexpr reference(const reference&) = default;

	constexpr ~reference() noexcept { /*Empty*/ }

	constexpr reference& operator=(const bool val) noexcept
    {
		_bitsetRef->_set_bit(_position, val);
		return *this;
	}

	constexpr reference& operator=(const reference& bitRef) noexcept
    {
		_bitsetRef->_set_bit(_position, static_cast<bool>(bitRef));
		return *this;
	}

	constexpr bool operator~() const noexcept
    {
		return !_bitsetRef->_get_bit(_position);
	}

	constexpr operator bool() const noexcept
    {
		return _bitsetRef->_get_bit(_position);
	}

	constexpr reference& flip() noexcept
    {
		_bitsetRef->_flip_bit(_position);
		return *this;
	}
};	// END bitset::reference

CUSTOM_END
