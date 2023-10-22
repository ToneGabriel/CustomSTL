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

};	// END Bitset

CUSTOM_END
