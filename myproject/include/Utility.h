#pragma once
#include "xUtility.h"


CUSTOM_BEGIN

// move
template<class Ty>
constexpr RemoveReference_t<Ty>&& move(Ty&& val) noexcept { 
    return static_cast<RemoveReference_t<Ty>&&>(val);
}

// forward
template<class Ty>
constexpr Ty&& forward(RemoveReference_t<Ty>& val) noexcept {
    return static_cast<Ty&&>(val);
}

template<class Ty>
constexpr Ty&& forward(RemoveReference_t<Ty>&& val) noexcept {
    static_assert(!IsLvalueReference_v<Ty>, "bad forward call");
    return static_cast<Ty&&>(val);
}

template<class Ty,
EnableIf_t<IsNothrowMoveConstructible_v<Ty> && IsNothrowMoveAssignable_v<Ty> , bool> = true>
constexpr void swap(Ty& first, Ty& second) noexcept {
    Ty temp = custom::move(first);
    first   = custom::move(second);
    second  = custom::move(temp);
}

template<class Ty, class Other = Ty,
EnableIf_t<IsNothrowMoveConstructible_v<Ty> && IsNothrowAssignable_v<Ty&, Other>, bool> = true>
constexpr Ty exchange(Ty& val, Other&& newVal) noexcept {
    // assign newVal to val, return previous val
    Ty old  = custom::move(val);
    val     = custom::move(newVal);
    return old;
}


CUSTOM_END