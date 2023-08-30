// algorithm standard header

// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once
#ifndef _ALGORITHM_
#define _ALGORITHM_
#include <yvals_core.h>
#if _STL_COMPILER_PREPROCESSOR
#include <xmemory>

#if _HAS_CXX23
#include <optional>
#endif // _HAS_CXX23

#pragma pack(push, _CRT_PACKING)
#pragma warning(push, _STL_WARNING_LEVEL)
#pragma warning(disable : _STL_DISABLED_WARNINGS)
_STL_DISABLE_CLANG_WARNINGS
#pragma push_macro("new")
#undef new

#if _USE_STD_VECTOR_ALGORITHMS

_EXTERN_C
struct _Min_max_element_t {
    const void* _Min;
    const void* _Max;
};

// The "noalias" attribute tells the compiler optimizer that pointers going into these hand-vectorized algorithms
// won't be stored beyond the lifetime of the function, and that the function will only reference arrays denoted by
// those pointers. The optimizer also assumes in that case that a pointer parameter is not returned to the caller via
// the return value, so functions using "noalias" must usually return void. This attribute is valuable because these
// functions are in native code objects that the compiler cannot analyze. In the absence of the noalias attribute, the
// compiler has to assume that the denoted arrays are "globally address taken", and that any later calls to
// unanalyzable routines may modify those arrays.
__declspec(noalias) void __cdecl __std_reverse_copy_trivially_copyable_1(
    const void* _First, const void* _Last, void* _Dest) noexcept;
__declspec(noalias) void __cdecl __std_reverse_copy_trivially_copyable_2(
    const void* _First, const void* _Last, void* _Dest) noexcept;
__declspec(noalias) void __cdecl __std_reverse_copy_trivially_copyable_4(
    const void* _First, const void* _Last, void* _Dest) noexcept;
__declspec(noalias) void __cdecl __std_reverse_copy_trivially_copyable_8(
    const void* _First, const void* _Last, void* _Dest) noexcept;

_Min_max_element_t __stdcall __std_minmax_element_1(const void* _First, const void* _Last, bool _Signed) noexcept;
_Min_max_element_t __stdcall __std_minmax_element_2(const void* _First, const void* _Last, bool _Signed) noexcept;
_Min_max_element_t __stdcall __std_minmax_element_4(const void* _First, const void* _Last, bool _Signed) noexcept;
_Min_max_element_t __stdcall __std_minmax_element_8(const void* _First, const void* _Last, bool _Signed) noexcept;
_END_EXTERN_C

template <class _Ty>
_STD pair<_Ty*, _Ty*> __std_minmax_element(_Ty* _First, _Ty* _Last) noexcept {
    constexpr bool _Signed = _STD is_signed_v<_Ty>;

    _Min_max_element_t _Res;

    if constexpr (sizeof(_Ty) == 1) {
        _Res = __std_minmax_element_1(_First, _Last, _Signed);
    }
    else if constexpr (sizeof(_Ty) == 2) {
        _Res = __std_minmax_element_2(_First, _Last, _Signed);
    }
    else if constexpr (sizeof(_Ty) == 4) {
        _Res = __std_minmax_element_4(_First, _Last, _Signed);
    }
    else if constexpr (sizeof(_Ty) == 8) {
        _Res = __std_minmax_element_8(_First, _Last, _Signed);
    }
    else {
        static_assert(_STD _Always_false<_Ty>, "Unexpected size");
    }

    return { const_cast<_Ty*>(static_cast<const _Ty*>(_Res._Min)), const_cast<_Ty*>(static_cast<const _Ty*>(_Res._Max)) };
}
#endif // _USE_STD_VECTOR_ALGORITHMS

_STD_BEGIN
_INLINE_VAR constexpr int _ISORT_MAX = 32; // maximum size for insertion sort

template <class _It>
_INLINE_VAR constexpr _Iter_diff_t<_It> _Isort_max{ _ISORT_MAX };

template <class _Diff>
constexpr ptrdiff_t _Temporary_buffer_size(const _Diff _Value) noexcept {
    // convert an iterator difference_type to a ptrdiff_t for use in temporary buffers
    using _CT = common_type_t<ptrdiff_t, _Diff>;
    return static_cast<ptrdiff_t>((_STD min)(static_cast<_CT>(PTRDIFF_MAX), static_cast<_CT>(_Value)));
}

template <class _Ty>
struct _Optimistic_temporary_buffer { // temporary storage with _alloca-like attempt
    static constexpr size_t _Optimistic_size = 4096; // default to ~1 page
    static constexpr size_t _Optimistic_count = (_STD max)(static_cast<size_t>(1), _Optimistic_size / sizeof(_Ty));

    template <class _Diff>
    explicit _Optimistic_temporary_buffer(const _Diff _Requested_size) noexcept { // get temporary storage
        const auto _Attempt = _Temporary_buffer_size(_Requested_size);
        // Since _Diff is a count of elements in a forward range, and forward iterators must denote objects in memory,
        // it must fit in a size_t.
        if (static_cast<size_t>(_Requested_size) <= _Optimistic_count) { // unconditionally engage stack space
            _Data = reinterpret_cast<_Ty*>(&_Stack_space[0]);
            _Capacity = static_cast<ptrdiff_t>(_Requested_size); // in bounds due to if condition
            return;
        }

        const pair<_Ty*, ptrdiff_t> _Raw = _Get_temporary_buffer<_Ty>(_Attempt);
        if (static_cast<size_t>(_Raw.second) > _Optimistic_count) { // engage heap space
            _Data = _Raw.first;
            _Capacity = _Raw.second;
            return;
        }

        // less heap space than stack space, give up and use stack instead
        _Return_temporary_buffer(_Raw.first);
        _Data = reinterpret_cast<_Ty*>(&_Stack_space[0]);
        _Capacity = _Optimistic_count;
    }

    _Optimistic_temporary_buffer(const _Optimistic_temporary_buffer&) = delete;
    _Optimistic_temporary_buffer& operator=(const _Optimistic_temporary_buffer&) = delete;

    ~_Optimistic_temporary_buffer() noexcept {
        if (static_cast<size_t>(_Capacity) > _Optimistic_count) {
            _Return_temporary_buffer(_Data);
        }
    }

    _Ty* _Data; // points to heap memory iff _Capacity > _Optimistic_count
    ptrdiff_t _Capacity;
    _Aligned_storage_t<sizeof(_Ty), alignof(_Ty)> _Stack_space[_Optimistic_count];
};

#ifdef __cpp_lib_concepts
namespace ranges {
    _EXPORT_STD template <class _In, class _Fun>
        struct in_fun_result {
        /* [[no_unique_address]] */ _In in;
        /* [[no_unique_address]] */ _Fun fun;

        template <_Convertible_from<const _In&> _IIn, _Convertible_from<const _Fun&> _FFun>
        constexpr operator in_fun_result<_IIn, _FFun>() const& {
            return { in, fun };
        }

        template <_Convertible_from<_In> _IIn, _Convertible_from<_Fun> _FFun>
        constexpr operator in_fun_result<_IIn, _FFun>()&& {
            return { _STD move(in), _STD move(fun) };
        }
    };

    _EXPORT_STD template <class _In1, class _In2, class _Out>
        struct in_in_out_result {
        /* [[no_unique_address]] */ _In1 in1;
        /* [[no_unique_address]] */ _In2 in2;
        /* [[no_unique_address]] */ _Out out;

        template <_Convertible_from<const _In1&> _IIn1, _Convertible_from<const _In2&> _IIn2,
            _Convertible_from<const _Out&> _OOut>
        constexpr operator in_in_out_result<_IIn1, _IIn2, _OOut>() const& {
            return { in1, in2, out };
        }

        template <_Convertible_from<_In1> _IIn1, _Convertible_from<_In2> _IIn2, _Convertible_from<_Out> _OOut>
        constexpr operator in_in_out_result<_IIn1, _IIn2, _OOut>()&& {
            return { _STD move(in1), _STD move(in2), _STD move(out) };
        }
    };

    _EXPORT_STD template <class _In, class _Out1, class _Out2>
        struct in_out_out_result {
        /* [[no_unique_address]] */ _In in;
        /* [[no_unique_address]] */ _Out1 out1;
        /* [[no_unique_address]] */ _Out2 out2;

        template <_Convertible_from<const _In&> _IIn, _Convertible_from<const _Out1&> _OOut1,
            _Convertible_from<const _Out2&> _OOut2>
        constexpr operator in_out_out_result<_IIn, _OOut1, _OOut2>() const& {
            return { in, out1, out2 };
        }

        template <_Convertible_from<_In> _IIn, _Convertible_from<_Out1> _OOut1, _Convertible_from<_Out2> _OOut2>
        constexpr operator in_out_out_result<_IIn, _OOut1, _OOut2>()&& {
            return { _STD move(in), _STD move(out1), _STD move(out2) };
        }
    };

    _EXPORT_STD template <class _Ty>
        struct min_max_result {
        /* [[no_unique_address]] */ _Ty min;
        /* [[no_unique_address]] */ _Ty max;

        template <_Convertible_from<const _Ty&> _Ty2>
        constexpr operator min_max_result<_Ty2>() const& {
            return { min, max };
        }

        template <_Convertible_from<_Ty> _Ty2>
        constexpr operator min_max_result<_Ty2>()&& {
            return { _STD move(min), _STD move(max) };
        }
    };

    _EXPORT_STD template <class _In>
        struct in_found_result {
        /* [[no_unique_address]] */ _In in;
        bool found;

        template <_Convertible_from<const _In&> _IIn>
        constexpr operator in_found_result<_IIn>() const& {
            return { in, found };
        }

        template <_Convertible_from<_In> _IIn>
        constexpr operator in_found_result<_IIn>()&& {
            return { _STD move(in), found };
        }
    };

#if _HAS_CXX23
    _EXPORT_STD template <class _In, class _Ty>
        struct in_value_result {
        /* [[no_unique_address]] */ _In in;
        /* [[no_unique_address]] */ _Ty value;

        template <class _IIn, class _TTy>
            requires convertible_to<const _In&, _IIn>&& convertible_to<const _Ty&, _TTy>
        constexpr operator in_value_result<_IIn, _TTy>() const& {
            return { in, value };
        }

        template <class _IIn, class _TTy>
            requires convertible_to<_In, _IIn>&& convertible_to<_Ty, _TTy>
        constexpr operator in_value_result<_IIn, _TTy>()&& {
            return { _STD move(in), _STD move(value) };
        }
    };
#endif // _HAS_CXX23
} // namespace ranges
#endif // __cpp_lib_concepts

_EXPORT_STD template <class _InIt, class _Fn>
_CONSTEXPR20 _Fn for_each(_InIt _First, _InIt _Last, _Fn _Func) { // perform function for each element [_First, _Last)
    _Adl_verify_range(_First, _Last);
    auto _UFirst = _Get_unwrapped(_First);
    const auto _ULast = _Get_unwrapped(_Last);
    for (; _UFirst != _ULast; ++_UFirst) {
        _Func(*_UFirst);
    }

    return _Func;
}

#if _HAS_CXX17
_EXPORT_STD template <class _ExPo, class _FwdIt, class _Fn, _Enable_if_execution_policy_t<_ExPo> = 0>
void for_each(_ExPo&& _Exec, _FwdIt _First, _FwdIt _Last, _Fn _Func) noexcept; // terminates

_EXPORT_STD template <class _InIt, class _Diff, class _Fn>
_CONSTEXPR20 _InIt for_each_n(_InIt _First, const _Diff _Count_raw, _Fn _Func) {
    // perform function for each element [_First, _First + _Count)
    _Algorithm_int_t<_Diff> _Count = _Count_raw;
    if (0 < _Count) {
        auto _UFirst = _Get_unwrapped_n(_First, _Count);
        do {
            _Func(*_UFirst);
            --_Count;
            ++_UFirst;
        } while (0 < _Count);

        _Seek_wrapped(_First, _UFirst);
    }

    return _First;
}

_EXPORT_STD template <class _ExPo, class _FwdIt, class _Diff, class _Fn, _Enable_if_execution_policy_t<_ExPo> = 0>
_FwdIt for_each_n(_ExPo&& _Exec, _FwdIt _First, _Diff _Count_raw, _Fn _Func) noexcept; // terminates
#endif // _HAS_CXX17

#ifdef __cpp_lib_concepts
namespace ranges {
    _EXPORT_STD template <class _In, class _Fun>
        using for_each_result = in_fun_result<_In, _Fun>;

    class _For_each_fn : private _Not_quite_object {
    public:
        using _Not_quite_object::_Not_quite_object;

        template <input_iterator _It, sentinel_for<_It> _Se, class _Pj = identity,
            indirectly_unary_invocable<projected<_It, _Pj>> _Fn>
        constexpr for_each_result<_It, _Fn> operator()(_It _First, _Se _Last, _Fn _Func, _Pj _Proj = {}) const {
            _Adl_verify_range(_First, _Last);

            auto _UResult = _For_each_unchecked(_Unwrap_iter<_Se>(_STD move(_First)),
                _Unwrap_sent<_It>(_STD move(_Last)), _STD move(_Func), _Pass_fn(_Proj));

            _Seek_wrapped(_First, _STD move(_UResult.in));
            return { _STD move(_First), _STD move(_UResult.fun) };
        }

        template <input_range _Rng, class _Pj = identity,
            indirectly_unary_invocable<projected<iterator_t<_Rng>, _Pj>> _Fn>
        constexpr for_each_result<borrowed_iterator_t<_Rng>, _Fn> operator()(
            _Rng&& _Range, _Fn _Func, _Pj _Proj = {}) const {
            auto _First = _RANGES begin(_Range);

            auto _UResult = _For_each_unchecked(
                _Unwrap_range_iter<_Rng>(_STD move(_First)), _Uend(_Range), _STD move(_Func), _Pass_fn(_Proj));

            _Seek_wrapped(_First, _STD move(_UResult.in));
            return { _STD move(_First), _STD move(_UResult.fun) };
        }

    private:
        template <class _It, class _Se, class _Pj, class _Fn>
        _NODISCARD static constexpr for_each_result<_It, _Fn> _For_each_unchecked(
            _It _First, const _Se _Last, _Fn _Func, _Pj _Proj) {
            _STL_INTERNAL_STATIC_ASSERT(input_iterator<_It>);
            _STL_INTERNAL_STATIC_ASSERT(sentinel_for<_Se, _It>);
            _STL_INTERNAL_STATIC_ASSERT(indirectly_unary_invocable<_Fn, projected<_It, _Pj>>);

            for (; _First != _Last; ++_First) {
                _STD invoke(_Func, _STD invoke(_Proj, *_First));
            }

            return { _STD move(_First), _STD move(_Func) };
        }
    };

    _EXPORT_STD inline constexpr _For_each_fn for_each{ _Not_quite_object::_Construct_tag{} };

    _EXPORT_STD template <class _In, class _Fun>
        using for_each_n_result = in_fun_result<_In, _Fun>;

    class _For_each_n_fn : private _Not_quite_object {
    public:
        using _Not_quite_object::_Not_quite_object;

        template <input_iterator _It, class _Pj = identity, indirectly_unary_invocable<projected<_It, _Pj>> _Fn>
        constexpr for_each_n_result<_It, _Fn> operator()(
            _It _First, iter_difference_t<_It> _Count, _Fn _Func, _Pj _Proj = {}) const {
            if (0 < _Count) {
                auto _UFirst = _Get_unwrapped_n(_STD move(_First), _Count);
                do {
                    _STD invoke(_Func, _STD invoke(_Proj, *_UFirst));
                    --_Count;
                    ++_UFirst;
                } while (0 < _Count);

                _Seek_wrapped(_First, _STD move(_UFirst));
            }

            return { _STD move(_First), _STD move(_Func) };
        }
    };

    _EXPORT_STD inline constexpr _For_each_n_fn for_each_n{ _Not_quite_object::_Construct_tag{} };
} // namespace ranges
#endif // __cpp_lib_concepts

#if _HAS_CXX17
_EXPORT_STD template <class _ExPo, class _FwdIt, class _Pr, _Enable_if_execution_policy_t<_ExPo> = 0>
_NODISCARD _FwdIt find_if(_ExPo&& _Exec, _FwdIt _First, _FwdIt _Last, _Pr _Pred) noexcept; // terminates
#endif // _HAS_CXX17

_EXPORT_STD template <class _InIt, class _Pr>
_NODISCARD _CONSTEXPR20 _InIt find_if_not(_InIt _First, const _InIt _Last, _Pr _Pred) {
    // find first element that satisfies !_Pred
    _Adl_verify_range(_First, _Last);
    auto _UFirst = _Get_unwrapped(_First);
    const auto _ULast = _Get_unwrapped(_Last);
    for (; _UFirst != _ULast; ++_UFirst) {
        if (!_Pred(*_UFirst)) {
            break;
        }
    }

    _Seek_wrapped(_First, _UFirst);
    return _First;
}

#if _HAS_CXX17
_EXPORT_STD template <class _ExPo, class _FwdIt, class _Pr, _Enable_if_execution_policy_t<_ExPo> = 0>
_NODISCARD _FwdIt find_if_not(_ExPo&& _Exec, _FwdIt _First, _FwdIt _Last, _Pr _Pred) noexcept; // terminates
#endif // _HAS_CXX17

_EXPORT_STD template <class _FwdIt, class _Pr>
_NODISCARD _CONSTEXPR20 _FwdIt adjacent_find(const _FwdIt _First, _FwdIt _Last, _Pr _Pred) {
    // find first satisfying _Pred with successor
    _Adl_verify_range(_First, _Last);
    auto _UFirst = _Get_unwrapped(_First);
    auto _ULast = _Get_unwrapped(_Last);
    if (_UFirst != _ULast) {
        for (auto _UNext = _UFirst; ++_UNext != _ULast; _UFirst = _UNext) {
            if (_Pred(*_UFirst, *_UNext)) {
                _ULast = _UFirst;
                break;
            }
        }
    }

    _Seek_wrapped(_Last, _ULast);
    return _Last;
}

_EXPORT_STD template <class _FwdIt>
_NODISCARD _CONSTEXPR20 _FwdIt adjacent_find(const _FwdIt _First, const _FwdIt _Last) { // find first matching successor
    return _STD adjacent_find(_First, _Last, equal_to<>{});
}

#if _HAS_CXX17
_EXPORT_STD template <class _ExPo, class _FwdIt, class _Pr, _Enable_if_execution_policy_t<_ExPo> = 0>
_NODISCARD _FwdIt adjacent_find(_ExPo&& _Exec, _FwdIt _First, _FwdIt _Last, _Pr _Pred) noexcept; // terminates

_EXPORT_STD template <class _ExPo, class _FwdIt, _Enable_if_execution_policy_t<_ExPo> = 0>
_NODISCARD _FwdIt adjacent_find(_ExPo&& _Exec, const _FwdIt _First, const _FwdIt _Last) noexcept /* terminates */ {
    // find first matching successor
    return _STD adjacent_find(_STD forward<_ExPo>(_Exec), _First, _Last, equal_to{});
}
#endif // _HAS_CXX17

#ifdef __cpp_lib_concepts
namespace ranges {
    class _Count_fn : private _Not_quite_object {
    public:
        using _Not_quite_object::_Not_quite_object;

        template <input_iterator _It, sentinel_for<_It> _Se, class _Ty, class _Pj = identity>
            requires indirect_binary_predicate<ranges::equal_to, projected<_It, _Pj>, const _Ty*>
        _NODISCARD constexpr iter_difference_t<_It> operator()(
            _It _First, _Se _Last, const _Ty& _Val, _Pj _Proj = {}) const {
            _Adl_verify_range(_First, _Last);
            return _Count_unchecked(
                _Unwrap_iter<_Se>(_STD move(_First)), _Unwrap_sent<_It>(_STD move(_Last)), _Val, _Pass_fn(_Proj));
        }

        template <input_range _Rng, class _Ty, class _Pj = identity>
            requires indirect_binary_predicate<ranges::equal_to, projected<iterator_t<_Rng>, _Pj>, const _Ty*>
        _NODISCARD constexpr range_difference_t<_Rng> operator()(_Rng&& _Range, const _Ty& _Val, _Pj _Proj = {}) const {
            return _Count_unchecked(_Ubegin(_Range), _Uend(_Range), _Val, _Pass_fn(_Proj));
        }

    private:
        template <class _It, class _Se, class _Ty, class _Pj>
        _NODISCARD static constexpr iter_difference_t<_It> _Count_unchecked(
            _It _First, const _Se _Last, const _Ty& _Val, _Pj _Proj) {
            _STL_INTERNAL_STATIC_ASSERT(input_iterator<_It>);
            _STL_INTERNAL_STATIC_ASSERT(sentinel_for<_Se, _It>);
            _STL_INTERNAL_STATIC_ASSERT(indirect_binary_predicate<ranges::equal_to, projected<_It, _Pj>, const _Ty*>);

#if _USE_STD_VECTOR_ALGORITHMS
            if constexpr (is_same_v<_Pj, identity> && _Vector_alg_in_find_is_safe<_It, _Ty>
                && sized_sentinel_for<_Se, _It>) {
                if (!_STD is_constant_evaluated()) {
                    if (!_STD _Could_compare_equal_to_value_type<_It>(_Val)) {
                        return 0;
                    }

                    const auto _First_ptr = _To_address(_First);
                    const auto _Last_ptr = _First_ptr + (_Last - _First);

                    return static_cast<iter_difference_t<_It>>(__std_count_trivial(_First_ptr, _Last_ptr, _Val));
                }
            }
#endif // _USE_STD_VECTOR_ALGORITHMS

            iter_difference_t<_It> _Count = 0;
            for (; _First != _Last; ++_First) {
                if (_STD invoke(_Proj, *_First) == _Val) {
                    ++_Count;
                }
            }

            return _Count;
        }
    };

    _EXPORT_STD inline constexpr _Count_fn count{ _Not_quite_object::_Construct_tag{} };
} // namespace ranges
#endif // __cpp_lib_concepts

_EXPORT_STD template <class _InIt, class _Pr>
_NODISCARD _CONSTEXPR20 _Iter_diff_t<_InIt> count_if(_InIt _First, _InIt _Last, _Pr _Pred) {
    // count elements satisfying _Pred
    _Adl_verify_range(_First, _Last);
    auto _UFirst = _Get_unwrapped(_First);
    const auto _ULast = _Get_unwrapped(_Last);
    _Iter_diff_t<_InIt> _Count = 0;
    for (; _UFirst != _ULast; ++_UFirst) {
        if (_Pred(*_UFirst)) {
            ++_Count;
        }
    }

    return _Count;
}

#if _HAS_CXX17
_EXPORT_STD template <class _ExPo, class _FwdIt, class _Pr, _Enable_if_execution_policy_t<_ExPo> = 0>
_NODISCARD _Iter_diff_t<_FwdIt> count_if(_ExPo&& _Exec, _FwdIt _First, _FwdIt _Last, _Pr _Pred) noexcept; // terminates
#endif // _HAS_CXX17

#ifdef __cpp_lib_concepts
namespace ranges {
    class _Count_if_fn : private _Not_quite_object {
    public:
        using _Not_quite_object::_Not_quite_object;

        template <input_iterator _It, sentinel_for<_It> _Se, class _Pj = identity,
            indirect_unary_predicate<projected<_It, _Pj>> _Pr>
        _NODISCARD constexpr iter_difference_t<_It> operator()(_It _First, _Se _Last, _Pr _Pred, _Pj _Proj = {}) const {
            _Adl_verify_range(_First, _Last);
            return _Count_if_unchecked(_Unwrap_iter<_Se>(_STD move(_First)), _Unwrap_sent<_It>(_STD move(_Last)),
                _Pass_fn(_Pred), _Pass_fn(_Proj));
        }

        template <input_range _Rng, class _Pj = identity,
            indirect_unary_predicate<projected<iterator_t<_Rng>, _Pj>> _Pr>
        _NODISCARD constexpr range_difference_t<_Rng> operator()(_Rng&& _Range, _Pr _Pred, _Pj _Proj = {}) const {
            return _Count_if_unchecked(_Ubegin(_Range), _Uend(_Range), _Pass_fn(_Pred), _Pass_fn(_Proj));
        }

    private:
        template <class _It, class _Se, class _Pj, class _Pr>
        _NODISCARD static constexpr iter_difference_t<_It> _Count_if_unchecked(
            _It _First, const _Se _Last, _Pr _Pred, _Pj _Proj) {
            _STL_INTERNAL_STATIC_ASSERT(input_iterator<_It>);
            _STL_INTERNAL_STATIC_ASSERT(sentinel_for<_Se, _It>);
            _STL_INTERNAL_STATIC_ASSERT(indirect_unary_predicate<_Pr, projected<_It, _Pj>>);

            iter_difference_t<_It> _Count = 0;
            for (; _First != _Last; ++_First) {
                if (_STD invoke(_Pred, _STD invoke(_Proj, *_First))) {
                    ++_Count;
                }
            }

            return _Count;
        }
    };

    _EXPORT_STD inline constexpr _Count_if_fn count_if{ _Not_quite_object::_Construct_tag{} };
} // namespace ranges
#endif // __cpp_lib_concepts

_EXPORT_STD template <class _InIt1, class _InIt2, class _Pr>
_NODISCARD _CONSTEXPR20 pair<_InIt1, _InIt2> mismatch(_InIt1 _First1, const _InIt1 _Last1, _InIt2 _First2, _Pr _Pred) {
    // return [_First1, _Last1)/[_First2, ...) mismatch
    _Adl_verify_range(_First1, _Last1);
    auto _UFirst1 = _Get_unwrapped(_First1);
    const auto _ULast1 = _Get_unwrapped(_Last1);
    auto _UFirst2 = _Get_unwrapped_n(_First2, _Idl_distance<_InIt1>(_UFirst1, _ULast1));
    while (_UFirst1 != _ULast1 && _Pred(*_UFirst1, *_UFirst2)) {
        ++_UFirst1;
        ++_UFirst2;
    }

    _Seek_wrapped(_First2, _UFirst2);
    _Seek_wrapped(_First1, _UFirst1);
    return { _First1, _First2 };
}

#if _HAS_CXX17
_EXPORT_STD template <class _ExPo, class _FwdIt1, class _FwdIt2, class _Pr, _Enable_if_execution_policy_t<_ExPo> = 0>
_NODISCARD pair<_FwdIt1, _FwdIt2> mismatch(
    _ExPo&& _Exec, _FwdIt1 _First1, _FwdIt1 _Last1, _FwdIt2 _First2, _Pr _Pred) noexcept; // terminates
#endif // _HAS_CXX17

_EXPORT_STD template <class _InIt1, class _InIt2>
_NODISCARD _CONSTEXPR20 pair<_InIt1, _InIt2> mismatch(const _InIt1 _First1, const _InIt1 _Last1, const _InIt2 _First2) {
    // return [_First1, _Last1)/[_First2, ...) mismatch
    return _STD mismatch(_First1, _Last1, _First2, equal_to<>{});
}

#if _HAS_CXX17
_EXPORT_STD template <class _ExPo, class _FwdIt1, class _FwdIt2, _Enable_if_execution_policy_t<_ExPo> = 0>
_NODISCARD pair<_FwdIt1, _FwdIt2> mismatch(
    _ExPo&& _Exec, const _FwdIt1 _First1, const _FwdIt1 _Last1, const _FwdIt2 _First2) noexcept /* terminates */ {
    // return [_First1, _Last1)/[_First2, ...) mismatch
    _REQUIRE_PARALLEL_ITERATOR(_FwdIt1);
    _REQUIRE_PARALLEL_ITERATOR(_FwdIt2);
    return _STD mismatch(_STD forward<_ExPo>(_Exec), _First1, _Last1, _First2, equal_to{});
}
#endif // _HAS_CXX17

_EXPORT_STD template <class _InIt1, class _InIt2, class _Pr>
_NODISCARD _CONSTEXPR20 pair<_InIt1, _InIt2> mismatch(
    _InIt1 _First1, _InIt1 _Last1, _InIt2 _First2, _InIt2 _Last2, _Pr _Pred) {
    // return [_First1, _Last1)/[_First2, _Last2) mismatch
    _Adl_verify_range(_First1, _Last1);
    _Adl_verify_range(_First2, _Last2);
    auto _UFirst1 = _Get_unwrapped(_First1);
    auto _ULast1 = _Get_unwrapped(_Last1);
    auto _UFirst2 = _Get_unwrapped(_First2);
    const auto _ULast2 = _Get_unwrapped(_Last2);
    if constexpr (_Is_ranges_random_iter_v<_InIt1> && _Is_ranges_random_iter_v<_InIt2>) {
        using _CT = _Common_diff_t<_InIt1, _InIt2>;
        const _CT _Count1 = _ULast1 - _UFirst1;
        const _CT _Count2 = _ULast2 - _UFirst2;
        const auto _Count = static_cast<_Iter_diff_t<_InIt1>>((_STD min)(_Count1, _Count2));
        _ULast1 = _UFirst1 + _Count;
        while (_UFirst1 != _ULast1 && _Pred(*_UFirst1, *_UFirst2)) {
            ++_UFirst1;
            ++_UFirst2;
        }
    }
    else {
        while (_UFirst1 != _ULast1 && _UFirst2 != _ULast2 && _Pred(*_UFirst1, *_UFirst2)) {
            ++_UFirst1;
            ++_UFirst2;
        }
    }

    _Seek_wrapped(_First2, _UFirst2);
    _Seek_wrapped(_First1, _UFirst1);
    return { _First1, _First2 };
}

#if _HAS_CXX17
_EXPORT_STD template <class _ExPo, class _FwdIt1, class _FwdIt2, class _Pr, _Enable_if_execution_policy_t<_ExPo> = 0>
_NODISCARD pair<_FwdIt1, _FwdIt2> mismatch(
    _ExPo&& _Exec, _FwdIt1 _First1, _FwdIt1 _Last1, _FwdIt2 _First2, _FwdIt2 _Last2, _Pr _Pred) noexcept; // terminates
#endif // _HAS_CXX17

_EXPORT_STD template <class _InIt1, class _InIt2>
_NODISCARD _CONSTEXPR20 pair<_InIt1, _InIt2> mismatch(_InIt1 _First1, _InIt1 _Last1, _InIt2 _First2, _InIt2 _Last2) {
    // return [_First1, _Last1)/[_First2, _Last2) mismatch
    return _STD mismatch(_First1, _Last1, _First2, _Last2, equal_to<>{});
}

#if _HAS_CXX17
_EXPORT_STD template <class _ExPo, class _FwdIt1, class _FwdIt2, _Enable_if_execution_policy_t<_ExPo> = 0>
_NODISCARD pair<_FwdIt1, _FwdIt2> mismatch(
    _ExPo&& _Exec, _FwdIt1 _First1, _FwdIt1 _Last1, _FwdIt2 _First2, _FwdIt2 _Last2) noexcept /* terminates */ {
    // return [_First1, _Last1)/[_First2, _Last2) mismatch
    _REQUIRE_PARALLEL_ITERATOR(_FwdIt1);
    _REQUIRE_PARALLEL_ITERATOR(_FwdIt2);
    return _STD mismatch(_STD forward<_ExPo>(_Exec), _First1, _Last1, _First2, _Last2, equal_to{});
}
#endif // _HAS_CXX17

#ifdef __cpp_lib_concepts
namespace ranges {
    template <input_iterator _It1, input_iterator _It2, _Integer_like _Size, class _Pr, class _Pj1, class _Pj2>
        requires indirectly_comparable<_It1, _It2, _Pr, _Pj1, _Pj2>
    _NODISCARD constexpr bool _Equal_count(
        _It1 _First1, _It2 _First2, _Size _Count, _Pr _Pred, _Pj1 _Proj1, _Pj2 _Proj2) {
        _STL_INTERNAL_CHECK(_Count >= 0);
        if constexpr (_Equal_memcmp_is_safe<_It1, _It2, _Pr> && same_as<_Pj1, identity> && same_as<_Pj2, identity>) {
            if (!_STD is_constant_evaluated()) {
                return _Memcmp_count(_First1, _First2, static_cast<size_t>(_Count)) == 0;
            }
        }

        for (; _Count != 0; ++_First1, (void) ++_First2, --_Count) {
            if (!_STD invoke(_Pred, _STD invoke(_Proj1, *_First1), _STD invoke(_Proj2, *_First2))) {
                return false;
            }
        }

        return true;
    }

    class _Equal_fn : private _Not_quite_object {
    private:
        template <class _It1, class _Se1, class _It2, class _Se2, class _Pr, class _Pj1, class _Pj2>
        _NODISCARD static constexpr bool _Equal_4(
            _It1 _First1, _Se1 _Last1, _It2 _First2, _Se2 _Last2, _Pr _Pred, _Pj1 _Proj1, _Pj2 _Proj2) {
            _STL_INTERNAL_STATIC_ASSERT(sentinel_for<_Se1, _It1>);
            _STL_INTERNAL_STATIC_ASSERT(sentinel_for<_Se2, _It2>);
            _STL_INTERNAL_STATIC_ASSERT(indirectly_comparable<_It1, _It2, _Pr, _Pj1, _Pj2>);

            for (;;) {
                if (_First1 == _Last1) {
                    return _First2 == _Last2;
                }
                else if (_First2 == _Last2) {
                    return false;
                }

                if (!_STD invoke(_Pred, _STD invoke(_Proj1, *_First1), _STD invoke(_Proj2, *_First2))) {
                    return false;
                }

                ++_First1;
                ++_First2;
            }
        }

    public:
        using _Not_quite_object::_Not_quite_object;

        template <input_iterator _It1, sentinel_for<_It1> _Se1, input_iterator _It2, sentinel_for<_It2> _Se2,
            class _Pr = ranges::equal_to, class _Pj1 = identity, class _Pj2 = identity>
            requires indirectly_comparable<_It1, _It2, _Pr, _Pj1, _Pj2>
        _NODISCARD constexpr bool operator()(_It1 _First1, _Se1 _Last1, _It2 _First2, _Se2 _Last2, _Pr _Pred = {},
            _Pj1 _Proj1 = {}, _Pj2 _Proj2 = {}) const {
            _Adl_verify_range(_First1, _Last1);
            _Adl_verify_range(_First2, _Last2);
            auto _UFirst1 = _Unwrap_iter<_Se1>(_STD move(_First1));
            auto _ULast1 = _Unwrap_sent<_It1>(_STD move(_Last1));
            auto _UFirst2 = _Unwrap_iter<_Se2>(_STD move(_First2));
            auto _ULast2 = _Unwrap_sent<_It2>(_STD move(_Last2));

            if constexpr (sized_sentinel_for<_Se1, _It1> && sized_sentinel_for<_Se2, _It2>) {
                const auto _Count = _ULast1 - _UFirst1;
                if (_Count != _ULast2 - _UFirst2) {
                    return false;
                }

                return _RANGES _Equal_count(_STD move(_UFirst1), _STD move(_UFirst2), _Count, _Pass_fn(_Pred),
                    _Pass_fn(_Proj1), _Pass_fn(_Proj2));
            }
            else {
                return _Equal_4(_STD move(_UFirst1), _STD move(_ULast1), _STD move(_UFirst2), _STD move(_ULast2),
                    _Pass_fn(_Pred), _Pass_fn(_Proj1), _Pass_fn(_Proj2));
            }
        }

        template <input_range _Rng1, input_range _Rng2, class _Pr = ranges::equal_to, class _Pj1 = identity,
            class _Pj2 = identity>
            requires indirectly_comparable<iterator_t<_Rng1>, iterator_t<_Rng2>, _Pr, _Pj1, _Pj2>
        _NODISCARD constexpr bool operator()(
            _Rng1&& _Range1, _Rng2&& _Range2, _Pr _Pred = {}, _Pj1 _Proj1 = {}, _Pj2 _Proj2 = {}) const {
            if constexpr (sized_range<_Rng1> && sized_range<_Rng2>) {
                using _Size1 = _Make_unsigned_like_t<range_size_t<_Rng1>>;
                const auto _Count = static_cast<_Size1>(_RANGES size(_Range1));
                using _Size2 = _Make_unsigned_like_t<range_size_t<_Rng2>>;
                if (_Count != static_cast<_Size2>(_RANGES size(_Range2))) {
                    return false;
                }
                return _RANGES _Equal_count(
                    _Ubegin(_Range1), _Ubegin(_Range2), _Count, _Pass_fn(_Pred), _Pass_fn(_Proj1), _Pass_fn(_Proj2));
            }
            else {
                return _Equal_4(_Ubegin(_Range1), _Uend(_Range1), _Ubegin(_Range2), _Uend(_Range2), _Pass_fn(_Pred),
                    _Pass_fn(_Proj1), _Pass_fn(_Proj2));
            }
        }
    };

    _EXPORT_STD inline constexpr _Equal_fn equal{ _Not_quite_object::_Construct_tag{} };
} // namespace ranges
#endif // __cpp_lib_concepts

_EXPORT_STD template <class _FwdIt1, class _FwdIt2, class _Pr>
_NODISCARD _CONSTEXPR20 bool is_permutation(_FwdIt1 _First1, _FwdIt1 _Last1, _FwdIt2 _First2, _Pr _Pred) {
    // test if [_First1, _Last1) == permuted [_First2, ...)
    _Adl_verify_range(_First1, _Last1);
    auto _UFirst1 = _Get_unwrapped(_First1);
    const auto _ULast1 = _Get_unwrapped(_Last1);
    auto _UFirst2 = _Get_unwrapped_n(_First2, _Idl_distance<_FwdIt1>(_UFirst1, _ULast1));

    for (;; ++_UFirst1, (void) ++_UFirst2) { // trim matching prefix
        if (_UFirst1 == _ULast1) { // everything matched
            return true;
        }

        if (!_Pred(*_UFirst1, *_UFirst2)) { // found first inequality, check match counts in suffix
            break;
        }
    }

    // Narrowing _Iter_diff_t<_FwdIt1> to _Iter_diff_t<_FwdIt2> is OK because the second range must be at least as long
    // as the first.
    const auto _Dist2 = static_cast<_Iter_diff_t<_FwdIt2>>(_STD distance(_UFirst1, _ULast1));
    return _Check_match_counts(_UFirst1, _ULast1, _UFirst2, _STD next(_UFirst2, _Dist2), _Pass_fn(_Pred));
}

_EXPORT_STD template <class _FwdIt1, class _FwdIt2>
_NODISCARD _CONSTEXPR20 bool is_permutation(_FwdIt1 _First1, _FwdIt1 _Last1, _FwdIt2 _First2) {
    // test if [_First1, _Last1) == permuted [_First2, ...)
    return _STD is_permutation(_First1, _Last1, _First2, equal_to<>{});
}

_EXPORT_STD template <class _FwdIt1, class _FwdIt2, class _Pr>
_NODISCARD _CONSTEXPR20 bool is_permutation(
    _FwdIt1 _First1, _FwdIt1 _Last1, _FwdIt2 _First2, _FwdIt2 _Last2, _Pr _Pred) {
    // test if [_First1, _Last1) == permuted [_First2, _Last2)
    _Adl_verify_range(_First1, _Last1);
    _Adl_verify_range(_First2, _Last2);
    auto _UFirst1 = _Get_unwrapped(_First1);
    auto _ULast1 = _Get_unwrapped(_Last1);
    auto _UFirst2 = _Get_unwrapped(_First2);
    auto _ULast2 = _Get_unwrapped(_Last2);
    if constexpr (_Is_ranges_random_iter_v<_FwdIt1> && _Is_ranges_random_iter_v<_FwdIt2>) {
        if (_ULast1 - _UFirst1 != _ULast2 - _UFirst2) {
            return false;
        }

        for (; _UFirst1 != _ULast1; ++_UFirst1, (void) ++_UFirst2) { // trim matching prefix
            if (!_Pred(*_UFirst1, *_UFirst2)) {
                // found first inequality, check match counts in suffix
                return _Check_match_counts(_UFirst1, _ULast1, _UFirst2, _ULast2, _Pass_fn(_Pred));
            }
        }

        return true;
    }
    else {
        static_assert(_Is_ranges_fwd_iter_v<_FwdIt1> && _Is_ranges_fwd_iter_v<_FwdIt2>,
            "Iterators must be at least forward iterators");
        for (;; ++_UFirst1, (void) ++_UFirst2) { // trim matching prefix
            if (_UFirst1 == _ULast1) {
                return _UFirst2 == _ULast2;
            }

            if (_UFirst2 == _ULast2) {
                return false;
            }

            if (!_Pred(*_UFirst1, *_UFirst2)) { // found first inequality, check match counts in suffix
                break;
            }
        }

        auto _Next1 = _UFirst1;
        auto _Next2 = _UFirst2;
        for (;; ++_Next1, (void) ++_Next2) { // check for same lengths
            if (_Next1 == _ULast1) {
                if (_Next2 == _ULast2) {
                    return _Check_match_counts(_UFirst1, _ULast1, _UFirst2, _ULast2, _Pass_fn(_Pred));
                }

                return false; // sequence 1 is shorter than sequence 2, not a permutation
            }

            if (_Next2 == _ULast2) {
                return false; // sequence 1 is longer than sequence 2, not a permutation
            }
        }
    }
}

_EXPORT_STD template <class _FwdIt1, class _FwdIt2>
_NODISCARD _CONSTEXPR20 bool is_permutation(_FwdIt1 _First1, _FwdIt1 _Last1, _FwdIt2 _First2, _FwdIt2 _Last2) {
    // test if [_First1, _Last1) == permuted [_First2, _Last2)
    return _STD is_permutation(_First1, _Last1, _First2, _Last2, equal_to<>{});
}

#ifdef __cpp_lib_concepts
namespace ranges {
    class _Is_permutation_fn : private _Not_quite_object {
    public:
        using _Not_quite_object::_Not_quite_object;

        template <forward_iterator _It1, sentinel_for<_It1> _Se1, forward_iterator _It2, sentinel_for<_It2> _Se2,
            class _Pj1 = identity, class _Pj2 = identity,
            indirect_equivalence_relation<projected<_It1, _Pj1>, projected<_It2, _Pj2>> _Pr = ranges::equal_to>
        _NODISCARD constexpr bool operator()(_It1 _First1, _Se1 _Last1, _It2 _First2, _Se2 _Last2, _Pr _Pred = {},
            _Pj1 _Proj1 = {}, _Pj2 _Proj2 = {}) const {
            _Adl_verify_range(_First1, _Last1);
            _Adl_verify_range(_First2, _Last2);
            auto _UFirst1 = _Unwrap_iter<_Se1>(_STD move(_First1));
            auto _ULast1 = _Unwrap_sent<_It1>(_STD move(_Last1));
            auto _UFirst2 = _Unwrap_iter<_Se2>(_STD move(_First2));
            auto _ULast2 = _Unwrap_sent<_It2>(_STD move(_Last2));

            if constexpr (sized_sentinel_for<_Se1, _It1> && sized_sentinel_for<_Se2, _It2>) {
                const auto _Count = _ULast1 - _UFirst1;
                if (_ULast2 - _UFirst2 != _Count) {
                    return false;
                }

                return _Is_permutation_sized(_STD move(_UFirst1), _STD move(_ULast1), _STD move(_UFirst2),
                    _STD move(_ULast2), _Count, _Pass_fn(_Pred), _Pass_fn(_Proj1), _Pass_fn(_Proj2));
            }
            else {
                return _Is_permutation_unsized(_STD move(_UFirst1), _STD move(_ULast1), _STD move(_UFirst2),
                    _STD move(_ULast2), _Pass_fn(_Pred), _Pass_fn(_Proj1), _Pass_fn(_Proj2));
            }
        }

        template <forward_range _Rng1, forward_range _Rng2, class _Pj1 = identity, class _Pj2 = identity,
            indirect_equivalence_relation<projected<iterator_t<_Rng1>, _Pj1>, projected<iterator_t<_Rng2>, _Pj2>> _Pr =
            ranges::equal_to>
        _NODISCARD constexpr bool operator()(
            _Rng1&& _Range1, _Rng2&& _Range2, _Pr _Pred = {}, _Pj1 _Proj1 = {}, _Pj2 _Proj2 = {}) const {
            if constexpr (sized_range<_Rng1> && sized_range<_Rng2>) {
                const auto _Count = _RANGES distance(_Range1);
                if (_RANGES distance(_Range2) != _Count) {
                    return false;
                }

                return _Is_permutation_sized(_Ubegin(_Range1), _Uend(_Range1), _Ubegin(_Range2), _Uend(_Range2), _Count,
                    _Pass_fn(_Pred), _Pass_fn(_Proj1), _Pass_fn(_Proj2));
            }
            else {
                return _Is_permutation_unsized(_Ubegin(_Range1), _Uend(_Range1), _Ubegin(_Range2), _Uend(_Range2),
                    _Pass_fn(_Pred), _Pass_fn(_Proj1), _Pass_fn(_Proj2));
            }
        }

    private:
        template <class _It1, class _Se1, class _It2, class _Se2, class _Pr, class _Pj1, class _Pj2>
        _NODISCARD static constexpr bool _Is_permutation_sized(_It1 _First1, _Se1 _Last1, _It2 _First2, _Se2 _Last2,
            iter_difference_t<_It1> _Count, _Pr _Pred, _Pj1 _Proj1, _Pj2 _Proj2) {
            _STL_INTERNAL_STATIC_ASSERT(forward_iterator<_It1>);
            _STL_INTERNAL_STATIC_ASSERT(sentinel_for<_Se1, _It1>);
            _STL_INTERNAL_STATIC_ASSERT(forward_iterator<_It2>);
            _STL_INTERNAL_STATIC_ASSERT(sentinel_for<_Se2, _It2>);
            _STL_INTERNAL_STATIC_ASSERT(
                indirect_equivalence_relation<_Pr, projected<_It1, _Pj1>, projected<_It2, _Pj2>>);
            _STL_INTERNAL_CHECK(_RANGES distance(_First1, _Last1) == _Count);
            _STL_INTERNAL_CHECK(_RANGES distance(_First2, _Last2) == _Count);

            for (;; ++_First1, (void) ++_First2, --_Count) { // trim matching prefixes
                if (_Count == 0) { // everything matched
                    return true;
                }

                if (!_STD invoke(_Pred, _STD invoke(_Proj1, *_First1), _STD invoke(_Proj2, *_First2))) { // mismatch
                    break;
                }
            }

            if (_Count == 1) { // single non-matching elements remain; not a permutation
                return false;
            }
            // If we get here, _Count > 1 and initial elements do not match.

            if constexpr (bidirectional_iterator<_It1> && bidirectional_iterator<_It2>) {
                // determine final iterator values
                auto _Final1 = _Find_last_iterator(_First1, _Last1, _Count);
                auto _Final2 = _Find_last_iterator(_First2, _Last2, _Count);

                for (;;) { // trim matching suffixes
                    --_Final1;
                    --_Final2;
                    if (!_STD invoke(_Pred, _STD invoke(_Proj1, *_Final1), _STD invoke(_Proj2, *_Final2))) { // mismatch
                        break;
                    }

                    if (--_Count == 1) {
                        return false; // initial elements still do not match
                    }
                }
                // If we get here, _Count > 1, initial elements do not match, and final elements do not match.

                // We've trimmed matching prefixes and matching suffixes.
                // Now we need to compare each range's prefix to the other range's suffix.

                const auto _ProjectedPred = [&]<class _Ty1, class _Ty2>(_Ty1 && _Left, _Ty2 && _Right) -> bool {
                    return _STD invoke(_Pred, _STD invoke(_Proj1, _STD forward<_Ty1>(_Left)),
                        _STD invoke(_Proj2, _STD forward<_Ty2>(_Right)));
                };

                const _TrimResult _Res = _Trim_completely(_First1, _Final1, _First2, _Final2, _ProjectedPred);

                if (_Res != _TrimResult::_HaveWorkAfterTrimming) {
                    return _Res == _TrimResult::_ReturnTrue;
                }

                ++_Final1;
                ++_Final2;
                // If we get here, initial elements do not match, final elements do not match, and ranges have length
                // at least 2 and at most _Count.

                // We've trimmed matching prefixes, matching suffixes,
                // and each range's prefix matching the other range's suffix. That is, given:
                // Range 1: [A, ..., B]
                // Range 2: [X, ..., Y]
                // we know that A != X, A != Y, B != X, and B != Y.
                // (A == B and X == Y are possible but irrelevant.)

                return _Match_counts(_STD move(_First1), _STD move(_Final1), _STD move(_First2), _STD move(_Final2),
                    _Pred, _Proj1, _Proj2);
            }
            else {
                return _Match_counts(_STD move(_First1), _STD move(_Last1), _STD move(_First2), _STD move(_Last2),
                    _Pred, _Proj1, _Proj2);
            }
        }

        template <class _It1, class _Se1, class _It2, class _Se2, class _Pr, class _Pj1, class _Pj2>
        _NODISCARD static constexpr bool _Is_permutation_unsized(
            _It1 _First1, _Se1 _Last1, _It2 _First2, _Se2 _Last2, _Pr _Pred, _Pj1 _Proj1, _Pj2 _Proj2) {
            _STL_INTERNAL_STATIC_ASSERT(forward_iterator<_It1>);
            _STL_INTERNAL_STATIC_ASSERT(sentinel_for<_Se1, _It1>);
            _STL_INTERNAL_STATIC_ASSERT(forward_iterator<_It2>);
            _STL_INTERNAL_STATIC_ASSERT(sentinel_for<_Se2, _It2>);
            _STL_INTERNAL_STATIC_ASSERT(
                indirect_equivalence_relation<_Pr, projected<_It1, _Pj1>, projected<_It2, _Pj2>>);

            for (;; ++_First1, (void) ++_First2) { // trim matching prefixes
                if (_First1 == _Last1) { // first range is a prefix of second
                    return _First2 == _Last2;
                }
                else if (_First2 == _Last2) { // second range is a proper prefix of first
                    return false;
                }

                if (!_STD invoke(_Pred, _STD invoke(_Proj1, *_First1), _STD invoke(_Proj2, *_First2))) { // mismatch
                    break;
                }
            }
            // If we get here, initial elements do not match.

            // determine final iterator values and validate lengths
            auto _Final1 = _First1;
            auto _Final2 = _First2;
            for (;;) {
                ++_Final1;
                ++_Final2;
                if (_Final1 == _Last1) {
                    if (_Final2 == _Last2) {
                        break; // equal lengths
                    }

                    return false; // different lengths; not a permutation
                }
                else if (_Final2 == _Last2) {
                    return false; // ditto different lengths
                }
            }
            // If we get here, initial elements do not match and ranges have equal lengths.

            if constexpr (bidirectional_iterator<_It1> && bidirectional_iterator<_It2>) {
                for (;;) { // trim matching suffixes
                    if (--_Final1 == _First1) {
                        return false; // initial elements still do not match
                    }
                    --_Final2; // since ranges have equal lengths, _Final2 cannot equal _First2

                    if (!_STD invoke(_Pred, _STD invoke(_Proj1, *_Final1), _STD invoke(_Proj2, *_Final2))) { // mismatch
                        break;
                    }
                }
                // If we get here, initial elements do not match, final elements do not match, and ranges have length
                // at least 2.

                // We've trimmed matching prefixes and matching suffixes.
                // Now we need to compare each range's prefix to the other range's suffix.

                const auto _ProjectedPred = [&]<class _Ty1, class _Ty2>(_Ty1 && _Left, _Ty2 && _Right) -> bool {
                    return _STD invoke(_Pred, _STD invoke(_Proj1, _STD forward<_Ty1>(_Left)),
                        _STD invoke(_Proj2, _STD forward<_Ty2>(_Right)));
                };

                const _TrimResult _Res = _Trim_completely(_First1, _Final1, _First2, _Final2, _ProjectedPred);

                if (_Res != _TrimResult::_HaveWorkAfterTrimming) {
                    return _Res == _TrimResult::_ReturnTrue;
                }

                ++_Final1;
                ++_Final2;
                // If we get here, initial elements do not match, final elements do not match, and ranges have length
                // at least 2.

                // We've trimmed matching prefixes, matching suffixes,
                // and each range's prefix matching the other range's suffix. That is, given:
                // Range 1: [A, ..., B]
                // Range 2: [X, ..., Y]
                // we know that A != X, A != Y, B != X, and B != Y.
                // (A == B and X == Y are possible but irrelevant.)
            }

            return _Match_counts(
                _STD move(_First1), _STD move(_Final1), _STD move(_First2), _STD move(_Final2), _Pred, _Proj1, _Proj2);
        }

        template <class _It1, class _Se1, class _It2, class _Se2, class _Pr, class _Pj1, class _Pj2>
        _NODISCARD static constexpr bool _Match_counts(const _It1 _First1, const _Se1 _Last1, const _It2 _First2,
            const _Se2 _Last2, _Pr _Pred, _Pj1 _Proj1, _Pj2 _Proj2) {
            _STL_INTERNAL_STATIC_ASSERT(forward_iterator<_It1>);
            _STL_INTERNAL_STATIC_ASSERT(sentinel_for<_Se1, _It1>);
            _STL_INTERNAL_STATIC_ASSERT(forward_iterator<_It2>);
            _STL_INTERNAL_STATIC_ASSERT(sentinel_for<_Se2, _It2>);
            _STL_INTERNAL_STATIC_ASSERT(
                indirect_equivalence_relation<_Pr, projected<_It1, _Pj1>, projected<_It2, _Pj2>>);

            for (auto _Current = _First1; _Current != _Last1; ++_Current) {
                bool _Found = false;
                auto _Mid1 = _First1;
                for (; _Mid1 != _Current; ++_Mid1) {
                    if (_STD invoke(_Pred, _STD invoke(_Proj1, *_Current), _STD invoke(_Proj1, *_Mid1))) {
                        // this value appears earlier in the first range so we've already counted occurrences
                        _Found = true;
                        break;
                    }
                }

                if (_Found) {
                    continue;
                }

                // count occurrences of this value in the first range
                iter_difference_t<_It1> _Occurrences = 1;
                while (++_Mid1 != _Last1) {
                    if (_STD invoke(_Pred, _STD invoke(_Proj1, *_Current), _STD invoke(_Proj1, *_Mid1))) {
                        ++_Occurrences;
                    }
                }

                // subtract occurrences in the second range
                for (auto _Mid2 = _First2; _Mid2 != _Last2; ++_Mid2) {
                    if (_STD invoke(_Pred, _STD invoke(_Proj1, *_Current), _STD invoke(_Proj2, *_Mid2))) {
                        if (--_Occurrences < 0) {
                            // value appears more in second range than first; not a permutation
                            return false;
                        }
                    }
                }

                if (_Occurrences != 0) {
                    // value appears more in first range than second; not a permutation
                    return false;
                }
            }

            return true;
        }
    };

    _EXPORT_STD inline constexpr _Is_permutation_fn is_permutation{ _Not_quite_object::_Construct_tag{} };
} // namespace ranges
#endif // __cpp_lib_concepts

_EXPORT_STD template <class _InIt, class _Pr>
_NODISCARD _CONSTEXPR20 bool all_of(_InIt _First, _InIt _Last, _Pr _Pred) { // test if all elements satisfy _Pred
    _Adl_verify_range(_First, _Last);
    auto _UFirst = _Get_unwrapped(_First);
    const auto _ULast = _Get_unwrapped(_Last);
    for (; _UFirst != _ULast; ++_UFirst) {
        if (!_Pred(*_UFirst)) {
            return false;
        }
    }

    return true;
}

#if _HAS_CXX17
_EXPORT_STD template <class _ExPo, class _FwdIt, class _Pr, _Enable_if_execution_policy_t<_ExPo> = 0>
_NODISCARD bool all_of(_ExPo&&, _FwdIt _First, _FwdIt _Last, _Pr _Pred) noexcept; // terminates
#endif // _HAS_CXX17

#ifdef __cpp_lib_concepts
namespace ranges {
    class _All_of_fn : private _Not_quite_object {
    public:
        using _Not_quite_object::_Not_quite_object;

        template <input_iterator _It, sentinel_for<_It> _Se, class _Pj = identity,
            indirect_unary_predicate<projected<_It, _Pj>> _Pr>
        _NODISCARD constexpr bool operator()(_It _First, _Se _Last, _Pr _Pred, _Pj _Proj = {}) const {
            _Adl_verify_range(_First, _Last);

            return _All_of_unchecked(_Unwrap_iter<_Se>(_STD move(_First)), _Unwrap_sent<_It>(_STD move(_Last)),
                _Pass_fn(_Pred), _Pass_fn(_Proj));
        }

        template <input_range _Rng, class _Pj = identity,
            indirect_unary_predicate<projected<iterator_t<_Rng>, _Pj>> _Pr>
        _NODISCARD constexpr bool operator()(_Rng&& _Range, _Pr _Pred, _Pj _Proj = {}) const {
            return _All_of_unchecked(_Ubegin(_Range), _Uend(_Range), _Pass_fn(_Pred), _Pass_fn(_Proj));
        }

    private:
        template <class _It, class _Se, class _Pj, class _Pr>
        _NODISCARD static constexpr bool _All_of_unchecked(_It _First, const _Se _Last, _Pr _Pred, _Pj _Proj) {
            _STL_INTERNAL_STATIC_ASSERT(input_iterator<_It>);
            _STL_INTERNAL_STATIC_ASSERT(sentinel_for<_Se, _It>);
            _STL_INTERNAL_STATIC_ASSERT(indirect_unary_predicate<_Pr, projected<_It, _Pj>>);

            for (; _First != _Last; ++_First) {
                if (!_STD invoke(_Pred, _STD invoke(_Proj, *_First))) {
                    return false;
                }
            }

            return true;
        }
    };

    _EXPORT_STD inline constexpr _All_of_fn all_of{ _Not_quite_object::_Construct_tag{} };
} // namespace ranges
#endif // __cpp_lib_concepts

_EXPORT_STD template <class _InIt, class _Pr>
_NODISCARD _CONSTEXPR20 bool any_of(const _InIt _First, const _InIt _Last, _Pr _Pred) {
    // test if any element satisfies _Pred
    _Adl_verify_range(_First, _Last);
    auto _UFirst = _Get_unwrapped(_First);
    const auto _ULast = _Get_unwrapped(_Last);
    for (; _UFirst != _ULast; ++_UFirst) {
        if (_Pred(*_UFirst)) {
            return true;
        }
    }

    return false;
}

#if _HAS_CXX17
_EXPORT_STD template <class _ExPo, class _FwdIt, class _Pr, _Enable_if_execution_policy_t<_ExPo> = 0>
_NODISCARD bool any_of(_ExPo&&, _FwdIt _First, _FwdIt _Last, _Pr _Pred) noexcept; // terminates
#endif // _HAS_CXX17

#ifdef __cpp_lib_concepts
namespace ranges {
    class _Any_of_fn : private _Not_quite_object {
    public:
        using _Not_quite_object::_Not_quite_object;

        template <input_iterator _It, sentinel_for<_It> _Se, class _Pj = identity,
            indirect_unary_predicate<projected<_It, _Pj>> _Pr>
        _NODISCARD constexpr bool operator()(_It _First, _Se _Last, _Pr _Pred, _Pj _Proj = {}) const {
            _Adl_verify_range(_First, _Last);

            return _Any_of_unchecked(_Unwrap_iter<_Se>(_STD move(_First)), _Unwrap_sent<_It>(_STD move(_Last)),
                _Pass_fn(_Pred), _Pass_fn(_Proj));
        }

        template <input_range _Rng, class _Pj = identity,
            indirect_unary_predicate<projected<iterator_t<_Rng>, _Pj>> _Pr>
        _NODISCARD constexpr bool operator()(_Rng&& _Range, _Pr _Pred, _Pj _Proj = {}) const {
            return _Any_of_unchecked(_Ubegin(_Range), _Uend(_Range), _Pass_fn(_Pred), _Pass_fn(_Proj));
        }

    private:
        template <class _It, class _Se, class _Pj, class _Pr>
        _NODISCARD static constexpr bool _Any_of_unchecked(_It _First, const _Se _Last, _Pr _Pred, _Pj _Proj) {
            _STL_INTERNAL_STATIC_ASSERT(input_iterator<_It>);
            _STL_INTERNAL_STATIC_ASSERT(sentinel_for<_Se, _It>);
            _STL_INTERNAL_STATIC_ASSERT(indirect_unary_predicate<_Pr, projected<_It, _Pj>>);

            for (; _First != _Last; ++_First) {
                if (_STD invoke(_Pred, _STD invoke(_Proj, *_First))) {
                    return true;
                }
            }

            return false;
        }
    };

    _EXPORT_STD inline constexpr _Any_of_fn any_of{ _Not_quite_object::_Construct_tag{} };
} // namespace ranges
#endif // __cpp_lib_concepts

_EXPORT_STD template <class _InIt, class _Pr>
_NODISCARD _CONSTEXPR20 bool none_of(const _InIt _First, const _InIt _Last, _Pr _Pred) {
    // test if no elements satisfy _Pred
    _Adl_verify_range(_First, _Last);
    auto _UFirst = _Get_unwrapped(_First);
    const auto _ULast = _Get_unwrapped(_Last);
    for (; _UFirst != _ULast; ++_UFirst) {
        if (_Pred(*_UFirst)) {
            return false;
        }
    }

    return true;
}

#if _HAS_CXX17
_EXPORT_STD template <class _ExPo, class _FwdIt, class _Pr, _Enable_if_execution_policy_t<_ExPo> = 0>
_NODISCARD bool none_of(_ExPo&&, _FwdIt _First, _FwdIt _Last, _Pr _Pred) noexcept; // terminates
#endif // _HAS_CXX17

#ifdef __cpp_lib_concepts
namespace ranges {
    class _None_of_fn : private _Not_quite_object {
    public:
        using _Not_quite_object::_Not_quite_object;

        template <input_iterator _It, sentinel_for<_It> _Se, class _Pj = identity,
            indirect_unary_predicate<projected<_It, _Pj>> _Pr>
        _NODISCARD constexpr bool operator()(_It _First, _Se _Last, _Pr _Pred, _Pj _Proj = {}) const {
            _Adl_verify_range(_First, _Last);

            return _None_of_unchecked(_Unwrap_iter<_Se>(_STD move(_First)), _Unwrap_sent<_It>(_STD move(_Last)),
                _Pass_fn(_Pred), _Pass_fn(_Proj));
        }

        template <input_range _Rng, class _Pj = identity,
            indirect_unary_predicate<projected<iterator_t<_Rng>, _Pj>> _Pr>
        _NODISCARD constexpr bool operator()(_Rng&& _Range, _Pr _Pred, _Pj _Proj = {}) const {
            return _None_of_unchecked(_Ubegin(_Range), _Uend(_Range), _Pass_fn(_Pred), _Pass_fn(_Proj));
        }

    private:
        template <class _It, class _Se, class _Pj, class _Pr>
        _NODISCARD static constexpr bool _None_of_unchecked(_It _First, const _Se _Last, _Pr _Pred, _Pj _Proj) {
            _STL_INTERNAL_STATIC_ASSERT(input_iterator<_It>);
            _STL_INTERNAL_STATIC_ASSERT(sentinel_for<_Se, _It>);
            _STL_INTERNAL_STATIC_ASSERT(indirect_unary_predicate<_Pr, projected<_It, _Pj>>);

            for (; _First != _Last; ++_First) {
                if (_STD invoke(_Pred, _STD invoke(_Proj, *_First))) {
                    return false;
                }
            }

            return true;
        }
    };

    _EXPORT_STD inline constexpr _None_of_fn none_of{ _Not_quite_object::_Construct_tag{} };

#if _HAS_CXX23
    class _Contains_fn : private _Not_quite_object {
    public:
        using _Not_quite_object::_Not_quite_object;

        template <input_iterator _It, sentinel_for<_It> _Se, class _Ty, class _Pj = identity>
            requires indirect_binary_predicate<ranges::equal_to, projected<_It, _Pj>, const _Ty*>
        _NODISCARD constexpr bool operator()(_It _First, _Se _Last, const _Ty& _Val, _Pj _Proj = {}) const {
            _Adl_verify_range(_First, _Last);
            const auto _ULast = _Unwrap_sent<_It>(_STD move(_Last));
            const auto _UResult =
                _RANGES _Find_unchecked(_Unwrap_iter<_Se>(_STD move(_First)), _ULast, _Val, _Pass_fn(_Proj));
            return _UResult != _ULast;
        }

        template <input_range _Rng, class _Ty, class _Pj = identity>
            requires indirect_binary_predicate<ranges::equal_to, projected<iterator_t<_Rng>, _Pj>, const _Ty*>
        _NODISCARD constexpr bool operator()(_Rng&& _Range, const _Ty& _Val, _Pj _Proj = {}) const {
            const auto _UResult = _RANGES _Find_unchecked(_Ubegin(_Range), _Uend(_Range), _Val, _Pass_fn(_Proj));
            return _UResult != _Uend(_Range);
        }
    };

    _EXPORT_STD inline constexpr _Contains_fn contains{ _Not_quite_object::_Construct_tag{} };

    class _Contains_subrange_fn : private _Not_quite_object {
    public:
        using _Not_quite_object::_Not_quite_object;

        template <forward_iterator _It1, sentinel_for<_It1> _Se1, forward_iterator _It2, sentinel_for<_It2> _Se2,
            class _Pr = ranges::equal_to, class _Pj1 = identity, class _Pj2 = identity>
            requires indirectly_comparable<_It1, _It2, _Pr, _Pj1, _Pj2>
        _NODISCARD constexpr bool operator()(_It1 _First1, _Se1 _Last1, _It2 _First2, _Se2 _Last2, _Pr _Pred = {},
            _Pj1 _Proj1 = {}, _Pj2 _Proj2 = {}) const {
            _Adl_verify_range(_First2, _Last2);
            auto _UFirst2 = _Unwrap_iter<_Se2>(_STD move(_First2));
            auto _ULast2 = _Unwrap_sent<_It2>(_STD move(_Last2));

            if (_UFirst2 == _ULast2) {
                return true;
            }

            const auto _Match = _RANGES search(_STD move(_First1), _STD move(_Last1), _STD move(_UFirst2),
                _STD move(_ULast2), _Pass_fn(_Pred), _Pass_fn(_Proj1), _Pass_fn(_Proj2));
            return !_Match.empty();
        }

        template <forward_range _Rng1, forward_range _Rng2, class _Pr = ranges::equal_to, class _Pj1 = identity,
            class _Pj2 = identity>
            requires indirectly_comparable<iterator_t<_Rng1>, iterator_t<_Rng2>, _Pr, _Pj1, _Pj2>
        _NODISCARD constexpr bool operator()(
            _Rng1&& _Range1, _Rng2&& _Range2, _Pr _Pred = {}, _Pj1 _Proj1 = {}, _Pj2 _Proj2 = {}) const {
            if (_RANGES empty(_Range2)) {
                return true;
            }

            const auto _Match = _RANGES search(_Range1, _Range2, _Pass_fn(_Pred), _Pass_fn(_Proj1), _Pass_fn(_Proj2));
            return !_Match.empty();
        }
    };

    _EXPORT_STD inline constexpr _Contains_subrange_fn contains_subrange{ _Not_quite_object::_Construct_tag{} };
#endif // _HAS_CXX23

    _EXPORT_STD template <class _In, class _Out>
        using copy_n_result = in_out_result<_In, _Out>;

    class _Copy_n_fn : private _Not_quite_object {
    public:
        using _Not_quite_object::_Not_quite_object;

        template <input_iterator _It, weakly_incrementable _Out>
            requires indirectly_copyable<_It, _Out>
        constexpr copy_n_result<_It, _Out> operator()(_It _First, iter_difference_t<_It> _Count, _Out _Result) const {
            auto _UFirst = _Get_unwrapped_n(_STD move(_First), _Count);
            if constexpr (_Iter_copy_cat<decltype(_UFirst), _Out>::_Bitcopy_assignable) {
                if (!_STD is_constant_evaluated()) {
                    _Result = _Copy_memmove_n(_UFirst, static_cast<size_t>(_Count), _STD move(_Result));
                    _UFirst += _Count;
                    _Seek_wrapped(_First, _STD move(_UFirst));
                    return { _STD move(_First), _STD move(_Result) };
                }
            }

            for (; _Count > 0; ++_UFirst, (void) ++_Result, --_Count) {
                *_Result = *_UFirst;
            }

            _Seek_wrapped(_First, _STD move(_UFirst));
            return { _STD move(_First), _STD move(_Result) };
        }
    };

    _EXPORT_STD inline constexpr _Copy_n_fn copy_n{ _Not_quite_object::_Construct_tag{} };

    _EXPORT_STD template <class _In, class _Out>
        using copy_backward_result = in_out_result<_In, _Out>;

    class _Copy_backward_fn : private _Not_quite_object {
    public:
        using _Not_quite_object::_Not_quite_object;

        template <bidirectional_iterator _It1, sentinel_for<_It1> _Se1, bidirectional_iterator _It2>
            requires indirectly_copyable<_It1, _It2>
        constexpr copy_backward_result<_It1, _It2> operator()(_It1 _First, _Se1 _Last, _It2 _Result) const {
            _Adl_verify_range(_First, _Last);
            auto _UFirst = _Unwrap_iter<_Se1>(_STD move(_First));
            auto _ULast = _Get_final_iterator_unwrapped<_It1>(_UFirst, _STD move(_Last));
            _Seek_wrapped(_First, _ULast);
            _Result = _Copy_backward_unchecked(_STD move(_UFirst), _STD move(_ULast), _STD move(_Result));
            return { _STD move(_First), _STD move(_Result) };
        }

        template <bidirectional_range _Rng, bidirectional_iterator _It>
            requires indirectly_copyable<iterator_t<_Rng>, _It>
        constexpr copy_backward_result<borrowed_iterator_t<_Rng>, _It> operator()(_Rng&& _Range, _It _Result) const {
            auto _ULast = _Get_final_iterator_unwrapped(_Range);
            _Result = _Copy_backward_unchecked(_Ubegin(_Range), _ULast, _STD move(_Result));
            return { _Rewrap_iterator(_Range, _STD move(_ULast)), _STD move(_Result) };
        }
    };

    _EXPORT_STD inline constexpr _Copy_backward_fn copy_backward{ _Not_quite_object::_Construct_tag{} };
} // namespace ranges
#endif // __cpp_lib_concepts

_EXPORT_STD template <class _InIt, class _OutIt, class _Pr>
_CONSTEXPR20 _OutIt copy_if(_InIt _First, _InIt _Last, _OutIt _Dest, _Pr _Pred) { // copy each satisfying _Pred
    _Adl_verify_range(_First, _Last);
    auto _UFirst = _Get_unwrapped(_First);
    const auto _ULast = _Get_unwrapped(_Last);
    auto _UDest = _Get_unwrapped_unverified(_Dest);
    for (; _UFirst != _ULast; ++_UFirst) {
        if (_Pred(*_UFirst)) {
            *_UDest = *_UFirst;
            ++_UDest;
        }
    }

    _Seek_wrapped(_Dest, _UDest);
    return _Dest;
}

#if _HAS_CXX17
_EXPORT_STD template <class _ExPo, class _FwdIt1, class _FwdIt2, class _Pr, _Enable_if_execution_policy_t<_ExPo> = 0>
_FwdIt2 copy_if(_ExPo&&, _FwdIt1 _First, _FwdIt1 _Last, _FwdIt2 _Dest, _Pr _Pred) noexcept /* terminates */ {
    // copy each satisfying _Pred
    // not parallelized at present, parallelism expected to be feasible in a future release
    _REQUIRE_PARALLEL_ITERATOR(_FwdIt1);
    _REQUIRE_CPP17_MUTABLE_ITERATOR(_FwdIt2);
    return _STD copy_if(_First, _Last, _Dest, _Pass_fn(_Pred));
}
#endif // _HAS_CXX17

#ifdef __cpp_lib_concepts
namespace ranges {
    _EXPORT_STD template <class _In, class _Out>
        using copy_if_result = in_out_result<_In, _Out>;

    class _Copy_if_fn : private _Not_quite_object {
    public:
        using _Not_quite_object::_Not_quite_object;

        template <input_iterator _It, sentinel_for<_It> _Se, weakly_incrementable _Out, class _Pj = identity,
            indirect_unary_predicate<projected<_It, _Pj>> _Pr>
            requires indirectly_copyable<_It, _Out>
        constexpr copy_if_result<_It, _Out> operator()(
            _It _First, _Se _Last, _Out _Result, _Pr _Pred, _Pj _Proj = {}) const {
            _Adl_verify_range(_First, _Last);
            auto _UResult = _Copy_if_unchecked(_Unwrap_iter<_Se>(_STD move(_First)),
                _Unwrap_sent<_It>(_STD move(_Last)), _STD move(_Result), _Pass_fn(_Pred), _Pass_fn(_Proj));
            _Seek_wrapped(_First, _STD move(_UResult.in));
            return { _STD move(_First), _STD move(_UResult.out) };
        }

        template <input_range _Rng, weakly_incrementable _Out, class _Pj = identity,
            indirect_unary_predicate<projected<iterator_t<_Rng>, _Pj>> _Pr>
            requires indirectly_copyable<iterator_t<_Rng>, _Out>
        constexpr copy_if_result<borrowed_iterator_t<_Rng>, _Out> operator()(
            _Rng&& _Range, _Out _Result, _Pr _Pred, _Pj _Proj = {}) const {
            auto _First = _RANGES begin(_Range);
            auto _UResult = _Copy_if_unchecked(_Unwrap_range_iter<_Rng>(_STD move(_First)), _Uend(_Range),
                _STD move(_Result), _Pass_fn(_Pred), _Pass_fn(_Proj));
            _Seek_wrapped(_First, _STD move(_UResult.in));
            return { _STD move(_First), _STD move(_UResult.out) };
        }

    private:
        template <class _It, class _Se, class _Out, class _Pj, class _Pr>
        _NODISCARD static constexpr copy_if_result<_It, _Out> _Copy_if_unchecked(
            _It _First, const _Se _Last, _Out _Result, _Pr _Pred, _Pj _Proj) {
            _STL_INTERNAL_STATIC_ASSERT(input_iterator<_It>);
            _STL_INTERNAL_STATIC_ASSERT(sentinel_for<_Se, _It>);
            _STL_INTERNAL_STATIC_ASSERT(indirect_unary_predicate<_Pr, projected<_It, _Pj>>);
            _STL_INTERNAL_STATIC_ASSERT(indirectly_copyable<_It, _Out>);

            for (; _First != _Last; ++_First) {
                if (_STD invoke(_Pred, _STD invoke(_Proj, *_First))) {
                    *_Result = *_First;
                    ++_Result;
                }
            }

            return { _STD move(_First), _STD move(_Result) };
        }
    };

    _EXPORT_STD inline constexpr _Copy_if_fn copy_if{ _Not_quite_object::_Construct_tag{} };

    _EXPORT_STD template <class _In, class _Out>
        using move_result = in_out_result<_In, _Out>;

    template <input_iterator _It, sentinel_for<_It> _Se, weakly_incrementable _Out>
        requires indirectly_movable<_It, _Out>
    constexpr move_result<_It, _Out> _Move_unchecked(_It _First, _Se _Last, _Out _Result) {
        if constexpr (_Iter_move_cat<_It, _Out>::_Bitcopy_assignable) {
            if (!_STD is_constant_evaluated()) {
                auto _Final = _RANGES next(_First, _STD move(_Last));
                _Result = _Copy_memmove(_STD move(_First), _Final, _STD move(_Result));
                return { _STD move(_Final), _STD move(_Result) };
            }
        }

        for (; _First != _Last; ++_First, (void) ++_Result) {
            *_Result = _RANGES iter_move(_First);
        }

        return { _STD move(_First), _STD move(_Result) };
    }

    class _Move_fn : private _Not_quite_object {
    public:
        using _Not_quite_object::_Not_quite_object;

        template <input_iterator _It, sentinel_for<_It> _Se, weakly_incrementable _Out>
            requires indirectly_movable<_It, _Out>
        constexpr move_result<_It, _Out> operator()(_It _First, _Se _Last, _Out _Result) const {
            _Adl_verify_range(_First, _Last);
            auto _UResult = _RANGES _Move_unchecked(
                _Unwrap_iter<_Se>(_STD move(_First)), _Unwrap_sent<_It>(_STD move(_Last)), _STD move(_Result));

            _Seek_wrapped(_First, _STD move(_UResult.in));
            return { _STD move(_First), _STD move(_UResult.out) };
        }

        template <input_range _Rng, weakly_incrementable _Out>
            requires indirectly_movable<iterator_t<_Rng>, _Out>
        constexpr move_result<borrowed_iterator_t<_Rng>, _Out> operator()(_Rng&& _Range, _Out _Result) const {
            auto _First = _RANGES begin(_Range);
            auto _UResult =
                _RANGES _Move_unchecked(_Unwrap_range_iter<_Rng>(_STD move(_First)), _Uend(_Range), _STD move(_Result));

            _Seek_wrapped(_First, _STD move(_UResult.in));
            return { _STD move(_First), _STD move(_UResult.out) };
        }
    };

    _EXPORT_STD inline constexpr _Move_fn move{ _Not_quite_object::_Construct_tag{} };

    _EXPORT_STD template <class _In, class _Out>
        using move_backward_result = in_out_result<_In, _Out>;

    // concept-constrained for strict enforcement as it is used by several algorithms
    template <bidirectional_iterator _It1, bidirectional_iterator _It2>
        requires indirectly_movable<_It1, _It2>
    constexpr _It2 _Move_backward_common(const _It1 _First, _It1 _Last, _It2 _Result) {
        if constexpr (_Iter_move_cat<_It1, _It2>::_Bitcopy_assignable) {
            if (!_STD is_constant_evaluated()) {
                return _Copy_backward_memmove(_First, _Last, _Result);
            }
        }

        while (_First != _Last) {
            *--_Result = _RANGES iter_move(--_Last);
        }

        return _Result;
    }

    class _Move_backward_fn : private _Not_quite_object {
    public:
        using _Not_quite_object::_Not_quite_object;

        template <bidirectional_iterator _It1, sentinel_for<_It1> _Se1, bidirectional_iterator _It2>
            requires indirectly_movable<_It1, _It2>
        constexpr move_backward_result<_It1, _It2> operator()(_It1 _First, _Se1 _Last, _It2 _Result) const {
            _Adl_verify_range(_First, _Last);
            auto _UFirst = _Unwrap_iter<_Se1>(_STD move(_First));
            auto _ULast = _Get_final_iterator_unwrapped<_It1>(_UFirst, _STD move(_Last));
            _Seek_wrapped(_First, _ULast);
            _Result = _RANGES _Move_backward_common(_STD move(_UFirst), _STD move(_ULast), _STD move(_Result));
            return { _STD move(_First), _STD move(_Result) };
        }

        template <bidirectional_range _Rng, bidirectional_iterator _It>
            requires indirectly_movable<iterator_t<_Rng>, _It>
        constexpr move_backward_result<borrowed_iterator_t<_Rng>, _It> operator()(_Rng&& _Range, _It _Result) const {
            auto _ULast = _Get_final_iterator_unwrapped(_Range);
            _Result = _RANGES _Move_backward_common(_Ubegin(_Range), _ULast, _STD move(_Result));
            return { _Rewrap_iterator(_Range, _STD move(_ULast)), _STD move(_Result) };
        }
    };

    _EXPORT_STD inline constexpr _Move_backward_fn move_backward{ _Not_quite_object::_Construct_tag{} };
} // namespace ranges
#endif // __cpp_lib_concepts

_EXPORT_STD template <class _InIt, class _OutIt1, class _OutIt2, class _Pr>
_CONSTEXPR20 pair<_OutIt1, _OutIt2> partition_copy(
    _InIt _First, _InIt _Last, _OutIt1 _Dest_true, _OutIt2 _Dest_false, _Pr _Pred) {
    // copy true partition to _Dest_true, false to _Dest_false
    _Adl_verify_range(_First, _Last);
    auto _UFirst = _Get_unwrapped(_First);
    const auto _ULast = _Get_unwrapped(_Last);
    auto _UDest_true = _Get_unwrapped_unverified(_Dest_true);
    auto _UDest_false = _Get_unwrapped_unverified(_Dest_false);
    for (; _UFirst != _ULast; ++_UFirst) {
        if (_Pred(*_UFirst)) {
            *_UDest_true = *_UFirst;
            ++_UDest_true;
        }
        else {
            *_UDest_false = *_UFirst;
            ++_UDest_false;
        }
    }

    _Seek_wrapped(_Dest_false, _UDest_false);
    _Seek_wrapped(_Dest_true, _UDest_true);
    return { _Dest_true, _Dest_false };
}

#if _HAS_CXX17
_EXPORT_STD template <class _ExPo, class _FwdIt1, class _FwdIt2, class _FwdIt3, class _Pr,
    _Enable_if_execution_policy_t<_ExPo> = 0>
    pair<_FwdIt2, _FwdIt3> partition_copy(_ExPo&&, _FwdIt1 _First, _FwdIt1 _Last, _FwdIt2 _Dest_true, _FwdIt3 _Dest_false,
        _Pr _Pred) noexcept /* terminates */ {
    // copy true partition to _Dest_true, false to _Dest_false
    // not parallelized at present, parallelism expected to be feasible in a future release
    _REQUIRE_PARALLEL_ITERATOR(_FwdIt1);
    _REQUIRE_CPP17_MUTABLE_ITERATOR(_FwdIt2);
    _REQUIRE_CPP17_MUTABLE_ITERATOR(_FwdIt3);
    return _STD partition_copy(_First, _Last, _Dest_true, _Dest_false, _Pass_fn(_Pred));
}

#ifdef __cpp_lib_concepts
namespace ranges {
    _EXPORT_STD template <class _In, class _Out1, class _Out2>
        using partition_copy_result = in_out_out_result<_In, _Out1, _Out2>;

    class _Partition_copy_fn : private _Not_quite_object {
    public:
        using _Not_quite_object::_Not_quite_object;

        template <input_iterator _It, sentinel_for<_It> _Se, weakly_incrementable _Out1, weakly_incrementable _Out2,
            class _Pj = identity, indirect_unary_predicate<projected<_It, _Pj>> _Pr>
            requires indirectly_copyable<_It, _Out1>&& indirectly_copyable<_It, _Out2>
        constexpr partition_copy_result<_It, _Out1, _Out2> operator()(
            _It _First, _Se _Last, _Out1 _Dest_true, _Out2 _Dest_false, _Pr _Pred, _Pj _Proj = {}) const {
            _Adl_verify_range(_First, _Last);
            auto _UResult = _Partition_copy_unchecked(_Unwrap_iter<_Se>(_STD move(_First)),
                _Unwrap_sent<_It>(_STD move(_Last)), _Get_unwrapped_unverified(_STD move(_Dest_true)),
                _Get_unwrapped_unverified(_STD move(_Dest_false)), _Pass_fn(_Pred), _Pass_fn(_Proj));
            _Seek_wrapped(_First, _STD move(_UResult.in));
            _Seek_wrapped(_Dest_true, _STD move(_UResult.out1));
            _Seek_wrapped(_Dest_false, _STD move(_UResult.out2));
            return { _STD move(_First), _STD move(_Dest_true), _STD move(_Dest_false) };
        }

        template <input_range _Rng, weakly_incrementable _Out1, weakly_incrementable _Out2, class _Pj = identity,
            indirect_unary_predicate<projected<iterator_t<_Rng>, _Pj>> _Pr>
            requires indirectly_copyable<iterator_t<_Rng>, _Out1>&& indirectly_copyable<iterator_t<_Rng>, _Out2>
        constexpr partition_copy_result<borrowed_iterator_t<_Rng>, _Out1, _Out2> operator()(
            _Rng&& _Range, _Out1 _Dest_true, _Out2 _Dest_false, _Pr _Pred, _Pj _Proj = {}) const {
            auto _First = _RANGES begin(_Range);
            auto _UResult = _Partition_copy_unchecked(_Unwrap_range_iter<_Rng>(_STD move(_First)), _Uend(_Range),
                _Get_unwrapped_unverified(_STD move(_Dest_true)), _Get_unwrapped_unverified(_STD move(_Dest_false)),
                _Pass_fn(_Pred), _Pass_fn(_Proj));
            _Seek_wrapped(_First, _STD move(_UResult.in));
            _Seek_wrapped(_Dest_true, _STD move(_UResult.out1));
            _Seek_wrapped(_Dest_false, _STD move(_UResult.out2));
            return { _STD move(_First), _STD move(_Dest_true), _STD move(_Dest_false) };
        }

    private:
        template <class _It, class _Se, class _Out1, class _Out2, class _Pr, class _Pj>
        _NODISCARD static constexpr partition_copy_result<_It, _Out1, _Out2> _Partition_copy_unchecked(
            _It _First, const _Se _Last, _Out1 _Dest_true, _Out2 _Dest_false, _Pr _Pred, _Pj _Proj) {
            // copy true partition to _Dest_true, false to _Dest_false
            _STL_INTERNAL_STATIC_ASSERT(input_iterator<_It>);
            _STL_INTERNAL_STATIC_ASSERT(sentinel_for<_Se, _It>);
            _STL_INTERNAL_STATIC_ASSERT(weakly_incrementable<_Out1> && weakly_incrementable<_Out2>);
            _STL_INTERNAL_STATIC_ASSERT(indirect_unary_predicate<_Pr, projected<_It, _Pj>>);
            _STL_INTERNAL_STATIC_ASSERT(indirectly_copyable<_It, _Out1> && indirectly_copyable<_It, _Out2>);

            for (; _First != _Last; ++_First) {
                if (_STD invoke(_Pred, _STD invoke(_Proj, *_First))) {
                    *_Dest_true = *_First;
                    ++_Dest_true;
                }
                else {
                    *_Dest_false = *_First;
                    ++_Dest_false;
                }
            }

            return { _STD move(_First), _STD move(_Dest_true), _STD move(_Dest_false) };
        }
    };

    _EXPORT_STD inline constexpr _Partition_copy_fn partition_copy{ _Not_quite_object::_Construct_tag{} };
} // namespace ranges
#endif // __cpp_lib_concepts
#endif // _HAS_CXX17

_EXPORT_STD template <class _InIt, class _Pr>
_NODISCARD _CONSTEXPR20 bool is_partitioned(const _InIt _First, const _InIt _Last, _Pr _Pred) {
    // test if [_First, _Last) partitioned by _Pred
    _Adl_verify_range(_First, _Last);
    auto _UFirst = _Get_unwrapped(_First);
    const auto _ULast = _Get_unwrapped(_Last);

    for (;; ++_UFirst) { // skip true partition
        if (_UFirst == _ULast) {
            return true;
        }

        if (!_Pred(*_UFirst)) {
            break;
        }
    }

    while (++_UFirst != _ULast) { // verify false partition
        if (_Pred(*_UFirst)) {
            return false; // found out of place element
        }
    }

    return true;
}

#if _HAS_CXX17
_EXPORT_STD template <class _ExPo, class _FwdIt, class _Pr, _Enable_if_execution_policy_t<_ExPo> = 0>
_NODISCARD bool is_partitioned(_ExPo&&, _FwdIt _First, _FwdIt _Last, _Pr _Pred) noexcept; // terminates
#endif // _HAS_CXX17

#ifdef __cpp_lib_concepts
namespace ranges {
    class _Is_partitioned_fn : private _Not_quite_object {
    public:
        using _Not_quite_object::_Not_quite_object;

        template <input_iterator _It, sentinel_for<_It> _Se, class _Pj = identity,
            indirect_unary_predicate<projected<_It, _Pj>> _Pr>
        _NODISCARD constexpr bool operator()(_It _First, _Se _Last, _Pr _Pred, _Pj _Proj = {}) const {
            _Adl_verify_range(_First, _Last);
            return _Is_partitioned_unchecked(_Unwrap_iter<_Se>(_STD move(_First)), _Unwrap_sent<_It>(_STD move(_Last)),
                _Pass_fn(_Pred), _Pass_fn(_Proj));
        }

        template <input_range _Rng, class _Pj = identity,
            indirect_unary_predicate<projected<iterator_t<_Rng>, _Pj>> _Pr>
        _NODISCARD constexpr bool operator()(_Rng&& _Range, _Pr _Pred, _Pj _Proj = {}) const {
            return _Is_partitioned_unchecked(_Ubegin(_Range), _Uend(_Range), _Pass_fn(_Pred), _Pass_fn(_Proj));
        }

    private:
        template <class _It, class _Se, class _Pr, class _Pj>
        _NODISCARD static constexpr bool _Is_partitioned_unchecked(_It _First, const _Se _Last, _Pr _Pred, _Pj _Proj) {
            // test if [_First, _Last) is partitioned with respect to _Pred and _Proj
            _STL_INTERNAL_STATIC_ASSERT(input_iterator<_It>);
            _STL_INTERNAL_STATIC_ASSERT(sentinel_for<_Se, _It>);
            _STL_INTERNAL_STATIC_ASSERT(indirect_unary_predicate<_Pr, projected<_It, _Pj>>);

            for (;; ++_First) { // skip true partition
                if (_First == _Last) {
                    return true;
                }

                if (!_STD invoke(_Pred, _STD invoke(_Proj, *_First))) {
                    break;
                }
            }

            while (++_First != _Last) { // verify false partition
                if (_STD invoke(_Pred, _STD invoke(_Proj, *_First))) {
                    return false; // found out of place element
                }
            }

            return true;
        }
    };

    _EXPORT_STD inline constexpr _Is_partitioned_fn is_partitioned{ _Not_quite_object::_Construct_tag{} };
} // namespace ranges
#endif // __cpp_lib_concepts

_EXPORT_STD template <class _FwdIt, class _Pr>
_NODISCARD _CONSTEXPR20 _FwdIt partition_point(_FwdIt _First, _FwdIt _Last, _Pr _Pred) {
    // find beginning of false partition in [_First, _Last)
    _Adl_verify_range(_First, _Last);
    auto _UFirst = _Get_unwrapped(_First);
    const auto _ULast = _Get_unwrapped(_Last);
    auto _Count = _STD distance(_UFirst, _ULast);
    while (0 < _Count) { // divide and conquer, find half that contains answer
        const auto _Count2 = static_cast<_Iter_diff_t<_FwdIt>>(_Count / 2);
        const auto _UMid = _STD next(_UFirst, _Count2);

        if (_Pred(*_UMid)) { // try top half
            _UFirst = _Next_iter(_UMid);
            _Count -= _Count2;
            --_Count;
        }
        else {
            _Count = _Count2;
        }
    }

    _Seek_wrapped(_First, _UFirst);
    return _First;
}

#ifdef __cpp_lib_concepts
namespace ranges {
    class _Partition_point_fn : private _Not_quite_object {
    public:
        using _Not_quite_object::_Not_quite_object;

        template <forward_iterator _It, sentinel_for<_It> _Se, class _Pj = identity,
            indirect_unary_predicate<projected<_It, _Pj>> _Pr>
        _NODISCARD constexpr _It operator()(_It _First, _Se _Last, _Pr _Pred, _Pj _Proj = {}) const {
            _Adl_verify_range(_First, _Last);
            auto _UFirst = _Unwrap_iter<_Se>(_STD move(_First));
            if constexpr (sized_sentinel_for<_Se, _It>) {
                const auto _Length = _Unwrap_sent<_It>(_STD move(_Last)) - _UFirst;
                _UFirst = _Partition_point_n_unchecked(_STD move(_UFirst), _Length, _Pass_fn(_Pred), _Pass_fn(_Proj));
            }
            else {
                _UFirst = _Partition_point_unchecked(
                    _STD move(_UFirst), _Unwrap_sent<_It>(_STD move(_Last)), _Pass_fn(_Pred), _Pass_fn(_Proj));
            }

            _Seek_wrapped(_First, _STD move(_UFirst));
            return _First;
        }

        template <forward_range _Rng, class _Pj = identity,
            indirect_unary_predicate<projected<iterator_t<_Rng>, _Pj>> _Pr>
        _NODISCARD constexpr borrowed_iterator_t<_Rng> operator()(_Rng&& _Range, _Pr _Pred, _Pj _Proj = {}) const {
            if constexpr (sized_range<_Rng>) {
                const auto _Length = _RANGES distance(_Range);
                auto _UFirst = _Partition_point_n_unchecked(_Ubegin(_Range), _Length, _Pass_fn(_Pred), _Pass_fn(_Proj));
                return _Rewrap_iterator(_Range, _STD move(_UFirst));
            }
            else {
                auto _UFirst =
                    _Partition_point_unchecked(_Ubegin(_Range), _Uend(_Range), _Pass_fn(_Pred), _Pass_fn(_Proj));
                return _Rewrap_iterator(_Range, _STD move(_UFirst));
            }
        }

    private:
        template <class _It, class _Pr, class _Pj>
        _NODISCARD static constexpr _It _Partition_point_n_unchecked(
            _It _First, iter_difference_t<_It> _Length, _Pr _Pred, _Pj _Proj) {
            _STL_INTERNAL_STATIC_ASSERT(forward_iterator<_It>);
            _STL_INTERNAL_STATIC_ASSERT(indirect_unary_predicate<_Pr, projected<_It, _Pj>>);
            _STL_INTERNAL_CHECK(_Length >= 0);

            while (_Length > 0) {
                const auto _Half = static_cast<iter_difference_t<_It>>(_Length / 2);
                auto _Mid = _RANGES next(_First, _Half);
                if (_STD invoke(_Pred, _STD invoke(_Proj, *_Mid))) { // _Mid is before the partition point
                    _First = _STD move(_Mid);
                    ++_First;
                    _Length -= _Half;
                    --_Length;
                }
                else { // _Mid is at or past the partition point
                    _Length = _Half;
                }
            }

            return _First;
        }

        template <class _It, class _Se, class _Pr, class _Pj>
        _NODISCARD static constexpr _It _Partition_point_unchecked(_It _First, const _Se _Last, _Pr _Pred, _Pj _Proj) {
            _STL_INTERNAL_STATIC_ASSERT(forward_iterator<_It>);
            _STL_INTERNAL_STATIC_ASSERT(sentinel_for<_Se, _It>);
            _STL_INTERNAL_STATIC_ASSERT(indirect_unary_predicate<_Pr, projected<_It, _Pj>>);

            // Instead of blindly seeking the end of the range, probe elements at exponentially increasing intervals to
            // find an element past the partition point.
            iter_difference_t<_It> _Skip = 2;
            for (;;) {
                auto _Mid = _First;
                _Skip -= _RANGES advance(_Mid, _Skip, _Last);
                if (_Mid == _Last) { // we've located the end of the range
                    break;
                }

                if (!_STD invoke(_Pred, _STD invoke(_Proj, *_Mid))) { // _Mid is at or past the partition point
                    break;
                }

                _First = _STD move(_Mid);
                ++_First;

                using _Uty = _Make_unsigned_like_t<iter_difference_t<_It>>;
                if (static_cast<_Uty>(_Skip) <= (static_cast<_Uty>(-1) >> 1)) {
                    _Skip <<= 1;
                }
            }

            return _Partition_point_n_unchecked(_STD move(_First), _Skip, _Pred, _Proj);
        }
    };

    _EXPORT_STD inline constexpr _Partition_point_fn partition_point{ _Not_quite_object::_Construct_tag{} };
} // namespace ranges
#endif // __cpp_lib_concepts

template <class _InIt1, class _InIt2, class _Pr>
_NODISCARD _CONSTEXPR20 bool _Equal_rev_pred_unchecked(_InIt1 _First1, _InIt2 _First2, const _InIt2 _Last2, _Pr _Pred) {
    // compare [_First1, ...) to [_First2, _Last2)
    if constexpr (_Equal_memcmp_is_safe<_InIt1, _InIt2, _Pr>) {
#if _HAS_CXX20
        if (!_STD is_constant_evaluated())
#endif // _HAS_CXX20
        {
            return _Memcmp_ranges(_First2, _Last2, _First1) == 0;
        }
    }

    for (; _First2 != _Last2; ++_First1, (void) ++_First2) {
        if (!_Pred(*_First1, *_First2)) {
            return false;
        }
    }

    return true;
}

_EXPORT_STD template <class _FwdItHaystack, class _FwdItPat, class _Pr>
_NODISCARD _CONSTEXPR20 _FwdItHaystack search(_FwdItHaystack _First1, _FwdItHaystack _Last1, const _FwdItPat _First2,
    const _FwdItPat _Last2, _Pr _Pred) { // find first [_First2, _Last2) satisfying _Pred
    _Adl_verify_range(_First1, _Last1);
    _Adl_verify_range(_First2, _Last2);
    auto _UFirst1 = _Get_unwrapped(_First1);
    const auto _ULast1 = _Get_unwrapped(_Last1);
    const auto _UFirst2 = _Get_unwrapped(_First2);
    const auto _ULast2 = _Get_unwrapped(_Last2);
    if constexpr (_Is_ranges_random_iter_v<_FwdItHaystack> && _Is_ranges_random_iter_v<_FwdItPat>) {
        const _Iter_diff_t<_FwdItPat> _Count2 = _ULast2 - _UFirst2;
        if (_ULast1 - _UFirst1 >= _Count2) {
            const auto _Last_possible = _ULast1 - static_cast<_Iter_diff_t<_FwdItHaystack>>(_Count2);
            for (;; ++_UFirst1) {
                if (_Equal_rev_pred_unchecked(_UFirst1, _UFirst2, _ULast2, _Pass_fn(_Pred))) {
                    _Seek_wrapped(_Last1, _UFirst1);
                    break;
                }

                if (_UFirst1 == _Last_possible) {
                    break;
                }
            }
        }
    }
    else {
        for (;; ++_UFirst1) { // loop until match or end of a sequence
            auto _UMid1 = _UFirst1;
            for (auto _UMid2 = _UFirst2;; ++_UMid1, (void) ++_UMid2) {
                if (_UMid2 == _ULast2) {
                    _Seek_wrapped(_Last1, _UFirst1);
                    return _Last1;
                }
                else if (_UMid1 == _ULast1) {
                    return _Last1;
                }
                else if (!_Pred(*_UMid1, *_UMid2)) {
                    break;
                }
            }
        }
    }

    return _Last1;
}

#if _HAS_CXX17
_EXPORT_STD template <class _ExPo, class _FwdItHaystack, class _FwdItPat, class _Pr,
    _Enable_if_execution_policy_t<_ExPo> = 0>
    _NODISCARD _FwdItHaystack search(_ExPo&& _Exec, _FwdItHaystack _First1, _FwdItHaystack _Last1, _FwdItPat _First2,
        _FwdItPat _Last2, _Pr _Pred) noexcept; // terminates
#endif // _HAS_CXX17

_EXPORT_STD template <class _FwdItHaystack, class _FwdItPat>
_NODISCARD _CONSTEXPR20 _FwdItHaystack search(
    const _FwdItHaystack _First1, const _FwdItHaystack _Last1, const _FwdItPat _First2, const _FwdItPat _Last2) {
    // find first [_First2, _Last2) match
    return _STD search(_First1, _Last1, _First2, _Last2, equal_to<>{});
}

#if _HAS_CXX17
_EXPORT_STD template <class _ExPo, class _FwdItHaystack, class _FwdItPat, _Enable_if_execution_policy_t<_ExPo> = 0>
_NODISCARD _FwdItHaystack search(_ExPo&& _Exec, const _FwdItHaystack _First1, const _FwdItHaystack _Last1,
    const _FwdItPat _First2, const _FwdItPat _Last2) noexcept /* terminates */ {
    // find first [_First2, _Last2) match
    return _STD search(_STD forward<_ExPo>(_Exec), _First1, _Last1, _First2, _Last2, equal_to{});
}
#endif // _HAS_CXX17

_EXPORT_STD template <class _FwdItHaystack, class _Searcher>
_NODISCARD _CONSTEXPR20 _FwdItHaystack search(
    const _FwdItHaystack _First, const _FwdItHaystack _Last, const _Searcher& _Search) {
    // find _Search's pattern in [_First, _Last)
    return _Search(_First, _Last).first;
}

_EXPORT_STD template <class _FwdIt, class _Diff, class _Ty, class _Pr>
_NODISCARD _CONSTEXPR20 _FwdIt search_n(
    const _FwdIt _First, _FwdIt _Last, const _Diff _Count_raw, const _Ty& _Val, _Pr _Pred) {
    // find first _Count * _Val satisfying _Pred
    const _Algorithm_int_t<_Diff> _Count = _Count_raw;
    if (_Count <= 0) {
        return _First;
    }

    if (static_cast<uintmax_t>(_Count) > static_cast<uintmax_t>((numeric_limits<_Iter_diff_t<_FwdIt>>::max)())) {
        // if the number of _Vals searched for is larger than the longest possible sequence, we can't find it
        return _Last;
    }

    _Adl_verify_range(_First, _Last);
    auto _UFirst = _Get_unwrapped(_First);
    const auto _ULast = _Get_unwrapped(_Last);
    if constexpr (_Is_ranges_random_iter_v<_FwdIt>) {
        const auto _Count_diff = static_cast<_Iter_diff_t<_FwdIt>>(_Count);
        auto _UOld_first = _UFirst;
        for (_Iter_diff_t<_FwdIt> _Inc = 0; _Count_diff <= _ULast - _UOld_first;) { // enough room, look for a match
            _UFirst = _UOld_first + _Inc;
            if (_Pred(*_UFirst, _Val)) { // found part of possible match, check it out
                _Iter_diff_t<_FwdIt> _Count1 = _Count_diff;
                auto _UMid = _UFirst;

                while (_UOld_first != _UFirst && _Pred(*_Prev_iter(_UFirst), _Val)) { // back up over any skipped prefix
                    --_Count1;
                    --_UFirst;
                }

                if (_Count1 <= _ULast - _UMid) {
                    for (;;) { // enough left, test suffix
                        if (--_Count1 == 0) {
                            _Seek_wrapped(_Last, _UFirst); // found rest of match, report it
                            return _Last;
                        }
                        else if (!_Pred(*++_UMid, _Val)) { // short match not at end
                            break;
                        }
                    }
                }
                _UOld_first = ++_UMid; // failed match, take small jump
                _Inc = 0;
            }
            else { // no match, take big jump and back up as needed
                _UOld_first = _Next_iter(_UFirst);
                _Inc = _Count_diff - 1;
            }
        }
    }
    else {
        for (; _UFirst != _ULast; ++_UFirst) {
            if (_Pred(*_UFirst, _Val)) { // found start of possible match, check it out
                auto _UMid = _UFirst;

                for (_Algorithm_int_t<_Diff> _Count1 = _Count;;) {
                    if (--_Count1 == 0) {
                        _Seek_wrapped(_Last, _UFirst); // found rest of match, report it
                        return _Last;
                    }
                    else if (++_UMid == _ULast) {
                        return _Last; // short match at end
                    }
                    else if (!_Pred(*_UMid, _Val)) { // short match not at end
                        break;
                    }
                }

                _UFirst = _UMid; // pick up just beyond failed match
            }
        }
    }

    return _Last;
}

#if _HAS_CXX17
_EXPORT_STD template <class _ExPo, class _FwdIt, class _Diff, class _Ty, class _Pr,
    _Enable_if_execution_policy_t<_ExPo> = 0>
    _NODISCARD _FwdIt search_n(_ExPo&& _Exec, _FwdIt _First, _FwdIt _Last, _Diff _Count_raw, const _Ty& _Val,
        _Pr _Pred) noexcept; // terminates
#endif // _HAS_CXX17

_EXPORT_STD template <class _FwdIt, class _Diff, class _Ty>
_NODISCARD _CONSTEXPR20 _FwdIt search_n(const _FwdIt _First, const _FwdIt _Last, const _Diff _Count, const _Ty& _Val) {
    // find first _Count * _Val match
    return _STD search_n(_First, _Last, _Count, _Val, equal_to<>{});
}

#if _HAS_CXX17
_EXPORT_STD template <class _ExPo, class _FwdIt, class _Diff, class _Ty, _Enable_if_execution_policy_t<_ExPo> = 0>
_NODISCARD _FwdIt search_n(_ExPo&& _Exec, const _FwdIt _First, const _FwdIt _Last, const _Diff _Count,
    const _Ty& _Val) noexcept /* terminates */ { // find first _Count * _Val match
    return _STD search_n(_STD forward<_ExPo>(_Exec), _First, _Last, _Count, _Val, equal_to{});
}
#endif // _HAS_CXX17

#ifdef __cpp_lib_concepts
namespace ranges {
    class _Search_n_fn : private _Not_quite_object {
    public:
        using _Not_quite_object::_Not_quite_object;

        template <forward_iterator _It, sentinel_for<_It> _Se, class _Ty, class _Pr = ranges::equal_to,
            class _Pj = identity>
            requires indirectly_comparable<_It, const _Ty*, _Pr, _Pj>
        _NODISCARD constexpr subrange<_It> operator()(_It _First, _Se _Last, const iter_difference_t<_It> _Count,
            const _Ty& _Val, _Pr _Pred = {}, _Pj _Proj = {}) const {
            _Adl_verify_range(_First, _Last);

            if (_Count <= 0) {
                return { _First, _First };
            }

            auto _UFirst = _Unwrap_iter<_Se>(_STD move(_First));
            auto _ULast = _Unwrap_sent<_It>(_STD move(_Last));

            if constexpr (sized_sentinel_for<_Se, _It>) {
                const auto _Dist = _ULast - _UFirst;
                auto _UResult =
                    _Search_n_sized(_STD move(_UFirst), _Dist, _Val, _Count, _Pass_fn(_Pred), _Pass_fn(_Proj));
                return _Rewrap_subrange<subrange<_It>>(_First, _STD move(_UResult));
            }
            else {
                auto _UResult = _Search_n_unsized(
                    _STD move(_UFirst), _STD move(_ULast), _Val, _Count, _Pass_fn(_Pred), _Pass_fn(_Proj));
                return _Rewrap_subrange<subrange<_It>>(_First, _STD move(_UResult));
            }
        }

        template <forward_range _Rng, class _Ty, class _Pr = ranges::equal_to, class _Pj = identity>
            requires indirectly_comparable<iterator_t<_Rng>, const _Ty*, _Pr, _Pj>
        _NODISCARD constexpr borrowed_subrange_t<_Rng> operator()(_Rng&& _Range, const range_difference_t<_Rng> _Count,
            const _Ty& _Val, _Pr _Pred = {}, _Pj _Proj = {}) const {
            auto _First = _RANGES begin(_Range);

            if (_Count <= 0) {
                return { _First, _First };
            }

            if constexpr (sized_range<_Rng>) {
                const auto _Dist = _RANGES distance(_Range);

                auto _UResult =
                    _Search_n_sized(_Get_unwrapped(_First), _Dist, _Val, _Count, _Pass_fn(_Pred), _Pass_fn(_Proj));
                return _Rewrap_subrange<borrowed_subrange_t<_Rng>>(_First, _STD move(_UResult));
            }
            else {
                auto _UResult = _Search_n_unsized(
                    _Unwrap_range_iter<_Rng>(_First), _Uend(_Range), _Val, _Count, _Pass_fn(_Pred), _Pass_fn(_Proj));
                return _Rewrap_subrange<borrowed_subrange_t<_Rng>>(_First, _STD move(_UResult));
            }
        }

    private:
        template <class _It, class _Ty, class _Pr, class _Pj>
        _NODISCARD static constexpr subrange<_It> _Search_n_sized(_It _First, iter_difference_t<_It> _Dist,
            const _Ty& _Val, const iter_difference_t<_It> _Count, _Pr _Pred, _Pj _Proj) {
            _STL_INTERNAL_STATIC_ASSERT(forward_iterator<_It>);
            _STL_INTERNAL_STATIC_ASSERT(indirectly_comparable<_It, const _Ty*, _Pr, _Pj>);
            _STL_INTERNAL_CHECK(_Count > 0);
            // pre: _First + [0, _Dist) is a valid counted range

            if constexpr (bidirectional_iterator<_It>) {
                if (_Dist < _Count) {
                    _RANGES advance(_First, _Dist);
                    return { _First, _First };
                }

                auto _Last = _RANGES next(_First, _Count);
                auto _Mid1 = _First;
                auto _Mid2 = _Last;
                for (;;) {
                    // Invariants: _Last - _First == _Count, [_First, _Mid1) and [_Mid2, _Last) match _Val:
                    //
                    // _First  _Mid1    _Mid2    _Last
                    // |=======|????????|========|??????...

                    --_Mid2;
                    if (!_STD invoke(_Pred, _STD invoke(_Proj, *_Mid2), _Val)) { // mismatch; skip past it
                        ++_Mid2;
                        const auto _Delta = _RANGES distance(_First, _Mid2);

                        if (_Dist - _Delta < _Count) { // not enough space left
                            _First = _STD move(_Last);
                            _Dist -= _Count;
                            break;
                        }

                        _First = _STD move(_Mid2);
                        _Dist -= _Delta;
                        _Mid1 = _Last;
                        _RANGES advance(_Last, _Delta);
                        _Mid2 = _Last;
                        continue;
                    }

                    if (_Mid2 == _Mid1) { // [_Mid1, _Mid2) is empty, so [_First, _Last) all match
                        return { _STD move(_First), _STD move(_Last) };
                    }
                }
            }
            else {
                for (; _Dist >= _Count; ++_First, (void) --_Dist) {
                    if (_STD invoke(_Pred, _STD invoke(_Proj, *_First), _Val)) {
                        auto _Saved = _First;
                        for (iter_difference_t<_It> _Len = 0;;) {
                            ++_First;
                            if (++_Len == _Count) { // match
                                return { _STD move(_Saved), _STD move(_First) };
                            }

                            if (!_STD invoke(_Pred, _STD invoke(_Proj, *_First), _Val)) { // mismatch
                                _Dist -= _Len;
                                break;
                            }
                        }
                    }
                }
            }

            _RANGES advance(_First, _Dist);
            return { _First, _First };
        }

        template <class _It, class _Se, class _Ty, class _Pr, class _Pj>
        _NODISCARD static constexpr subrange<_It> _Search_n_unsized(
            _It _First, const _Se _Last, const _Ty& _Val, const iter_difference_t<_It> _Count, _Pr _Pred, _Pj _Proj) {
            _STL_INTERNAL_STATIC_ASSERT(forward_iterator<_It>);
            _STL_INTERNAL_STATIC_ASSERT(sentinel_for<_Se, _It>);
            _STL_INTERNAL_STATIC_ASSERT(indirectly_comparable<_It, const _Ty*, _Pr, _Pj>);
            _STL_INTERNAL_CHECK(_Count > 0);

            for (; _First != _Last; ++_First) {
                if (_STD invoke(_Pred, _STD invoke(_Proj, *_First), _Val)) {
                    auto _Saved = _First;
                    for (auto _Len = _Count;;) {
                        ++_First;
                        if (--_Len == 0) { // match
                            return { _STD move(_Saved), _STD move(_First) };
                        }

                        if (_First == _Last) { // no more to match against
                            return { _First, _First };
                        }

                        if (!_STD invoke(_Pred, _STD invoke(_Proj, *_First), _Val)) { // mismatch
                            break;
                        }
                    }
                }
            }

            return { _First, _First };
        }
    };

    _EXPORT_STD inline constexpr _Search_n_fn search_n{ _Not_quite_object::_Construct_tag{} };

#if _HAS_CXX23
    class _Starts_with_fn : private _Not_quite_object {
    public:
        using _Not_quite_object::_Not_quite_object;

        template <input_iterator _It1, sentinel_for<_It1> _Se1, input_iterator _It2, sentinel_for<_It2> _Se2,
            class _Pr = ranges::equal_to, class _Pj1 = identity, class _Pj2 = identity>
            requires indirectly_comparable<_It1, _It2, _Pr, _Pj1, _Pj2>
        _NODISCARD constexpr bool operator()(_It1 _First1, _Se1 _Last1, _It2 _First2, _Se2 _Last2, _Pr _Pred = {},
            _Pj1 _Proj1 = {}, _Pj2 _Proj2 = {}) const {
            _Adl_verify_range(_First1, _Last1);
            _Adl_verify_range(_First2, _Last2);

            auto _UFirst1 = _Unwrap_iter<_Se1>(_STD move(_First1));
            auto _ULast1 = _Unwrap_sent<_It1>(_STD move(_Last1));
            auto _UFirst2 = _Unwrap_iter<_Se2>(_STD move(_First2));
            const auto _ULast2 = _Unwrap_sent<_It2>(_STD move(_Last2));
            if constexpr (_Sized_or_unreachable_sentinel_for<_Se1, _It1> && sized_sentinel_for<_Se2, _It2>) {
                const iter_difference_t<_It2> _Count2 = _ULast2 - _UFirst2;
                if constexpr (sized_sentinel_for<_Se1, _It1>) {
                    if (_Count2 > _ULast1 - _UFirst1) {
                        return false;
                    }
                }

                return _RANGES _Equal_count(_STD move(_UFirst1), _STD move(_UFirst2),
                    static_cast<iter_difference_t<_It1>>(_Count2), _Pass_fn(_Pred), _Pass_fn(_Proj1), _Pass_fn(_Proj2));
            }
            else {
                const auto _Result = _RANGES _Mismatch_4(_STD move(_UFirst1), _STD move(_ULast1), _STD move(_UFirst2),
                    _ULast2, _Pass_fn(_Pred), _Pass_fn(_Proj1), _Pass_fn(_Proj2));
                return _Result.in2 == _ULast2;
            }
        }

        template <input_range _Rng1, input_range _Rng2, class _Pr = ranges::equal_to, class _Pj1 = identity,
            class _Pj2 = identity>
            requires indirectly_comparable<iterator_t<_Rng1>, iterator_t<_Rng2>, _Pr, _Pj1, _Pj2>
        _NODISCARD constexpr bool operator()(
            _Rng1&& _Range1, _Rng2&& _Range2, _Pr _Pred = {}, _Pj1 _Proj1 = {}, _Pj2 _Proj2 = {}) const {
            if constexpr (_Sized_or_infinite_range<_Rng1> && sized_range<_Rng2>) {
                const range_difference_t<_Rng2> _Count2 = _RANGES distance(_Range2);
                if constexpr (sized_range<_Rng1>) {
                    if (_Count2 > _RANGES distance(_Range1)) {
                        return false;
                    }
                }

                return _RANGES _Equal_count(_Ubegin(_Range1), _Ubegin(_Range2),
                    static_cast<range_difference_t<_Rng1>>(_Count2), _Pass_fn(_Pred), _Pass_fn(_Proj1),
                    _Pass_fn(_Proj2));
            }
            else {
                const auto _ULast2 = _Uend(_Range2);
                const auto _Result = _RANGES _Mismatch_4(_Ubegin(_Range1), _Uend(_Range1), _Ubegin(_Range2), _ULast2,
                    _Pass_fn(_Pred), _Pass_fn(_Proj1), _Pass_fn(_Proj2));
                return _Result.in2 == _ULast2;
            }
        }
    };

    _EXPORT_STD inline constexpr _Starts_with_fn starts_with{ _Not_quite_object::_Construct_tag{} };

    class _Ends_with_fn : private _Not_quite_object {
    public:
        using _Not_quite_object::_Not_quite_object;

        template <input_iterator _It1, sentinel_for<_It1> _Se1, input_iterator _It2, sentinel_for<_It2> _Se2,
            class _Pr = ranges::equal_to, class _Pj1 = identity, class _Pj2 = identity>
            requires (forward_iterator<_It1> || sized_sentinel_for<_Se1, _It1>)
        && (forward_iterator<_It2> || sized_sentinel_for<_Se2, _It2>)
            && indirectly_comparable<_It1, _It2, _Pr, _Pj1, _Pj2>
            _NODISCARD constexpr bool operator()(_It1 _First1, _Se1 _Last1, _It2 _First2, _Se2 _Last2, _Pr _Pred = {},
                _Pj1 _Proj1 = {}, _Pj2 _Proj2 = {}) const {
            _Adl_verify_range(_First1, _Last1);
            _Adl_verify_range(_First2, _Last2);
            auto _UFirst1 = _Unwrap_iter<_Se1>(_STD move(_First1));
            auto _ULast1 = _Unwrap_sent<_It1>(_STD move(_Last1));
            auto _UFirst2 = _Unwrap_iter<_Se2>(_STD move(_First2));
            auto _ULast2 = _Unwrap_sent<_It2>(_STD move(_Last2));

            const auto _Count1 = _Distance_helper(_UFirst1, _ULast1);
            const auto _Count2 = _Distance_helper(_UFirst2, _ULast2);

            return _Ends_with_impl(_STD move(_UFirst1), _STD move(_ULast1), _Count1, _STD move(_UFirst2),
                _STD move(_ULast2), _Count2, _Pass_fn(_Pred), _Pass_fn(_Proj1), _Pass_fn(_Proj2));
        }

        template <input_range _Rng1, input_range _Rng2, class _Pr = ranges::equal_to, class _Pj1 = identity,
            class _Pj2 = identity>
            requires (forward_range<_Rng1> || sized_range<_Rng1>) && (forward_range<_Rng2> || sized_range<_Rng2>)
        && indirectly_comparable<iterator_t<_Rng1>, iterator_t<_Rng2>, _Pr, _Pj1, _Pj2>
            _NODISCARD constexpr bool operator()(
                _Rng1&& _Range1, _Rng2&& _Range2, _Pr _Pred = {}, _Pj1 _Proj1 = {}, _Pj2 _Proj2 = {}) const {
            const auto _Count1 = _Distance_helper(_Range1);
            const auto _Count2 = _Distance_helper(_Range2);

            return _Ends_with_impl(_Ubegin(_Range1), _Uend(_Range1), _Count1, _Ubegin(_Range2), _Uend(_Range2), _Count2,
                _Pass_fn(_Pred), _Pass_fn(_Proj1), _Pass_fn(_Proj2));
        }

    private:
        struct _Not_a_difference {};

        template <class _It, class _Se>
        _NODISCARD static constexpr auto _Distance_helper(const _It& _First, const _Se& _Last) {
            _STL_INTERNAL_STATIC_ASSERT(sentinel_for<_Se, _It>);
            if constexpr (sized_sentinel_for<_Se, _It>) {
                return _Last - _First;
            }
            else {
                return _Not_a_difference{};
            }
        }

        template <class _Rng>
        _NODISCARD static constexpr auto _Distance_helper(_Rng&& _Range) {
            _STL_INTERNAL_STATIC_ASSERT(range<_Rng>);
            if constexpr (sized_range<_Rng>) {
                return _RANGES distance(_Range);
            }
            else {
                return _Not_a_difference{};
            }
        }

        template <class _Ty, class _It1, class _It2, class _Pr, class _Pj1, class _Pj2>
        _NODISCARD static constexpr bool _Match_backwards(
            const _Ty _First1, _It1 _Last1, const _It2 _First2, _It2 _Last2, _Pr _Pred, _Pj1 _Proj1, _Pj2 _Proj2) {
            _STL_INTERNAL_STATIC_ASSERT(same_as<_Ty, _It1> || same_as<_Ty, unreachable_sentinel_t>);
            _STL_INTERNAL_STATIC_ASSERT(bidirectional_iterator<_It1>);
            _STL_INTERNAL_STATIC_ASSERT(bidirectional_iterator<_It2>);
            _STL_INTERNAL_STATIC_ASSERT(indirectly_comparable<_It1, _It2, _Pr, _Pj1, _Pj2>);

            for (;;) {
                if (_First2 == _Last2) { // needle exhausted - match
                    return true;
                }

                if (_First1 == _Last1) { // haystack exhausted - no match
                    return false;
                }

                --_Last1;
                --_Last2;
                if (!_STD invoke(_Pred, _STD invoke(_Proj1, *_Last1), _STD invoke(_Proj2, *_Last2))) {
                    return false; // non-equal elements - no match
                }
            }
        }

        template <class _It1, class _Se1, class _Diff1, class _It2, class _Pr, class _Pj1, class _Pj2>
        _NODISCARD static constexpr bool _Ends_with_sized_needle(_It1 _First1, _Se1 _Last1, _Diff1 _Count1,
            _It2 _First2, iter_difference_t<_It2> _Count2, _Pr _Pred, _Pj1 _Proj1, _Pj2 _Proj2) {
            _STL_INTERNAL_STATIC_ASSERT(input_iterator<_It1> && sentinel_for<_Se1, _It1>);
            _STL_INTERNAL_STATIC_ASSERT(forward_iterator<_It1> || same_as<_Diff1, iter_difference_t<_It1>>);
            _STL_INTERNAL_STATIC_ASSERT(input_iterator<_It2>);
            _STL_INTERNAL_STATIC_ASSERT(indirectly_comparable<_It1, _It2, _Pr, _Pj1, _Pj2>);

            constexpr bool _Sized1 = !same_as<_Diff1, _Not_a_difference>;

            if constexpr (random_access_iterator<_It1> && _Sized1) {
                _First1 += static_cast<iter_difference_t<_It1>>(_Count1 - _Count2);
            }
            else if constexpr (_Bidi_common<_It1, _Se1>) {
                if constexpr (_Sized1) {
                    if ((_Count1 >> 1) >= _Count2) { // beginning of potential match is closer to _Last1
                        _RANGES advance(_Last1, static_cast<iter_difference_t<_It1>>(-_Count2));
                        _First1 = _STD move(_Last1);
                    }
                    else { // beginning of potential match is closer to _First1
                        _RANGES advance(_First1, static_cast<iter_difference_t<_It1>>(_Count1 - _Count2));
                    }
                }
                else { // Only second range is sized
                    if (_RANGES advance(_Last1, static_cast<iter_difference_t<_It1>>(-_Count2), _First1) != 0) {
                        // distance(_First1, _Last1) < _Count2
                        return false;
                    }
                    _First1 = _STD move(_Last1);
                }
            }
            else if constexpr (forward_iterator<_It1>) {
                auto _Mid1 = _First1;
                auto _Count = _Count2;
                do {
                    if (_Mid1 == _Last1) { // distance(_First1, _Last1) < _Count2
                        return false;
                    }

                    ++_Mid1;
                } while (--_Count != 0);

                // At this point, distance(_First1, _Mid1) == _Count2
                while (_Mid1 != _Last1) {
                    ++_First1;
                    ++_Mid1;
                }
            }
            else {
                _RANGES advance(_First1, static_cast<iter_difference_t<_It1>>(_Count1 - _Count2));
            }

            return _RANGES _Equal_count(_STD move(_First1), _STD move(_First2),
                static_cast<iter_difference_t<_It1>>(_Count2), _Pred, _Proj1, _Proj2);
        }

        template <class _It1, class _Se1, class _Diff1, class _It2, class _Se2, class _Pr, class _Pj1, class _Pj2>
        _NODISCARD static constexpr bool _Ends_with_unsized_needle(
            _It1 _First1, _Se1 _Last1, _Diff1 _Count1, _It2 _First2, _Se2 _Last2, _Pr _Pred, _Pj1 _Proj1, _Pj2 _Proj2) {
            _STL_INTERNAL_STATIC_ASSERT(input_iterator<_It1> && sentinel_for<_Se1, _It1>);
            _STL_INTERNAL_STATIC_ASSERT(forward_iterator<_It1> || same_as<_Diff1, iter_difference_t<_It1>>);
            _STL_INTERNAL_STATIC_ASSERT(forward_iterator<_It2> && sentinel_for<_Se2, _It2>);
            _STL_INTERNAL_STATIC_ASSERT(indirectly_comparable<_It1, _It2, _Pr, _Pj1, _Pj2>);

            constexpr bool _Sized1 = !same_as<_Diff1, _Not_a_difference>;

            iter_difference_t<_It2> _Count2 = 0;

            if constexpr (_Sized1) {
                for (auto _Mid2 = _First2; _Mid2 != _Last2; ++_Mid2, (void) ++_Count2) {
                    if (_Count2 == _Count1) { // distance(_First1, _Last1) < distance(_First2, _Last2)
                        return false;
                    }
                }

                _RANGES advance(_First1, static_cast<iter_difference_t<_It1>>(_Count1 - _Count2));
            }
            else { // first range isn't sized, so must be forward
                auto _Mid1 = _First1;
                for (auto _Mid2 = _First2; _Mid2 != _Last2; ++_Mid1, (void) ++_Mid2, ++_Count2) {
                    if (_Mid1 == _Last1) { // distance(_First1, _Last1) < distance(_First2, _Last2)
                        return false;
                    }
                }

                // distance(_First1, _Mid1) == distance(_First2, _Last2) == _Count2
                while (_Mid1 != _Last1) {
                    ++_First1;
                    ++_Mid1;
                }
            }

            return _RANGES _Equal_count(_STD move(_First1), _STD move(_First2),
                static_cast<iter_difference_t<_It1>>(_Count2), _Pred, _Proj1, _Proj2);
        }

        template <class _It1, class _Se1, class _Diff1, class _It2, class _Se2, class _Diff2, class _Pr, class _Pj1,
            class _Pj2>
        _NODISCARD static constexpr bool _Ends_with_impl(_It1 _First1, _Se1 _Last1, _Diff1 _Count1, _It2 _First2,
            _Se2 _Last2, _Diff2 _Count2, _Pr _Pred, _Pj1 _Proj1, _Pj2 _Proj2) {
            _STL_INTERNAL_STATIC_ASSERT(sentinel_for<_Se1, _It1>);
            _STL_INTERNAL_STATIC_ASSERT(_Is_any_of_v<_Diff1, iter_difference_t<_It1>, _Not_a_difference>);
            _STL_INTERNAL_STATIC_ASSERT(sentinel_for<_Se2, _It2>);
            _STL_INTERNAL_STATIC_ASSERT(_Is_any_of_v<_Diff2, iter_difference_t<_It2>, _Not_a_difference>);
            _STL_INTERNAL_STATIC_ASSERT(indirectly_comparable<_It1, _It2, _Pr, _Pj1, _Pj2>);

            constexpr bool _Sized1 = !same_as<_Diff1, _Not_a_difference>;
            constexpr bool _Sized2 = !same_as<_Diff2, _Not_a_difference>;
            constexpr bool _Both_sized = _Sized1 && _Sized2;
            constexpr bool _Both_bidi_common = _Bidi_common<_It1, _Se1>&& _Bidi_common<_It2, _Se2>;

            if constexpr (_Both_sized) {
                if (_Count2 > _Count1) {
                    return false;
                }
            }

            if constexpr (_Sized2) {
                if (_Count2 == 0) {
                    return true;
                }
            }

            if constexpr (_Both_bidi_common && !(random_access_iterator<_It1> && _Both_sized)) {
                if constexpr (_Both_sized) {
                    return _Match_backwards(unreachable_sentinel, _STD move(_Last1), _STD move(_First2),
                        _STD move(_Last2), _Pred, _Proj1, _Proj2);
                }
                else {
                    return _Match_backwards(_STD move(_First1), _STD move(_Last1), _STD move(_First2),
                        _STD move(_Last2), _Pred, _Proj1, _Proj2);
                }
            }
            else if constexpr (_Sized2) {
                return _Ends_with_sized_needle(
                    _STD move(_First1), _STD move(_Last1), _Count1, _STD move(_First2), _Count2, _Pred, _Proj1, _Proj2);
            }
            else {
                return _Ends_with_unsized_needle(_STD move(_First1), _STD move(_Last1), _Count1, _STD move(_First2),
                    _STD move(_Last2), _Pred, _Proj1, _Proj2);
            }
        }
    };

    _EXPORT_STD inline constexpr _Ends_with_fn ends_with{ _Not_quite_object::_Construct_tag{} };

    template <class _Fn>
    class _Flipped {
    private:
        _Fn _Func;

    public:
        template <class _Ty, class _Uty>
            requires invocable<_Fn&, _Uty, _Ty>
        invoke_result_t<_Fn&, _Uty, _Ty> operator()(_Ty&&, _Uty&&);
    };

    template <class _Fn, class _Ty, class _It, class _Uty>
    concept _Indirectly_binary_left_foldable_impl =
        movable<_Ty> && movable<_Uty> && convertible_to<_Ty, _Uty>&& invocable<_Fn&, _Uty, iter_reference_t<_It>>
        && assignable_from<_Uty&, invoke_result_t<_Fn&, _Uty, iter_reference_t<_It>>>;

    template <class _Fn, class _Ty, class _It>
    concept _Indirectly_binary_left_foldable =
        copy_constructible<_Fn> && indirectly_readable<_It> && invocable<_Fn&, _Ty, iter_reference_t<_It>>
        && convertible_to<invoke_result_t<_Fn&, _Ty, iter_reference_t<_It>>,
        decay_t<invoke_result_t<_Fn&, _Ty, iter_reference_t<_It>>>>
        && _Indirectly_binary_left_foldable_impl<_Fn, _Ty, _It,
        decay_t<invoke_result_t<_Fn&, _Ty, iter_reference_t<_It>>>>;

    template <class _Fn, class _Ty, class _It>
    concept _Indirectly_binary_right_foldable = _Indirectly_binary_left_foldable<_Flipped<_Fn>, _Ty, _It>;

    _EXPORT_STD template <class _It, class _Ty>
        using fold_left_with_iter_result = in_value_result<_It, _Ty>;

    _EXPORT_STD template <class _It, class _Ty>
        using fold_left_first_with_iter_result = in_value_result<_It, _Ty>;

    class _Fold_left_with_iter_fn : private _Not_quite_object {
    public:
        using _Not_quite_object::_Not_quite_object;

        template <input_iterator _It, sentinel_for<_It> _Se, class _Ty, _Indirectly_binary_left_foldable<_Ty, _It> _Fn>
        _NODISCARD constexpr auto operator()(_It _First, _Se _Last, _Ty _Init, _Fn _Func) const {
            _Adl_verify_range(_First, _Last);
            return _Fold_left_with_iter_impl<_It>(
                _STD move(_First), _STD move(_Last), _STD move(_Init), _Pass_fn(_Func));
        }

        template <input_range _Rng, class _Ty, _Indirectly_binary_left_foldable<_Ty, iterator_t<_Rng>> _Fn>
        _NODISCARD constexpr auto operator()(_Rng&& _Range, _Ty _Init, _Fn _Func) const {
            return _Fold_left_with_iter_impl<borrowed_iterator_t<_Rng>>(
                _RANGES begin(_Range), _RANGES end(_Range), _STD move(_Init), _Pass_fn(_Func));
        }

    private:
        template <class _RetIt, class _It, class _Se, class _Ty, class _Fn>
        _NODISCARD constexpr auto _Fold_left_with_iter_impl(_It&& _First, _Se&& _Last, _Ty&& _Init, _Fn _Func) const {
            _STL_INTERNAL_STATIC_ASSERT(input_iterator<_It>);
            _STL_INTERNAL_STATIC_ASSERT(sentinel_for<_Se, _It>);
            _STL_INTERNAL_STATIC_ASSERT(_Indirectly_binary_left_foldable<_Fn, _Ty, _It>);

            using _Uty = decay_t<invoke_result_t<_Fn&, _Ty, iter_reference_t<_It>>>;
            using _Return_type = fold_left_with_iter_result<_RetIt, _Uty>;

            if (_First == _Last) {
                return _Return_type{ _STD move(_First), static_cast<_Uty>(_STD move(_Init)) };
            }
            else {
                auto _UFirst = _Unwrap_iter<_Se>(_STD move(_First));
                auto _ULast = _Unwrap_sent<_It>(_STD move(_Last));

                _Uty _Accum = _STD invoke(_Func, _STD move(_Init), *_UFirst);
                for (++_UFirst; _UFirst != _ULast; ++_UFirst) {
                    _Accum = _STD invoke(_Func, _STD move(_Accum), *_UFirst);
                }

                _Seek_wrapped(_First, _STD move(_UFirst));
                return _Return_type{ _STD move(_First), _STD move(_Accum) };
            }
        }
    };

    _EXPORT_STD inline constexpr _Fold_left_with_iter_fn fold_left_with_iter{ _Not_quite_object::_Construct_tag{} };

    class _Fold_left_fn : private _Not_quite_object {
    public:
        using _Not_quite_object::_Not_quite_object;

        template <input_iterator _It, sentinel_for<_It> _Se, class _Ty, _Indirectly_binary_left_foldable<_Ty, _It> _Fn>
        _NODISCARD constexpr auto operator()(_It _First, _Se _Last, _Ty _Init, _Fn _Func) const {
            return _RANGES fold_left_with_iter(_STD move(_First), _Last, _STD move(_Init), _Pass_fn(_Func)).value;
        }

        template <input_range _Rng, class _Ty, _Indirectly_binary_left_foldable<_Ty, iterator_t<_Rng>> _Fn>
        _NODISCARD constexpr auto operator()(_Rng&& _Range, _Ty _Init, _Fn _Func) const {
            return _RANGES fold_left_with_iter(_STD forward<_Rng>(_Range), _STD move(_Init), _Pass_fn(_Func)).value;
        }
    };

    _EXPORT_STD inline constexpr _Fold_left_fn fold_left{ _Not_quite_object::_Construct_tag{} };

    class _Fold_left_first_with_iter_fn : private _Not_quite_object {
    public:
        using _Not_quite_object::_Not_quite_object;

        template <input_iterator _It, sentinel_for<_It> _Se,
            _Indirectly_binary_left_foldable<iter_value_t<_It>, _It> _Fn>
            requires constructible_from<iter_value_t<_It>, iter_reference_t<_It>>
        _NODISCARD constexpr auto operator()(_It _First, _Se _Last, _Fn _Func) const {
            _Adl_verify_range(_First, _Last);
            return _Fold_left_first_with_iter_impl<_It>(_STD move(_First), _STD move(_Last), _Pass_fn(_Func));
        }

        template <input_range _Rng, _Indirectly_binary_left_foldable<range_value_t<_Rng>, iterator_t<_Rng>> _Fn>
            requires constructible_from<range_value_t<_Rng>, range_reference_t<_Rng>>
        _NODISCARD constexpr auto operator()(_Rng&& _Range, _Fn _Func) const {
            return _Fold_left_first_with_iter_impl<borrowed_iterator_t<_Rng>>(
                _RANGES begin(_Range), _RANGES end(_Range), _Pass_fn(_Func));
        }

    private:
        template <class _RetIt, class _It, class _Se, class _Fn>
        _NODISCARD constexpr auto _Fold_left_first_with_iter_impl(_It&& _First, _Se&& _Last, _Fn _Func) const {
            _STL_INTERNAL_STATIC_ASSERT(input_iterator<_It>);
            _STL_INTERNAL_STATIC_ASSERT(sentinel_for<_Se, _It>);
            _STL_INTERNAL_STATIC_ASSERT(_Indirectly_binary_left_foldable<_Fn, iter_value_t<_It>, _It>);
            _STL_INTERNAL_STATIC_ASSERT(constructible_from<iter_value_t<_It>, iter_reference_t<_It>>);

            using _Uty =
                decltype(_RANGES fold_left(_STD move(_First), _Last, static_cast<iter_value_t<_It>>(*_First), _Func));
            using _Return_type = fold_left_first_with_iter_result<_RetIt, optional<_Uty>>;
            if (_First == _Last) {
                return _Return_type{ _STD move(_First), optional<_Uty>{} };
            }
            else {
                auto _UFirst = _Unwrap_iter<_Se>(_STD move(_First));
                auto _ULast = _Unwrap_sent<_It>(_STD move(_Last));

                optional<_Uty> _Init{ in_place, *_UFirst };
                _Uty& _Init_ref = *_Init;
                for (++_UFirst; _UFirst != _ULast; ++_UFirst) {
                    _Init_ref = _STD invoke(_Func, _STD move(_Init_ref), *_UFirst);
                }

                _Seek_wrapped(_First, _STD move(_UFirst));
                return _Return_type{ _STD move(_First), _STD move(_Init) };
            }
        }
    };

    _EXPORT_STD inline constexpr _Fold_left_first_with_iter_fn fold_left_first_with_iter{
        _Not_quite_object::_Construct_tag{} };

    class _Fold_left_first_fn : private _Not_quite_object {
    public:
        using _Not_quite_object::_Not_quite_object;

        template <input_iterator _It, sentinel_for<_It> _Se,
            _Indirectly_binary_left_foldable<iter_value_t<_It>, _It> _Fn>
            requires constructible_from<iter_value_t<_It>, iter_reference_t<_It>>
        _NODISCARD constexpr auto operator()(_It _First, _Se _Last, _Fn _Func) const {
            return _RANGES fold_left_first_with_iter(_STD move(_First), _STD move(_Last), _Pass_fn(_Func)).value;
        }

        template <input_range _Rng, _Indirectly_binary_left_foldable<range_value_t<_Rng>, iterator_t<_Rng>> _Fn>
            requires constructible_from<range_value_t<_Rng>, range_reference_t<_Rng>>
        _NODISCARD constexpr auto operator()(_Rng&& _Range, _Fn _Func) const {
            return _RANGES fold_left_first_with_iter(_STD forward<_Rng>(_Range), _Pass_fn(_Func)).value;
        }
    };

    _EXPORT_STD inline constexpr _Fold_left_first_fn fold_left_first{ _Not_quite_object::_Construct_tag{} };

    template <class _It, class _Se, class _Ty, class _Fn>
    _NODISCARD constexpr auto _Fold_right_unchecked(_It _First, _Se _Last, _Ty _Init, _Fn _Func) {
        _STL_INTERNAL_STATIC_ASSERT(bidirectional_iterator<_It>);
        _STL_INTERNAL_STATIC_ASSERT(sentinel_for<_Se, _It>);
        _STL_INTERNAL_STATIC_ASSERT(_Indirectly_binary_right_foldable<_Fn, _Ty, _It>);

        using _Uty = decay_t<invoke_result_t<_Fn&, iter_reference_t<_It>, _Ty>>;
        if (_First == _Last) {
            return static_cast<_Uty>(_STD move(_Init));
        }
        else {
            _It _Tail = _RANGES next(_First, _Last);
            _Uty _Accum = _STD invoke(_Func, *--_Tail, _STD move(_Init));
            while (_First != _Tail) {
                _Accum = _STD invoke(_Func, *--_Tail, _STD move(_Accum));
            }
            return _Accum;
        }
    }

    class _Fold_right_fn : private _Not_quite_object {
    public:
        using _Not_quite_object::_Not_quite_object;

        template <bidirectional_iterator _It, sentinel_for<_It> _Se, class _Ty,
            _Indirectly_binary_right_foldable<_Ty, _It> _Fn>
        _NODISCARD constexpr auto operator()(_It _First, _Se _Last, _Ty _Init, _Fn _Func) const {
            _Adl_verify_range(_First, _Last);
            return _RANGES _Fold_right_unchecked(_Unwrap_iter<_Se>(_STD move(_First)),
                _Unwrap_sent<_It>(_STD move(_Last)), _STD move(_Init), _Pass_fn(_Func));
        }

        template <bidirectional_range _Rng, class _Ty, _Indirectly_binary_right_foldable<_Ty, iterator_t<_Rng>> _Fn>
        _NODISCARD constexpr auto operator()(_Rng&& _Range, _Ty _Init, _Fn _Func) const {
            return _RANGES _Fold_right_unchecked(_Ubegin(_Range), _Uend(_Range), _STD move(_Init), _Pass_fn(_Func));
        }
    };

    _EXPORT_STD inline constexpr _Fold_right_fn fold_right{ _Not_quite_object::_Construct_tag{} };

    class _Fold_right_last_fn : private _Not_quite_object {
    public:
        using _Not_quite_object::_Not_quite_object;

        template <bidirectional_iterator _It, sentinel_for<_It> _Se,
            _Indirectly_binary_right_foldable<iter_value_t<_It>, _It> _Fn>
            requires constructible_from<iter_value_t<_It>, iter_reference_t<_It>>
        _NODISCARD constexpr auto operator()(_It _First, _Se _Last, _Fn _Func) const {
            _Adl_verify_range(_First, _Last);
            return _Fold_right_last_unchecked(
                _Unwrap_iter<_Se>(_STD move(_First)), _Unwrap_sent<_It>(_STD move(_Last)), _Pass_fn(_Func));
        }

        template <bidirectional_range _Rng,
            _Indirectly_binary_right_foldable<range_value_t<_Rng>, iterator_t<_Rng>> _Fn>
            requires constructible_from<range_value_t<_Rng>, range_reference_t<_Rng>>
        _NODISCARD constexpr auto operator()(_Rng&& _Range, _Fn _Func) const {
            return _Fold_right_last_unchecked(_Ubegin(_Range), _Uend(_Range), _Pass_fn(_Func));
        }

    private:
        template <class _It, class _Se, class _Fn>
        _NODISCARD constexpr auto _Fold_right_last_unchecked(_It _First, _Se _Last, _Fn _Func) const {
            _STL_INTERNAL_STATIC_ASSERT(bidirectional_iterator<_It>);
            _STL_INTERNAL_STATIC_ASSERT(sentinel_for<_Se, _It>);
            _STL_INTERNAL_STATIC_ASSERT(_Indirectly_binary_right_foldable<_Fn, iter_value_t<_It>, _It>);
            _STL_INTERNAL_STATIC_ASSERT(constructible_from<iter_value_t<_It>, iter_reference_t<_It>>);

            using _Uty = decltype(_RANGES fold_right(_First, _Last, static_cast<iter_value_t<_It>>(*_First), _Func));
            if (_First == _Last) {
                return optional<_Uty>{};
            }
            else {
                _It _Tail = _RANGES prev(_RANGES next(_First, _STD move(_Last)));
                return optional<_Uty>{in_place, _RANGES _Fold_right_unchecked(_STD move(_First), _Tail,
                    static_cast<iter_value_t<_It>>(*_Tail), _STD move(_Func))};
            }
        }
    };

    _EXPORT_STD inline constexpr _Fold_right_last_fn fold_right_last{ _Not_quite_object::_Construct_tag{} };

    class _Find_last_fn : private _Not_quite_object {
    public:
        using _Not_quite_object::_Not_quite_object;

        template <forward_iterator _It, sentinel_for<_It> _Se, class _Ty, class _Pj = identity>
            requires indirect_binary_predicate<ranges::equal_to, projected<_It, _Pj>, const _Ty*>
        _NODISCARD constexpr subrange<_It> operator()(_It _First, _Se _Last, const _Ty& _Value, _Pj _Proj = {}) const {
            _Adl_verify_range(_First, _Last);

            auto _UFirst = _Unwrap_iter<_Se>(_STD move(_First));
            if constexpr (bidirectional_iterator<_It>) {
                auto _ULast = _Get_final_iterator_unwrapped<_It>(_UFirst, _STD move(_Last));
                auto _UResult = _Find_last_unchecked(_STD move(_UFirst), _STD move(_ULast), _Value, _Pass_fn(_Proj));
                return _Rewrap_subrange<subrange<_It>>(_First, _STD move(_UResult));
            }
            else {
                auto _ULast = _Unwrap_sent<_It>(_STD move(_Last));
                auto _UResult = _Find_last_unchecked(_STD move(_UFirst), _STD move(_ULast), _Value, _Pass_fn(_Proj));
                return _Rewrap_subrange<subrange<_It>>(_First, _STD move(_UResult));
            }
        }

        template <forward_range _Rng, class _Ty, class _Pj = identity>
            requires indirect_binary_predicate<ranges::equal_to, projected<iterator_t<_Rng>, _Pj>, const _Ty*>
        _NODISCARD constexpr borrowed_subrange_t<_Rng> operator()(
            _Rng&& _Range, const _Ty& _Value, _Pj _Proj = {}) const {
            if constexpr (bidirectional_range<_Rng>) {
                auto _UResult = _Find_last_unchecked(
                    _Ubegin(_Range), _Get_final_iterator_unwrapped(_Range), _Value, _Pass_fn(_Proj));
                return _Rewrap_subrange<borrowed_subrange_t<_Rng>>(_Range, _STD move(_UResult));
            }
            else {
                auto _UResult = _Find_last_unchecked(_Ubegin(_Range), _Uend(_Range), _Value, _Pass_fn(_Proj));
                return _Rewrap_subrange<borrowed_subrange_t<_Rng>>(_Range, _STD move(_UResult));
            }
        }

    private:
        template <class _It, class _Se, class _Ty, class _Pj>
        _NODISCARD static constexpr subrange<_It> _Find_last_unchecked(
            _It _First, _Se _Last, const _Ty& _Value, _Pj _Proj) {
            _STL_INTERNAL_STATIC_ASSERT(forward_iterator<_It>);
            _STL_INTERNAL_STATIC_ASSERT(sentinel_for<_Se, _It>);
            _STL_INTERNAL_STATIC_ASSERT(indirect_binary_predicate<ranges::equal_to, projected<_It, _Pj>, const _Ty*>);

            if constexpr (_Bidi_common<_It, _Se>) {
                for (auto _Result = _Last; _Result != _First;) {
                    if (_STD invoke(_Proj, *--_Result) == _Value) {
                        return { _STD move(_Result), _STD move(_Last) };
                    }
                }
                return { _Last, _Last };
            }
            else if constexpr (same_as<_It, _Se>) {
                auto _Result = _Last;
                for (; _First != _Last; ++_First) {
                    if (_STD invoke(_Proj, *_First) == _Value) {
                        _Result = _First;
                    }
                }
                return { _STD move(_Result), _STD move(_Last) };
            }
            else {
                auto _Result = _First;
                bool _Found = false;
                for (;; ++_First) {
                    if (_First == _Last) {
                        if (!_Found) {
                            _Result = _First;
                        }
                        break;
                    }

                    if (_STD invoke(_Proj, *_First) == _Value) {
                        _Result = _First;
                        _Found = true;
                    }
                }
                return { _STD move(_Result), _STD move(_First) };
            }
        }
    };

    _EXPORT_STD inline constexpr _Find_last_fn find_last{ _Not_quite_object::_Construct_tag{} };

    template <bool _Search_for>
    class _Find_last_if_fn : private _Not_quite_object {
    public:
        using _Not_quite_object::_Not_quite_object;

        template <forward_iterator _It, sentinel_for<_It> _Se, class _Pj = identity,
            indirect_unary_predicate<projected<_It, _Pj>> _Pr>
        _NODISCARD constexpr subrange<_It> operator()(_It _First, _Se _Last, _Pr _Pred, _Pj _Proj = {}) const {
            _Adl_verify_range(_First, _Last);

            auto _UFirst = _Unwrap_iter<_Se>(_STD move(_First));
            if constexpr (bidirectional_iterator<_It>) {
                auto _ULast = _Get_final_iterator_unwrapped<_It>(_UFirst, _STD move(_Last));
                auto _UResult =
                    _Find_last_if_unchecked(_STD move(_UFirst), _STD move(_ULast), _Pass_fn(_Pred), _Pass_fn(_Proj));
                return _Rewrap_subrange<subrange<_It>>(_First, _STD move(_UResult));
            }
            else {
                auto _ULast = _Unwrap_sent<_It>(_STD move(_Last));
                auto _UResult =
                    _Find_last_if_unchecked(_STD move(_UFirst), _STD move(_ULast), _Pass_fn(_Pred), _Pass_fn(_Proj));
                return _Rewrap_subrange<subrange<_It>>(_First, _STD move(_UResult));
            }
        }

        template <forward_range _Rng, class _Pj = identity,
            indirect_unary_predicate<projected<iterator_t<_Rng>, _Pj>> _Pr>
        _NODISCARD constexpr borrowed_subrange_t<_Rng> operator()(_Rng&& _Range, _Pr _Pred, _Pj _Proj = {}) const {
            if constexpr (bidirectional_range<_Rng>) {
                auto _UResult = _Find_last_if_unchecked(
                    _Ubegin(_Range), _Get_final_iterator_unwrapped(_Range), _Pass_fn(_Pred), _Pass_fn(_Proj));
                return _Rewrap_subrange<borrowed_subrange_t<_Rng>>(_Range, _STD move(_UResult));
            }
            else {
                auto _UResult =
                    _Find_last_if_unchecked(_Ubegin(_Range), _Uend(_Range), _Pass_fn(_Pred), _Pass_fn(_Proj));
                return _Rewrap_subrange<borrowed_subrange_t<_Rng>>(_Range, _STD move(_UResult));
            }
        }

    private:
        template <class _It, class _Se, class _Pj, class _Pr>
        _NODISCARD static constexpr subrange<_It> _Find_last_if_unchecked(_It _First, _Se _Last, _Pr _Pred, _Pj _Proj) {
            _STL_INTERNAL_STATIC_ASSERT(forward_iterator<_It>);
            _STL_INTERNAL_STATIC_ASSERT(sentinel_for<_Se, _It>);
            _STL_INTERNAL_STATIC_ASSERT(indirect_unary_predicate<_Pr, projected<_It, _Pj>>);

            if constexpr (_Bidi_common<_It, _Se>) {
                for (auto _Result = _Last; _Result != _First;) {
                    if (_STD invoke_r<bool>(_Pred, _STD invoke(_Proj, *--_Result)) == _Search_for) {
                        return { _STD move(_Result), _STD move(_Last) };
                    }
                }
                return { _Last, _Last };
            }
            else if constexpr (same_as<_It, _Se>) {
                auto _Result = _Last;
                for (; _First != _Last; ++_First) {
                    if (_STD invoke_r<bool>(_Pred, _STD invoke(_Proj, *_First)) == _Search_for) {
                        _Result = _First;
                    }
                }
                return { _STD move(_Result), _STD move(_Last) };
            }
            else {
                auto _Result = _First;
                bool _Found = false;
                for (;; ++_First) {
                    if (_First == _Last) {
                        if (!_Found) {
                            _Result = _First;
                        }
                        break;
                    }

                    if (_STD invoke_r<bool>(_Pred, _STD invoke(_Proj, *_First)) == _Search_for) {
                        _Result = _First;
                        _Found = true;
                    }
                }
                return { _STD move(_Result), _STD move(_First) };
            }
        }
    };

    _EXPORT_STD inline constexpr _Find_last_if_fn<true> find_last_if{ _Not_quite_object::_Construct_tag{} };
    _EXPORT_STD inline constexpr _Find_last_if_fn<false> find_last_if_not{ _Not_quite_object::_Construct_tag{} };
#endif // _HAS_CXX23
} // namespace ranges
#endif // __cpp_lib_concepts

_EXPORT_STD template <class _FwdIt1, class _FwdIt2, class _Pr>
_NODISCARD _CONSTEXPR20 _FwdIt1 find_end(
    _FwdIt1 _First1, const _FwdIt1 _Last1, const _FwdIt2 _First2, const _FwdIt2 _Last2, _Pr _Pred) {
    // find last [_First2, _Last2) satisfying _Pred
    _Adl_verify_range(_First1, _Last1);
    _Adl_verify_range(_First2, _Last2);
    auto _UFirst1 = _Get_unwrapped(_First1);
    const auto _ULast1 = _Get_unwrapped(_Last1);
    const auto _UFirst2 = _Get_unwrapped(_First2);
    const auto _ULast2 = _Get_unwrapped(_Last2);
    if constexpr (_Is_ranges_random_iter_v<_FwdIt1> && _Is_ranges_random_iter_v<_FwdIt2>) {
        const _Iter_diff_t<_FwdIt2> _Count2 = _ULast2 - _UFirst2;
        if (_Count2 > 0 && _Count2 <= _ULast1 - _UFirst1) {
            for (auto _UCandidate = _ULast1 - static_cast<_Iter_diff_t<_FwdIt1>>(_Count2);; --_UCandidate) {
                if (_Equal_rev_pred_unchecked(_UCandidate, _UFirst2, _ULast2, _Pass_fn(_Pred))) {
                    _Seek_wrapped(_First1, _UCandidate);
                    return _First1;
                }

                if (_UCandidate == _UFirst1) {
                    break;
                }
            }
        }

        return _Last1;
    }
    else if constexpr (_Is_ranges_bidi_iter_v<_FwdIt1> && _Is_ranges_bidi_iter_v<_FwdIt2>) {
        for (auto _UCandidate = _ULast1;; --_UCandidate) { // try a match at _UCandidate
            auto _UNext1 = _UCandidate;
            auto _UNext2 = _ULast2;
            for (;;) { // test if [_UFirst2, _ULast2) is a suffix of [_UFirst1, _UCandidate)
                if (_UFirst2 == _UNext2) { // match found
                    _Seek_wrapped(_First1, _UNext1);
                    return _First1;
                }

                if (_UFirst1 == _UNext1) {
                    // [_UFirst1, _UCandidate) is shorter than [_UFirst2, _ULast2), remaining candidates nonviable
                    return _Last1;
                }

                --_UNext1;
                --_UNext2;
                if (!_Pred(*_UNext1, *_UNext2)) { // counterexample found
                    break;
                }
            }
        }
    }
    else {
        auto _UResult = _ULast1;
        for (;;) { // try a match at _UFirst1
            auto _UNext1 = _UFirst1;
            auto _UNext2 = _UFirst2;
            for (;;) { // test if [_UFirst2, _ULast2) is a prefix of [_UFirst1, _ULast1)
                const bool _End_of_needle = static_cast<bool>(_UNext2 == _ULast2);
                if (_End_of_needle) { // match candidate found
                    _UResult = _UFirst1;
                }

                if (_UNext1 == _ULast1) {
                    // trying the next candidate would make [_UFirst1, _ULast1) shorter than [_UFirst2, _ULast2), done
                    _Seek_wrapped(_First1, _UResult);
                    return _First1;
                }

                if (_End_of_needle || !_Pred(*_UNext1, *_UNext2)) {
                    break; // end of match or counterexample found, go to the next candidate
                }

                ++_UNext1;
                ++_UNext2;
            }

            ++_UFirst1;
        }

        _Seek_wrapped(_First1, _UResult);
        return _First1;
    }
}

_EXPORT_STD template <class _FwdIt1, class _FwdIt2>
_NODISCARD _CONSTEXPR20 _FwdIt1 find_end(
    _FwdIt1 const _First1, const _FwdIt1 _Last1, const _FwdIt2 _First2, const _FwdIt2 _Last2) {
    // find last [_First2, _Last2) match
    return _STD find_end(_First1, _Last1, _First2, _Last2, equal_to<>{});
}

#if _HAS_CXX17
_EXPORT_STD template <class _ExPo, class _FwdIt1, class _FwdIt2, class _Pr, _Enable_if_execution_policy_t<_ExPo> = 0>
_NODISCARD _FwdIt1 find_end(
    _ExPo&& _Exec, _FwdIt1 _First1, _FwdIt1 _Last1, _FwdIt2 _First2, _FwdIt2 _Last2, _Pr _Pred) noexcept; // terminates

_EXPORT_STD template <class _ExPo, class _FwdIt1, class _FwdIt2, _Enable_if_execution_policy_t<_ExPo> = 0>
_NODISCARD _FwdIt1 find_end(_ExPo&& _Exec, _FwdIt1 _First1, _FwdIt1 _Last1, _FwdIt2 _First2, _FwdIt2 _Last2) noexcept
/* terminates */ { // find last [_First2, _Last2) match
    return _STD find_end(_STD forward<_ExPo>(_Exec), _First1, _Last1, _First2, _Last2, equal_to{});
}
#endif // _HAS_CXX17

#ifdef __cpp_lib_concepts
namespace ranges {
    class _Find_end_fn : private _Not_quite_object {
    private:
        template <class _It1, class _It2, class _Pr, class _Pj1, class _Pj2>
        _NODISCARD static constexpr subrange<_It1> _Random_access_sized_ranges(_It1 _First1,
            const iter_difference_t<_It1> _Count1, _It2 _First2, const iter_difference_t<_It2> _Count2, _Pr _Pred,
            _Pj1 _Proj1, _Pj2 _Proj2) {
            _STL_INTERNAL_STATIC_ASSERT(random_access_iterator<_It1>);
            _STL_INTERNAL_STATIC_ASSERT(random_access_iterator<_It2>);
            _STL_INTERNAL_STATIC_ASSERT(indirectly_comparable<_It1, _It2, _Pr, _Pj1, _Pj2>);
            // pre: _First1 + [0, _Count1) is a valid counted range
            // pre: _First2 + [0, _Count2) is a valid counted range

            if (_Count2 > 0 && _Count2 <= _Count1) {
                const auto _Count2_as1 = static_cast<iter_difference_t<_It1>>(_Count2);

                for (auto _Candidate = _First1 + (_Count1 - _Count2_as1);; --_Candidate) {
                    auto [_Match, _Mid1] =
                        _RANGES _Equal_rev_pred(_Candidate, _First2, _First2 + _Count2, _Pred, _Proj1, _Proj2);
                    if (_Match) {
                        return { _STD move(_Candidate), _STD move(_Mid1) };
                    }

                    if (_Candidate == _First1) {
                        break;
                    }
                }
            }

            _First1 += _Count1;
            return { _First1, _First1 };
        }

        template <class _It1, class _It2, class _Pr, class _Pj1, class _Pj2>
        _NODISCARD static constexpr subrange<_It1> _Bidi_common_ranges(
            _It1 _First1, _It1 _Last1, _It2 _First2, const _It2 _Last2, _Pr _Pred, _Pj1 _Proj1, _Pj2 _Proj2) {
            _STL_INTERNAL_STATIC_ASSERT(bidirectional_iterator<_It1>);
            _STL_INTERNAL_STATIC_ASSERT(bidirectional_iterator<_It2>);
            _STL_INTERNAL_STATIC_ASSERT(indirectly_comparable<_It1, _It2, _Pr, _Pj1, _Pj2>);

            for (auto _Candidate = _Last1;; --_Candidate) { // try a match at _Candidate
                auto _Next1 = _Candidate;
                auto _Next2 = _Last2;
                for (;;) { // test if [_First2, _Last2) is a suffix of [_First1, _Candidate)
                    if (_First2 == _Next2) { // match found
                        return { _STD move(_Next1), _STD move(_Candidate) };
                    }

                    if (_First1 == _Next1) {
                        // [_First1, _Candidate) is shorter than [_First2, _Last2); remaining candidates nonviable
                        return { _Last1, _Last1 };
                    }

                    --_Next1;
                    --_Next2;
                    if (!_STD invoke(_Pred, _STD invoke(_Proj1, *_Next1), _STD invoke(_Proj2, *_Next2))) {
                        break; // mismatch
                    }
                }
            }
        }

        template <class _It1, class _Se1, class _It2, class _Se2, class _Pr, class _Pj1, class _Pj2>
        _NODISCARD static constexpr subrange<_It1> _Forward_ranges(
            _It1 _First1, const _Se1 _Last1, _It2 _First2, const _Se2 _Last2, _Pr _Pred, _Pj1 _Proj1, _Pj2 _Proj2) {
            _STL_INTERNAL_STATIC_ASSERT(forward_iterator<_It1>);
            _STL_INTERNAL_STATIC_ASSERT(sentinel_for<_Se1, _It1>);
            _STL_INTERNAL_STATIC_ASSERT(forward_iterator<_It2>);
            _STL_INTERNAL_STATIC_ASSERT(sentinel_for<_Se2, _It2>);
            _STL_INTERNAL_STATIC_ASSERT(indirectly_comparable<_It1, _It2, _Pr, _Pj1, _Pj2>);

            subrange<_It1> _Match{};
            bool _Found = false;

            for (;; ++_First1) { // try a match at _First1
                auto _Next1 = _First1;
                auto _Next2 = _First2;
                for (;; ++_Next1, (void) ++_Next2) { // test if [_First2, _Last2) is a prefix of [_First1, _Last1)
                    const bool _End_of_needle = _Next2 == _Last2;
                    if (_End_of_needle) { // match candidate found
                        _Match = subrange{ _First1, _Next1 };
                        _Found = true;
                    }

                    if (_Next1 == _Last1) { // haystack exhausted
                        if (!_Found) {
                            _Match = subrange{ _Next1, _Next1 };
                        }

                        return _Match;
                    }

                    if (_End_of_needle) {
                        break; // end of match found, go to the next candidate
                    }

                    if (!_STD invoke(_Pred, _STD invoke(_Proj1, *_Next1), _STD invoke(_Proj2, *_Next2))) {
                        break; // mismatch, go to the next candidate
                    }
                }
            }
        }

    public:
        using _Not_quite_object::_Not_quite_object;

        template <forward_iterator _It1, sentinel_for<_It1> _Se1, forward_iterator _It2, sentinel_for<_It2> _Se2,
            class _Pr = ranges::equal_to, class _Pj1 = identity, class _Pj2 = identity>
            requires indirectly_comparable<_It1, _It2, _Pr, _Pj1, _Pj2>
        _NODISCARD constexpr subrange<_It1> operator()(_It1 _First1, _Se1 _Last1, _It2 _First2, _Se2 _Last2,
            _Pr _Pred = {}, _Pj1 _Proj1 = {}, _Pj2 _Proj2 = {}) const {
            _Adl_verify_range(_First1, _Last1);
            _Adl_verify_range(_First2, _Last2);
            auto _UFirst1 = _Unwrap_iter<_Se1>(_First1);
            auto _ULast1 = _Unwrap_sent<_It1>(_Last1);
            auto _UFirst2 = _Unwrap_iter<_Se2>(_First2);
            auto _ULast2 = _Unwrap_sent<_It2>(_Last2);

            if constexpr (random_access_iterator<_It1> && sized_sentinel_for<_Se1, _It1> && random_access_iterator<_It2>
                && sized_sentinel_for<_Se2, _It2>) {
                const auto _Count1 = _ULast1 - _UFirst1;
                const auto _Count2 = _ULast2 - _UFirst2;
                auto _UResult = _Random_access_sized_ranges(_STD move(_UFirst1), _Count1, _STD move(_UFirst2), _Count2,
                    _Pass_fn(_Pred), _Pass_fn(_Proj1), _Pass_fn(_Proj2));
                return _Rewrap_subrange<subrange<_It1>>(_First1, _STD move(_UResult));
            }
            else if constexpr (_Bidi_common<_It1, _Se1> && _Bidi_common<_It2, _Se2>) {
                auto _UResult = _Bidi_common_ranges(_STD move(_UFirst1), _STD move(_ULast1), _STD move(_UFirst2),
                    _STD move(_ULast2), _Pass_fn(_Pred), _Pass_fn(_Proj1), _Pass_fn(_Proj2));
                return _Rewrap_subrange<subrange<_It1>>(_First1, _STD move(_UResult));
            }
            else {
                auto _UResult = _Forward_ranges(_STD move(_UFirst1), _STD move(_ULast1), _STD move(_UFirst2),
                    _STD move(_ULast2), _Pass_fn(_Pred), _Pass_fn(_Proj1), _Pass_fn(_Proj2));
                return _Rewrap_subrange<subrange<_It1>>(_First1, _STD move(_UResult));
            }
        }

        template <forward_range _Rng1, forward_range _Rng2, class _Pr = ranges::equal_to, class _Pj1 = identity,
            class _Pj2 = identity>
            requires indirectly_comparable<iterator_t<_Rng1>, iterator_t<_Rng2>, _Pr, _Pj1, _Pj2>
        _NODISCARD constexpr borrowed_subrange_t<_Rng1> operator()(
            _Rng1&& _Range1, _Rng2&& _Range2, _Pr _Pred = {}, _Pj1 _Proj1 = {}, _Pj2 _Proj2 = {}) const {
            if constexpr (random_access_range<_Rng1> && sized_range<_Rng1> && random_access_range<_Rng2>
                && sized_range<_Rng2>) {
                auto _UResult = _Random_access_sized_ranges(_Ubegin(_Range1), _RANGES distance(_Range1),
                    _Ubegin(_Range2), _RANGES distance(_Range2), _Pass_fn(_Pred), _Pass_fn(_Proj1), _Pass_fn(_Proj2));
                return _Rewrap_subrange<borrowed_subrange_t<_Rng1>>(_Range1, _STD move(_UResult));
            }
            else if constexpr (_Bidi_common_range<_Rng1> && _Bidi_common_range<_Rng2>) {
                auto _UResult = _Bidi_common_ranges(_Ubegin(_Range1), _Uend(_Range1), _Ubegin(_Range2), _Uend(_Range2),
                    _Pass_fn(_Pred), _Pass_fn(_Proj1), _Pass_fn(_Proj2));
                return _Rewrap_subrange<borrowed_subrange_t<_Rng1>>(_Range1, _STD move(_UResult));
            }
            else {
                auto _UResult = _Forward_ranges(_Ubegin(_Range1), _Uend(_Range1), _Ubegin(_Range2), _Uend(_Range2),
                    _Pass_fn(_Pred), _Pass_fn(_Proj1), _Pass_fn(_Proj2));
                return _Rewrap_subrange<borrowed_subrange_t<_Rng1>>(_Range1, _STD move(_UResult));
            }
        }
    };

    _EXPORT_STD inline constexpr _Find_end_fn find_end{ _Not_quite_object::_Construct_tag{} };
} // namespace ranges
#endif // __cpp_lib_concepts

_EXPORT_STD template <class _FwdIt1, class _FwdIt2, class _Pr>
_NODISCARD _CONSTEXPR20 _FwdIt1 find_first_of(
    _FwdIt1 _First1, const _FwdIt1 _Last1, const _FwdIt2 _First2, const _FwdIt2 _Last2, _Pr _Pred) {
    // look for one of [_First2, _Last2) satisfying _Pred with element
    _Adl_verify_range(_First1, _Last1);
    _Adl_verify_range(_First2, _Last2);
    auto _UFirst1 = _Get_unwrapped(_First1);
    const auto _ULast1 = _Get_unwrapped(_Last1);
    const auto _UFirst2 = _Get_unwrapped(_First2);
    const auto _ULast2 = _Get_unwrapped(_Last2);
    for (; _UFirst1 != _ULast1; ++_UFirst1) {
        for (auto _UMid2 = _UFirst2; _UMid2 != _ULast2; ++_UMid2) {
            if (_Pred(*_UFirst1, *_UMid2)) {
                _Seek_wrapped(_First1, _UFirst1);
                return _First1;
            }
        }
    }

    _Seek_wrapped(_First1, _UFirst1);
    return _First1;
}

_EXPORT_STD template <class _FwdIt1, class _FwdIt2>
_NODISCARD _CONSTEXPR20 _FwdIt1 find_first_of(const _FwdIt1 _First1, const _FwdIt1 _Last1, const _FwdIt2 _First2,
    const _FwdIt2 _Last2) { // look for one of [_First2, _Last2) that matches element
    return _STD find_first_of(_First1, _Last1, _First2, _Last2, equal_to<>{});
}

#if _HAS_CXX17
_EXPORT_STD template <class _ExPo, class _FwdIt1, class _FwdIt2, class _Pr, _Enable_if_execution_policy_t<_ExPo> = 0>
_NODISCARD _FwdIt1 find_first_of(
    _ExPo&& _Exec, _FwdIt1 _First1, _FwdIt1 _Last1, _FwdIt2 _First2, _FwdIt2 _Last2, _Pr _Pred) noexcept; // terminates

_EXPORT_STD template <class _ExPo, class _FwdIt1, class _FwdIt2, _Enable_if_execution_policy_t<_ExPo> = 0>
_NODISCARD _FwdIt1 find_first_of(_ExPo&& _Exec, const _FwdIt1 _First1, const _FwdIt1 _Last1, const _FwdIt2 _First2,
    const _FwdIt2 _Last2) noexcept /* terminates */ { // look for one of [_First2, _Last2) that matches element
    return _STD find_first_of(_STD forward<_ExPo>(_Exec), _First1, _Last1, _First2, _Last2, equal_to{});
}
#endif // _HAS_CXX17

#ifdef __cpp_lib_concepts
namespace ranges {
    class _Find_first_of_fn : private _Not_quite_object {
    public:
        using _Not_quite_object::_Not_quite_object;

        template <input_iterator _It1, sentinel_for<_It1> _Se1, forward_iterator _It2, sentinel_for<_It2> _Se2,
            class _Pr = ranges::equal_to, class _Pj1 = identity, class _Pj2 = identity>
            requires indirectly_comparable<_It1, _It2, _Pr, _Pj1, _Pj2>
        _NODISCARD constexpr _It1 operator()(_It1 _First1, _Se1 _Last1, _It2 _First2, _Se2 _Last2, _Pr _Pred = {},
            _Pj1 _Proj1 = {}, _Pj2 _Proj2 = {}) const {
            _Adl_verify_range(_First1, _Last1);
            _Adl_verify_range(_First2, _Last2);

            auto _UResult = _Find_first_of_unchecked(_Unwrap_iter<_Se1>(_STD move(_First1)),
                _Unwrap_sent<_It1>(_STD move(_Last1)), _Unwrap_iter<_Se2>(_STD move(_First2)),
                _Unwrap_sent<_It2>(_STD move(_Last2)), _Pass_fn(_Pred), _Pass_fn(_Proj1), _Pass_fn(_Proj2));

            _Seek_wrapped(_First1, _STD move(_UResult));
            return _First1;
        }

        template <input_range _Rng1, forward_range _Rng2, class _Pr = ranges::equal_to, class _Pj1 = identity,
            class _Pj2 = identity>
            requires indirectly_comparable<iterator_t<_Rng1>, iterator_t<_Rng2>, _Pr, _Pj1, _Pj2>
        _NODISCARD constexpr borrowed_iterator_t<_Rng1> operator()(
            _Rng1&& _Range1, _Rng2&& _Range2, _Pr _Pred = {}, _Pj1 _Proj1 = {}, _Pj2 _Proj2 = {}) const {
            auto _First1 = _RANGES begin(_Range1);

            auto _UResult = _Find_first_of_unchecked(_Unwrap_range_iter<_Rng1>(_STD move(_First1)), _Uend(_Range1),
                _Ubegin(_Range2), _Uend(_Range2), _Pass_fn(_Pred), _Pass_fn(_Proj1), _Pass_fn(_Proj2));

            _Seek_wrapped(_First1, _STD move(_UResult));
            return _First1;
        }

    private:
        template <class _It1, class _Se1, class _It2, class _Se2, class _Pr, class _Pj1, class _Pj2>
        _NODISCARD static constexpr _It1 _Find_first_of_unchecked(_It1 _First1, const _Se1 _Last1, const _It2 _First2,
            const _Se2 _Last2, _Pr _Pred, _Pj1 _Proj1, _Pj2 _Proj2) {
            _STL_INTERNAL_STATIC_ASSERT(input_iterator<_It1>);
            _STL_INTERNAL_STATIC_ASSERT(sentinel_for<_Se1, _It1>);
            _STL_INTERNAL_STATIC_ASSERT(forward_iterator<_It2>);
            _STL_INTERNAL_STATIC_ASSERT(sentinel_for<_Se2, _It2>);
            _STL_INTERNAL_STATIC_ASSERT(indirectly_comparable<_It1, _It2, _Pr, _Pj1, _Pj2>);

            for (; _First1 != _Last1; ++_First1) {
                for (auto _Mid2 = _First2; _Mid2 != _Last2; ++_Mid2) {
                    if (_STD invoke(_Pred, _STD invoke(_Proj1, *_First1), _STD invoke(_Proj2, *_Mid2))) {
                        return _First1;
                    }
                }
            }

            return _First1;
        }
    };

    _EXPORT_STD inline constexpr _Find_first_of_fn find_first_of{ _Not_quite_object::_Construct_tag{} };

    _EXPORT_STD template <class _In1, class _In2>
        using swap_ranges_result = in_in_result<_In1, _In2>;

    class _Swap_ranges_fn : private _Not_quite_object {
    public:
        using _Not_quite_object::_Not_quite_object;

        template <input_iterator _It1, sentinel_for<_It1> _Se1, input_iterator _It2, sentinel_for<_It2> _Se2>
            requires indirectly_swappable<_It1, _It2>
        constexpr swap_ranges_result<_It1, _It2> operator()(
            _It1 _First1, _Se1 _Last1, _It2 _First2, _Se2 _Last2) const {
            _Adl_verify_range(_First1, _Last1);
            _Adl_verify_range(_First2, _Last2);

            auto _UResult =
                _Swap_ranges_unchecked(_Unwrap_iter<_Se1>(_STD move(_First1)), _Unwrap_sent<_It1>(_STD move(_Last1)),
                    _Unwrap_iter<_Se2>(_STD move(_First2)), _Unwrap_sent<_It2>(_STD move(_Last2)));

            _Seek_wrapped(_First1, _STD move(_UResult.in1));
            _Seek_wrapped(_First2, _STD move(_UResult.in2));
            return { _STD move(_First1), _STD move(_First2) };
        }

        template <input_range _Rng1, input_range _Rng2>
            requires indirectly_swappable<iterator_t<_Rng1>, iterator_t<_Rng2>>
        constexpr swap_ranges_result<borrowed_iterator_t<_Rng1>, borrowed_iterator_t<_Rng2>> operator()(
            _Rng1&& _Range1, _Rng2&& _Range2) const {
            auto _First1 = _RANGES begin(_Range1);
            auto _First2 = _RANGES begin(_Range2);

            auto _UResult = _Swap_ranges_unchecked(_Unwrap_range_iter<_Rng1>(_STD move(_First1)), _Uend(_Range1),
                _Unwrap_range_iter<_Rng2>(_STD move(_First2)), _Uend(_Range2));

            _Seek_wrapped(_First1, _STD move(_UResult.in1));
            _Seek_wrapped(_First2, _STD move(_UResult.in2));
            return { _STD move(_First1), _STD move(_First2) };
        }

    private:
        template <class _It1, class _Se1, class _It2, class _Se2>
        _NODISCARD static constexpr swap_ranges_result<_It1, _It2> _Swap_ranges_unchecked(
            _It1 _First1, const _Se1 _Last1, _It2 _First2, const _Se2 _Last2) {
            _STL_INTERNAL_STATIC_ASSERT(input_iterator<_It1>);
            _STL_INTERNAL_STATIC_ASSERT(sentinel_for<_Se1, _It1>);
            _STL_INTERNAL_STATIC_ASSERT(input_iterator<_It2>);
            _STL_INTERNAL_STATIC_ASSERT(sentinel_for<_Se2, _It2>);
            _STL_INTERNAL_STATIC_ASSERT(indirectly_swappable<_It1, _It2>);

#if _USE_STD_VECTOR_ALGORITHMS
            using _Elem1 = remove_reference_t<iter_reference_t<_It1>>;
            using _Elem2 = remove_reference_t<iter_reference_t<_It2>>;
            if constexpr (same_as<_Elem1, _Elem2> && _Is_trivially_swappable_v<_Elem1> //
                && contiguous_iterator<_It1> && _Sized_or_unreachable_sentinel_for<_Se1, _It1> //
                && contiguous_iterator<_It2> && _Sized_or_unreachable_sentinel_for<_Se2, _It2>) {
                if (!_STD is_constant_evaluated()) {
                    constexpr bool _Is_sized1 = sized_sentinel_for<_Se1, _It1>;
                    constexpr bool _Is_sized2 = sized_sentinel_for<_Se2, _It2>;
                    const auto _First1_addr = _STD to_address(_First1);
                    const auto _First2_addr = _STD to_address(_First2);
                    if constexpr (_Is_sized1 && _Is_sized2) {
                        const size_t _Count =
                            (_STD min)(static_cast<size_t>(_Last1 - _First1), static_cast<size_t>(_Last2 - _First2));
                        const auto _Last1_addr = _First1_addr + _Count;
                        __std_swap_ranges_trivially_swappable_noalias(_First1_addr, _Last1_addr, _First2_addr);
                        return { _First1 + _Count, _First2 + _Count };
                    }
                    else if constexpr (_Is_sized1) {
                        const auto _Final1 = _RANGES next(_First1, _Last1);
                        const auto _Last1_addr = _STD to_address(_Final1);
                        __std_swap_ranges_trivially_swappable_noalias(_First1_addr, _Last1_addr, _First2_addr);
                        return { _Final1, _First2 + (_Last1 - _First1) };
                    }
                    else if constexpr (_Is_sized2) {
                        const auto _Final2 = _RANGES next(_First2, _Last2);
                        const auto _Last2_addr = _STD to_address(_Final2);
                        __std_swap_ranges_trivially_swappable_noalias(_First2_addr, _Last2_addr, _First1_addr);
                        return { _First1 + (_Last2 - _First2), _Final2 };
                    }
                    else {
                        _STL_ASSERT(false, "Tried to swap_ranges with two unreachable sentinels");
                    }
                }
            }
#endif // _USE_STD_VECTOR_ALGORITHMS

            for (; _First1 != _Last1 && _First2 != _Last2; ++_First1, (void) ++_First2) {
                _RANGES iter_swap(_First1, _First2);
            }

            return { _STD move(_First1), _STD move(_First2) };
        }
    };

    _EXPORT_STD inline constexpr _Swap_ranges_fn swap_ranges{ _Not_quite_object::_Construct_tag{} };
} // namespace ranges
#endif // __cpp_lib_concepts

_EXPORT_STD template <class _FwdIt1, class _FwdIt2>
_CONSTEXPR20 _FwdIt2 swap_ranges(const _FwdIt1 _First1, const _FwdIt1 _Last1, _FwdIt2 _First2) {
    // swap [_First1, _Last1) with [_First2, ...)
    _Adl_verify_range(_First1, _Last1);
    const auto _UFirst1 = _Get_unwrapped(_First1);
    const auto _ULast1 = _Get_unwrapped(_Last1);
    const auto _UFirst2 = _Get_unwrapped_n(_First2, _Idl_distance<_FwdIt1>(_UFirst1, _ULast1));
    _Verify_ranges_do_not_overlap(_UFirst1, _ULast1, _UFirst2);
    _Seek_wrapped(_First2, _Swap_ranges_unchecked(_UFirst1, _ULast1, _UFirst2));
    return _First2;
}

#if _HAS_CXX17
_EXPORT_STD template <class _ExPo, class _FwdIt1, class _FwdIt2, _Enable_if_execution_policy_t<_ExPo> = 0>
_FwdIt2 swap_ranges(_ExPo&&, _FwdIt1 _First1, _FwdIt1 _Last1, _FwdIt2 _Dest) noexcept /* terminates */ {
    // swap [_First1, _Last1) with [_Dest, ...)
    // not parallelized as benchmarks show it isn't worth it
    return _STD swap_ranges(_First1, _Last1, _Dest);
}
#endif // _HAS_CXX17

_EXPORT_STD template <class _FwdIt1, class _FwdIt2>
_CONSTEXPR20 void iter_swap(_FwdIt1 _Left, _FwdIt2 _Right) { // swap *_Left and *_Right
    swap(*_Left, *_Right); // intentional ADL
}

_EXPORT_STD template <class _InIt, class _OutIt, class _Fn>
_CONSTEXPR20 _OutIt transform(const _InIt _First, const _InIt _Last, _OutIt _Dest, _Fn _Func) {
    // transform [_First, _Last) with _Func
    _Adl_verify_range(_First, _Last);
    auto _UFirst = _Get_unwrapped(_First);
    const auto _ULast = _Get_unwrapped(_Last);
    auto _UDest = _Get_unwrapped_n(_Dest, _Idl_distance<_InIt>(_UFirst, _ULast));
    for (; _UFirst != _ULast; ++_UFirst, (void) ++_UDest) {
        *_UDest = _Func(*_UFirst);
    }

    _Seek_wrapped(_Dest, _UDest);
    return _Dest;
}

#if _HAS_CXX17
_EXPORT_STD template <class _ExPo, class _FwdIt1, class _FwdIt2, class _Fn, _Enable_if_execution_policy_t<_ExPo> = 0>
_FwdIt2 transform(_ExPo&& _Exec, _FwdIt1 _First, _FwdIt1 _Last, _FwdIt2 _Dest, _Fn _Func) noexcept; // terminates
#endif // _HAS_CXX17

_EXPORT_STD template <class _InIt1, class _InIt2, class _OutIt, class _Fn>
_CONSTEXPR20 _OutIt transform(
    const _InIt1 _First1, const _InIt1 _Last1, const _InIt2 _First2, _OutIt _Dest, _Fn _Func) {
    // transform [_First1, _Last1) and [_First2, ...) with _Func
    _Adl_verify_range(_First1, _Last1);
    auto _UFirst1 = _Get_unwrapped(_First1);
    const auto _ULast1 = _Get_unwrapped(_Last1);
    const auto _Count = _Idl_distance<_InIt1>(_UFirst1, _ULast1);
    auto _UFirst2 = _Get_unwrapped_n(_First2, _Count);
    auto _UDest = _Get_unwrapped_n(_Dest, _Count);
    for (; _UFirst1 != _ULast1; ++_UFirst1, (void) ++_UFirst2, ++_UDest) {
        *_UDest = _Func(*_UFirst1, *_UFirst2);
    }

    _Seek_wrapped(_Dest, _UDest);
    return _Dest;
}

#if _HAS_CXX17
_EXPORT_STD template <class _ExPo, class _FwdIt1, class _FwdIt2, class _FwdIt3, class _Fn,
    _Enable_if_execution_policy_t<_ExPo> = 0>
    _FwdIt3 transform(_ExPo&& _Exec, _FwdIt1 _First1, _FwdIt1 _Last1, _FwdIt2 _First2, _FwdIt3 _Dest,
        _Fn _Func) noexcept; // terminates
#endif // _HAS_CXX17

#ifdef __cpp_lib_concepts
namespace ranges {
    _EXPORT_STD template <class _In, class _Out>
        using unary_transform_result = in_out_result<_In, _Out>;

    _EXPORT_STD template <class _In1, class _In2, class _Out>
        using binary_transform_result = in_in_out_result<_In1, _In2, _Out>;

    class _Transform_fn : private _Not_quite_object {
    public:
        using _Not_quite_object::_Not_quite_object;

        template <input_iterator _It, sentinel_for<_It> _Se, weakly_incrementable _Out, copy_constructible _Fn,
            class _Pj = identity>
            requires indirectly_writable<_Out, indirect_result_t<_Fn&, projected<_It, _Pj>>>
        constexpr unary_transform_result<_It, _Out> operator()(
            _It _First, _Se _Last, _Out _Result, _Fn _Func, _Pj _Proj = {}) const {
            _Adl_verify_range(_First, _Last);
            auto _UResult = _Transform_unary_unchecked(_Unwrap_iter<_Se>(_STD move(_First)),
                _Unwrap_sent<_It>(_STD move(_Last)), _STD move(_Result), _Pass_fn(_Func), _Pass_fn(_Proj));

            _Seek_wrapped(_First, _STD move(_UResult.in));
            return { _STD move(_First), _STD move(_UResult.out) };
        }

        template <input_range _Rng, weakly_incrementable _Out, copy_constructible _Fn, class _Pj = identity>
            requires indirectly_writable<_Out, indirect_result_t<_Fn&, projected<iterator_t<_Rng>, _Pj>>>
        constexpr unary_transform_result<borrowed_iterator_t<_Rng>, _Out> operator()(
            _Rng&& _Range, _Out _Result, _Fn _Func, _Pj _Proj = {}) const {
            auto _First = _RANGES begin(_Range);
            auto _UResult = _Transform_unary_unchecked(_Unwrap_range_iter<_Rng>(_STD move(_First)), _Uend(_Range),
                _STD move(_Result), _Pass_fn(_Func), _Pass_fn(_Proj));

            _Seek_wrapped(_First, _STD move(_UResult.in));
            return { _STD move(_First), _STD move(_UResult.out) };
        }

        template <input_iterator _It1, sentinel_for<_It1> _Se1, input_iterator _It2, sentinel_for<_It2> _Se2,
            weakly_incrementable _Out, copy_constructible _Fn, class _Pj1 = identity, class _Pj2 = identity>
            requires indirectly_writable<_Out, indirect_result_t<_Fn&, projected<_It1, _Pj1>, projected<_It2, _Pj2>>>
        constexpr binary_transform_result<_It1, _It2, _Out> operator()(_It1 _First1, _Se1 _Last1, _It2 _First2,
            _Se2 _Last2, _Out _Result, _Fn _Func, _Pj1 _Proj1 = {}, _Pj2 _Proj2 = {}) const {
            _Adl_verify_range(_First1, _Last1);
            _Adl_verify_range(_First2, _Last2);
            auto _UResult = _Transform_binary_unchecked(_Unwrap_iter<_Se1>(_STD move(_First1)),
                _Unwrap_sent<_It1>(_STD move(_Last1)), _Unwrap_iter<_Se2>(_STD move(_First2)),
                _Unwrap_sent<_It2>(_STD move(_Last2)), _STD move(_Result), _Pass_fn(_Func), _Pass_fn(_Proj1),
                _Pass_fn(_Proj2));

            _Seek_wrapped(_First1, _STD move(_UResult.in1));
            _Seek_wrapped(_First2, _STD move(_UResult.in2));
            return { _STD move(_First1), _STD move(_First2), _STD move(_UResult.out) };
        }

        template <input_range _Rng1, input_range _Rng2, weakly_incrementable _Out, copy_constructible _Fn,
            class _Pj1 = identity, class _Pj2 = identity>
            requires indirectly_writable<_Out,
        indirect_result_t<_Fn&, projected<iterator_t<_Rng1>, _Pj1>, projected<iterator_t<_Rng2>, _Pj2>>>
        constexpr binary_transform_result<borrowed_iterator_t<_Rng1>, borrowed_iterator_t<_Rng2>, _Out> operator()(
            _Rng1&& _Range1, _Rng2&& _Range2, _Out _Result, _Fn _Func, _Pj1 _Proj1 = {}, _Pj2 _Proj2 = {}) const {
            auto _First1 = _RANGES begin(_Range1);
            auto _First2 = _RANGES begin(_Range2);
            auto _UResult = _Transform_binary_unchecked(_Unwrap_range_iter<_Rng1>(_STD move(_First1)), _Uend(_Range1),
                _Unwrap_range_iter<_Rng2>(_STD move(_First2)), _Uend(_Range2), _STD move(_Result), _Pass_fn(_Func),
                _Pass_fn(_Proj1), _Pass_fn(_Proj2));

            _Seek_wrapped(_First1, _STD move(_UResult.in1));
            _Seek_wrapped(_First2, _STD move(_UResult.in2));
            return { _STD move(_First1), _STD move(_First2), _STD move(_UResult.out) };
        }

    private:
        template <class _It, class _Se, class _Out, class _Fn, class _Pj>
        _NODISCARD static constexpr unary_transform_result<_It, _Out> _Transform_unary_unchecked(
            _It _First, const _Se _Last, _Out _Result, _Fn _Func, _Pj _Proj) {
            // transform projected [_First, _Last) with _Func
            _STL_INTERNAL_STATIC_ASSERT(input_iterator<_It>);
            _STL_INTERNAL_STATIC_ASSERT(weakly_incrementable<_Out>);
            _STL_INTERNAL_STATIC_ASSERT(indirectly_writable<_Out, indirect_result_t<_Fn&, projected<_It, _Pj>>>);

            for (; _First != _Last; ++_First, (void) ++_Result) {
                *_Result = _STD invoke(_Func, _STD invoke(_Proj, *_First));
            }

            return { _STD move(_First), _STD move(_Result) };
        }

        template <class _It1, class _Se1, class _It2, class _Se2, class _Out, class _Fn, class _Pj1, class _Pj2>
        _NODISCARD static constexpr binary_transform_result<_It1, _It2, _Out> _Transform_binary_unchecked(_It1 _First1,
            const _Se1 _Last1, _It2 _First2, const _Se2 _Last2, _Out _Result, _Fn _Func, _Pj1 _Proj1, _Pj2 _Proj2) {
            // transform projected [_First1, _Last1) and projected [_First2, _Last2) with _Func
            _STL_INTERNAL_STATIC_ASSERT(input_iterator<_It1>);
            _STL_INTERNAL_STATIC_ASSERT(input_iterator<_It2>);
            _STL_INTERNAL_STATIC_ASSERT(weakly_incrementable<_Out>);
            _STL_INTERNAL_STATIC_ASSERT(
                indirectly_writable<_Out, indirect_result_t<_Fn&, projected<_It1, _Pj1>, projected<_It2, _Pj2>>>);

            for (; _First1 != _Last1 && _First2 != _Last2; ++_First1, (void) ++_First2, ++_Result) {
                *_Result = _STD invoke(_Func, _STD invoke(_Proj1, *_First1), _STD invoke(_Proj2, *_First2));
            }

            return { _STD move(_First1), _STD move(_First2), _STD move(_Result) };
        }
    };

    _EXPORT_STD inline constexpr _Transform_fn transform{ _Not_quite_object::_Construct_tag{} };
} // namespace ranges
#endif // __cpp_lib_concepts

_EXPORT_STD template <class _FwdIt, class _Ty>
_CONSTEXPR20 void replace(const _FwdIt _First, const _FwdIt _Last, const _Ty& _Oldval, const _Ty& _Newval) {
    // replace each matching _Oldval with _Newval
    _Adl_verify_range(_First, _Last);
    auto _UFirst = _Get_unwrapped(_First);
    const auto _ULast = _Get_unwrapped(_Last);
    for (; _UFirst != _ULast; ++_UFirst) {
        if (*_UFirst == _Oldval) {
            *_UFirst = _Newval;
        }
    }
}

#if _HAS_CXX17
_EXPORT_STD template <class _ExPo, class _FwdIt, class _Ty, _Enable_if_execution_policy_t<_ExPo> = 0>
void replace(_ExPo&& _Exec, _FwdIt _First, _FwdIt _Last, const _Ty& _Oldval,
    const _Ty& _Newval) noexcept; // terminates
#endif // _HAS_CXX17

#ifdef __cpp_lib_concepts
namespace ranges {
    class _Replace_fn : private _Not_quite_object {
    public:
        using _Not_quite_object::_Not_quite_object;

        template <input_iterator _It, sentinel_for<_It> _Se, class _Ty1, class _Ty2, class _Pj = identity>
            requires indirectly_writable<_It, const _Ty2&>
        && indirect_binary_predicate<ranges::equal_to, projected<_It, _Pj>, const _Ty1*>
            constexpr _It operator()(
                _It _First, _Se _Last, const _Ty1& _Oldval, const _Ty2& _Newval, _Pj _Proj = {}) const {
            _Adl_verify_range(_First, _Last);
            auto _UResult = _Replace_unchecked(_Unwrap_iter<_Se>(_STD move(_First)),
                _Unwrap_sent<_It>(_STD move(_Last)), _Oldval, _Newval, _Pass_fn(_Proj));

            _Seek_wrapped(_First, _STD move(_UResult));
            return _First;
        }

        template <input_range _Rng, class _Ty1, class _Ty2, class _Pj = identity>
            requires indirectly_writable<iterator_t<_Rng>, const _Ty2&>
        && indirect_binary_predicate<ranges::equal_to, projected<iterator_t<_Rng>, _Pj>, const _Ty1*>
            constexpr borrowed_iterator_t<_Rng> operator()(
                _Rng&& _Range, const _Ty1& _Oldval, const _Ty2& _Newval, _Pj _Proj = {}) const {
            auto _First = _RANGES begin(_Range);
            auto _UResult = _Replace_unchecked(
                _Unwrap_range_iter<_Rng>(_STD move(_First)), _Uend(_Range), _Oldval, _Newval, _Pass_fn(_Proj));

            _Seek_wrapped(_First, _STD move(_UResult));
            return _First;
        }

    private:
        template <class _It, class _Se, class _Ty1, class _Ty2, class _Pj>
        _NODISCARD static constexpr _It _Replace_unchecked(
            _It _First, const _Se _Last, const _Ty1& _Oldval, const _Ty2& _Newval, _Pj _Proj) {
            // replace projected _Oldval with _Newval in [_First, _Last)
            _STL_INTERNAL_STATIC_ASSERT(input_iterator<_It>);
            _STL_INTERNAL_STATIC_ASSERT(sentinel_for<_Se, _It>);
            _STL_INTERNAL_STATIC_ASSERT(indirectly_writable<_It, const _Ty2&>);
            _STL_INTERNAL_STATIC_ASSERT(indirect_binary_predicate<ranges::equal_to, projected<_It, _Pj>, const _Ty1*>);

            for (; _First != _Last; ++_First) {
                if (_STD invoke(_Proj, *_First) == _Oldval) {
                    *_First = _Newval;
                }
            }

            return _First;
        }
    };

    _EXPORT_STD inline constexpr _Replace_fn replace{ _Not_quite_object::_Construct_tag{} };
} // namespace ranges
#endif // __cpp_lib_concepts

_EXPORT_STD template <class _FwdIt, class _Pr, class _Ty>
_CONSTEXPR20 void replace_if(const _FwdIt _First, const _FwdIt _Last, _Pr _Pred, const _Ty& _Val) {
    // replace each satisfying _Pred with _Val
    _Adl_verify_range(_First, _Last);
    auto _UFirst = _Get_unwrapped(_First);
    const auto _ULast = _Get_unwrapped(_Last);
    for (; _UFirst != _ULast; ++_UFirst) {
        if (_Pred(*_UFirst)) {
            *_UFirst = _Val;
        }
    }
}

#if _HAS_CXX17
_EXPORT_STD template <class _ExPo, class _FwdIt, class _Pr, class _Ty, _Enable_if_execution_policy_t<_ExPo> = 0>
void replace_if(_ExPo&& _Exec, _FwdIt _First, _FwdIt _Last, _Pr _Pred, const _Ty& _Val) noexcept; // terminates
#endif // _HAS_CXX17

#ifdef __cpp_lib_concepts
namespace ranges {
    class _Replace_if_fn : private _Not_quite_object {
    public:
        using _Not_quite_object::_Not_quite_object;

        template <input_iterator _It, sentinel_for<_It> _Se, class _Ty, class _Pj = identity,
            indirect_unary_predicate<projected<_It, _Pj>> _Pr>
            requires indirectly_writable<_It, const _Ty&>
        constexpr _It operator()(_It _First, _Se _Last, _Pr _Pred, const _Ty& _Newval, _Pj _Proj = {}) const {
            _Adl_verify_range(_First, _Last);
            auto _UResult = _Replace_if_unchecked(_Unwrap_iter<_Se>(_STD move(_First)),
                _Unwrap_sent<_It>(_STD move(_Last)), _Pass_fn(_Pred), _Newval, _Pass_fn(_Proj));

            _Seek_wrapped(_First, _STD move(_UResult));
            return _First;
        }

        template <input_range _Rng, class _Ty, class _Pj = identity,
            indirect_unary_predicate<projected<iterator_t<_Rng>, _Pj>> _Pr>
            requires indirectly_writable<iterator_t<_Rng>, const _Ty&>
        constexpr borrowed_iterator_t<_Rng> operator()(
            _Rng&& _Range, _Pr _Pred, const _Ty& _Newval, _Pj _Proj = {}) const {
            auto _First = _RANGES begin(_Range);
            auto _UResult = _Replace_if_unchecked(
                _Unwrap_range_iter<_Rng>(_STD move(_First)), _Uend(_Range), _Pass_fn(_Pred), _Newval, _Pass_fn(_Proj));

            _Seek_wrapped(_First, _STD move(_UResult));
            return _First;
        }

    private:
        template <class _It, class _Se, class _Ty, class _Pj, class _Pr>
        _NODISCARD static constexpr _It _Replace_if_unchecked(
            _It _First, const _Se _Last, _Pr _Pred, const _Ty& _Newval, _Pj _Proj) {
            // replace projected _Oldval that fulfills _Pred with _Newval in [_First, _Last)
            _STL_INTERNAL_STATIC_ASSERT(input_iterator<_It>);
            _STL_INTERNAL_STATIC_ASSERT(sentinel_for<_Se, _It>);
            _STL_INTERNAL_STATIC_ASSERT(indirectly_writable<_It, const _Ty&>);
            _STL_INTERNAL_STATIC_ASSERT(indirect_unary_predicate<_Pr, projected<_It, _Pj>>);

            for (; _First != _Last; ++_First) {
                if (_STD invoke(_Pred, _STD invoke(_Proj, *_First))) {
                    *_First = _Newval;
                }
            }

            return _First;
        }
    };

    _EXPORT_STD inline constexpr _Replace_if_fn replace_if{ _Not_quite_object::_Construct_tag{} };
} // namespace ranges
#endif // __cpp_lib_concepts

_EXPORT_STD template <class _InIt, class _OutIt, class _Ty>
_CONSTEXPR20 _OutIt replace_copy(_InIt _First, _InIt _Last, _OutIt _Dest, const _Ty& _Oldval, const _Ty& _Newval) {
    // copy replacing each matching _Oldval with _Newval
    _Adl_verify_range(_First, _Last);
    auto _UFirst = _Get_unwrapped(_First);
    const auto _ULast = _Get_unwrapped(_Last);
    auto _UDest = _Get_unwrapped_n(_Dest, _Idl_distance<_InIt>(_UFirst, _ULast));
    for (; _UFirst != _ULast; ++_UFirst, (void) ++_UDest) {
        if (*_UFirst == _Oldval) {
            *_UDest = _Newval;
        }
        else {
            *_UDest = *_UFirst;
        }
    }

    _Seek_wrapped(_Dest, _UDest);
    return _Dest;
}

#if _HAS_CXX17
_EXPORT_STD template <class _ExPo, class _FwdIt1, class _FwdIt2, class _Ty, _Enable_if_execution_policy_t<_ExPo> = 0>
_FwdIt2 replace_copy(_ExPo&&, _FwdIt1 _First, _FwdIt1 _Last, _FwdIt2 _Dest, const _Ty& _Oldval,
    const _Ty& _Newval) noexcept /* terminates */ {
    // copy replacing each matching _Oldval with _Newval
    // not parallelized at present, parallelism expected to be feasible in a future release
    _REQUIRE_PARALLEL_ITERATOR(_FwdIt1);
    _REQUIRE_CPP17_MUTABLE_ITERATOR(_FwdIt2);
    return _STD replace_copy(_First, _Last, _Dest, _Oldval, _Newval);
}
#endif // _HAS_CXX17

#ifdef __cpp_lib_concepts
namespace ranges {
    _EXPORT_STD template <class _In, class _Out>
        using replace_copy_result = in_out_result<_In, _Out>;

    class _Replace_copy_fn : private _Not_quite_object {
    public:
        using _Not_quite_object::_Not_quite_object;

        template <input_iterator _It, sentinel_for<_It> _Se, class _Ty1, class _Ty2, output_iterator<const _Ty2&> _Out,
            class _Pj = identity>
            requires indirectly_copyable<_It, _Out>
        && indirect_binary_predicate<ranges::equal_to, projected<_It, _Pj>, const _Ty1*>
            constexpr replace_copy_result<_It, _Out> operator()(
                _It _First, _Se _Last, _Out _Result, const _Ty1& _Oldval, const _Ty2& _Newval, _Pj _Proj = {}) const {
            _Adl_verify_range(_First, _Last);
            auto _UResult = _Replace_copy_unchecked(_Unwrap_iter<_Se>(_STD move(_First)),
                _Unwrap_sent<_It>(_STD move(_Last)), _STD move(_Result), _Oldval, _Newval, _Pass_fn(_Proj));

            _Seek_wrapped(_First, _STD move(_UResult.in));
            return { _STD move(_First), _STD move(_UResult.out) };
        }

        template <input_range _Rng, class _Ty1, class _Ty2, output_iterator<const _Ty2&> _Out, class _Pj = identity>
            requires indirectly_copyable<iterator_t<_Rng>, _Out>
        && indirect_binary_predicate<ranges::equal_to, projected<iterator_t<_Rng>, _Pj>, const _Ty1*>
            constexpr replace_copy_result<borrowed_iterator_t<_Rng>, _Out> operator()(
                _Rng&& _Range, _Out _Result, const _Ty1& _Oldval, const _Ty2& _Newval, _Pj _Proj = {}) const {
            auto _First = _RANGES begin(_Range);
            auto _UResult = _Replace_copy_unchecked(_Unwrap_range_iter<_Rng>(_STD move(_First)), _Uend(_Range),
                _STD move(_Result), _Oldval, _Newval, _Pass_fn(_Proj));

            _Seek_wrapped(_First, _STD move(_UResult.in));
            return { _STD move(_First), _STD move(_UResult.out) };
        }

    private:
        template <class _It, class _Se, class _Ty1, class _Ty2, class _Out, class _Pj>
        _NODISCARD static constexpr replace_copy_result<_It, _Out> _Replace_copy_unchecked(
            _It _First, const _Se _Last, _Out _Result, const _Ty1& _Oldval, const _Ty2& _Newval, _Pj _Proj) {
            // copy [_First, _Last) to _Result while replacing projected _Oldval with _Newval
            _STL_INTERNAL_STATIC_ASSERT(input_iterator<_It>);
            _STL_INTERNAL_STATIC_ASSERT(sentinel_for<_Se, _It>);
            _STL_INTERNAL_STATIC_ASSERT(output_iterator<_Out, const _Ty2&>);
            _STL_INTERNAL_STATIC_ASSERT(indirectly_copyable<_It, _Out>);
            _STL_INTERNAL_STATIC_ASSERT(indirect_binary_predicate<equal_to, projected<_It, _Pj>, const _Ty1*>);

            for (; _First != _Last; ++_First, (void) ++_Result) {
                if (_STD invoke(_Proj, *_First) == _Oldval) {
                    *_Result = _Newval;
                }
                else {
                    *_Result = *_First;
                }
            }

            return { _STD move(_First), _STD move(_Result) };
        }
    };

    _EXPORT_STD inline constexpr _Replace_copy_fn replace_copy{ _Not_quite_object::_Construct_tag{} };
} // namespace ranges
#endif // __cpp_lib_concepts

_EXPORT_STD template <class _InIt, class _OutIt, class _Pr, class _Ty>
_CONSTEXPR20 _OutIt replace_copy_if(_InIt _First, _InIt _Last, _OutIt _Dest, _Pr _Pred, const _Ty& _Val) {
    // copy replacing each satisfying _Pred with _Val
    _Adl_verify_range(_First, _Last);
    auto _UFirst = _Get_unwrapped(_First);
    const auto _ULast = _Get_unwrapped(_Last);
    auto _UDest = _Get_unwrapped_n(_Dest, _Idl_distance<_InIt>(_UFirst, _ULast));
    for (; _UFirst != _ULast; ++_UFirst, (void) ++_UDest) {
        if (_Pred(*_UFirst)) {
            *_UDest = _Val;
        }
        else {
            *_UDest = *_UFirst;
        }
    }

    _Seek_wrapped(_Dest, _UDest);
    return _Dest;
}

#if _HAS_CXX17
_EXPORT_STD template <class _ExPo, class _FwdIt1, class _FwdIt2, class _Pr, class _Ty,
    _Enable_if_execution_policy_t<_ExPo> = 0>
    _FwdIt2 replace_copy_if(_ExPo&&, _FwdIt1 _First, _FwdIt1 _Last, _FwdIt2 _Dest, _Pr _Pred, const _Ty& _Val) noexcept
    /* terminates */ {
    // copy replacing each satisfying _Pred with _Val
    // not parallelized at present, parallelism expected to be feasible in a future release
    _REQUIRE_PARALLEL_ITERATOR(_FwdIt1);
    _REQUIRE_CPP17_MUTABLE_ITERATOR(_FwdIt2);
    return _STD replace_copy_if(_First, _Last, _Dest, _Pass_fn(_Pred), _Val);
}
#endif // _HAS_CXX17

#ifdef __cpp_lib_concepts
namespace ranges {
    _EXPORT_STD template <class _In, class _Out>
        using replace_copy_if_result = in_out_result<_In, _Out>;

    class _Replace_copy_if_fn : private _Not_quite_object {
    public:
        using _Not_quite_object::_Not_quite_object;

        template <input_iterator _It, sentinel_for<_It> _Se, class _Ty, output_iterator<const _Ty&> _Out,
            class _Pj = identity, indirect_unary_predicate<projected<_It, _Pj>> _Pr>
            requires indirectly_copyable<_It, _Out>
        constexpr replace_copy_if_result<_It, _Out> operator()(
            _It _First, _Se _Last, _Out _Result, _Pr _Pred, const _Ty& _Newval, _Pj _Proj = {}) const {
            _Adl_verify_range(_First, _Last);
            auto _UResult = _Replace_copy_if_unchecked(_Unwrap_iter<_Se>(_STD move(_First)),
                _Unwrap_sent<_It>(_STD move(_Last)), _STD move(_Result), _Pass_fn(_Pred), _Newval, _Pass_fn(_Proj));

            _Seek_wrapped(_First, _STD move(_UResult.in));
            return { _STD move(_First), _STD move(_UResult.out) };
        }

        template <input_range _Rng, class _Ty, output_iterator<const _Ty&> _Out, class _Pj = identity,
            indirect_unary_predicate<projected<iterator_t<_Rng>, _Pj>> _Pr>
            requires indirectly_copyable<iterator_t<_Rng>, _Out>
        constexpr replace_copy_if_result<borrowed_iterator_t<_Rng>, _Out> operator()(
            _Rng&& _Range, _Out _Result, _Pr _Pred, const _Ty& _Newval, _Pj _Proj = {}) const {
            auto _First = _RANGES begin(_Range);
            auto _UResult = _Replace_copy_if_unchecked(_Unwrap_range_iter<_Rng>(_STD move(_First)), _Uend(_Range),
                _STD move(_Result), _Pass_fn(_Pred), _Newval, _Pass_fn(_Proj));

            _Seek_wrapped(_First, _STD move(_UResult.in));
            return { _STD move(_First), _STD move(_UResult.out) };
        }

    private:
        template <class _It, class _Se, class _Ty, class _Out, class _Pj, class _Pr>
        _NODISCARD static constexpr replace_copy_if_result<_It, _Out> _Replace_copy_if_unchecked(
            _It _First, const _Se _Last, _Out _Result, _Pr _Pred, const _Ty& _Newval, _Pj _Proj) {
            // copy [_First, _Last) to _Result while replacing _Oldval with _Newval if projected _Oldval fulfills _Pred
            _STL_INTERNAL_STATIC_ASSERT(input_iterator<_It>);
            _STL_INTERNAL_STATIC_ASSERT(sentinel_for<_Se, _It>);
            _STL_INTERNAL_STATIC_ASSERT(output_iterator<_Out, const _Ty&>);
            _STL_INTERNAL_STATIC_ASSERT(indirectly_copyable<_It, _Out>);
            _STL_INTERNAL_STATIC_ASSERT(indirect_unary_predicate<_Pr, projected<_It, _Pj>>);

            for (; _First != _Last; ++_First, (void) ++_Result) {
                if (_STD invoke(_Pred, _STD invoke(_Proj, *_First))) {
                    *_Result = _Newval;
                }
                else {
                    *_Result = *_First;
                }
            }

            return { _STD move(_First), _STD move(_Result) };
        }
    };

    _EXPORT_STD inline constexpr _Replace_copy_if_fn replace_copy_if{ _Not_quite_object::_Construct_tag{} };

    class _Fill_fn : private _Not_quite_object {
    public:
        using _Not_quite_object::_Not_quite_object;

        template <class _Ty, output_iterator<const _Ty&> _It, sentinel_for<_It> _Se>
        constexpr _It operator()(_It _First, _Se _Last, const _Ty& _Value) const {
            _Adl_verify_range(_First, _Last);
            auto _UFirst = _Unwrap_iter<_Se>(_STD move(_First));
            const auto _ULast = _Unwrap_sent<_It>(_STD move(_Last));
            if (!_STD is_constant_evaluated()) {
                if constexpr (sized_sentinel_for<decltype(_ULast), decltype(_UFirst)>) {
                    if constexpr (_Fill_memset_is_safe<decltype(_UFirst), _Ty>) {
                        const auto _Distance = static_cast<size_t>(_ULast - _UFirst);
                        _Fill_memset(_UFirst, _Value, _Distance);
                        _Seek_wrapped(_First, _UFirst + _Distance);
                        return _First;
                    }
                    else if constexpr (_Fill_zero_memset_is_safe<decltype(_UFirst), _Ty>) {
                        if (_Is_all_bits_zero(_Value)) {
                            const auto _Distance = static_cast<size_t>(_ULast - _UFirst);
                            _Fill_zero_memset(_UFirst, _Distance);
                            _Seek_wrapped(_First, _UFirst + _Distance);
                            return _First;
                        }
                    }
                }
            }

            for (; _UFirst != _ULast; ++_UFirst) {
                *_UFirst = _Value;
            }

            _Seek_wrapped(_First, _STD move(_UFirst));
            return _First;
        }

        template <class _Ty, output_range<const _Ty&> _Rng>
        constexpr borrowed_iterator_t<_Rng> operator()(_Rng&& _Range, const _Ty& _Value) const {
            auto _First = _RANGES begin(_Range);
            _Seek_wrapped(_First, (*this)(_Unwrap_range_iter<_Rng>(_STD move(_First)), _Uend(_Range), _Value));
            return _First;
        }
    };

    _EXPORT_STD inline constexpr _Fill_fn fill{ _Not_quite_object::_Construct_tag{} };

    class _Generate_fn : private _Not_quite_object {
    public:
        using _Not_quite_object::_Not_quite_object;

        template <input_or_output_iterator _Out, sentinel_for<_Out> _Se, copy_constructible _Fn>
            requires invocable<_Fn&>&& indirectly_writable<_Out, invoke_result_t<_Fn&>>
        constexpr _Out operator()(_Out _First, _Se _Last, _Fn _Gen) const {
            _Adl_verify_range(_First, _Last);
            _Seek_wrapped(_First, _Generate_unchecked(_Unwrap_iter<_Se>(_STD move(_First)),
                _Unwrap_sent<_Out>(_STD move(_Last)), _Pass_fn(_Gen)));
            return _First;
        }

        template <class _Rng, copy_constructible _Fn>
            requires invocable<_Fn&>&& output_range<_Rng, invoke_result_t<_Fn&>>
        constexpr borrowed_iterator_t<_Rng> operator()(_Rng&& _Range, _Fn _Gen) const {
            auto _First = _RANGES begin(_Range);
            _Seek_wrapped(_First,
                _Generate_unchecked(_Unwrap_range_iter<_Rng>(_STD move(_First)), _Uend(_Range), _Pass_fn(_Gen)));
            return _First;
        }

    private:
        template <class _Out, class _Se, class _Fn>
        _NODISCARD static constexpr _Out _Generate_unchecked(_Out _First, const _Se _Last, _Fn _Gen) {
            _STL_INTERNAL_STATIC_ASSERT(input_or_output_iterator<_Out>);
            _STL_INTERNAL_STATIC_ASSERT(sentinel_for<_Se, _Out>);
            _STL_INTERNAL_STATIC_ASSERT(copy_constructible<_Fn>);
            _STL_INTERNAL_STATIC_ASSERT(invocable<_Fn&>);
            _STL_INTERNAL_STATIC_ASSERT(indirectly_writable<_Out, invoke_result_t<_Fn&>>);

            for (; _First != _Last; ++_First) {
                *_First = _Gen();
            }

            return _First;
        }
    };

    _EXPORT_STD inline constexpr _Generate_fn generate{ _Not_quite_object::_Construct_tag{} };

    class _Generate_n_fn : private _Not_quite_object {
    public:
        using _Not_quite_object::_Not_quite_object;

        template <input_or_output_iterator _Out, copy_constructible _Fn>
            requires invocable<_Fn&>&& indirectly_writable<_Out, invoke_result_t<_Fn&>>
        constexpr _Out operator()(_Out _First, iter_difference_t<_Out> _Count, _Fn _Gen) const {
            if (_Count > 0) {
                auto _UFirst = _Get_unwrapped_n(_STD move(_First), _Count);
                do {
                    *_UFirst = _Gen();
                    ++_UFirst;
                } while (--_Count > 0);

                _Seek_wrapped(_First, _STD move(_UFirst));
            }

            return _First;
        }
    };

    _EXPORT_STD inline constexpr _Generate_n_fn generate_n{ _Not_quite_object::_Construct_tag{} };
} // namespace ranges
#endif // __cpp_lib_concepts

_EXPORT_STD template <class _FwdIt, class _Fn>
_CONSTEXPR20 void generate(_FwdIt _First, _FwdIt _Last, _Fn _Func) { // replace [_First, _Last) with _Func()
    _Adl_verify_range(_First, _Last);
    auto _UFirst = _Get_unwrapped(_First);
    const auto _ULast = _Get_unwrapped(_Last);
    for (; _UFirst != _ULast; ++_UFirst) {
        *_UFirst = _Func();
    }
}

#if _HAS_CXX17
_EXPORT_STD template <class _ExPo, class _FwdIt, class _Fn, _Enable_if_execution_policy_t<_ExPo> = 0>
void generate(_ExPo&&, _FwdIt _First, _FwdIt _Last, _Fn _Func) noexcept /* terminates */ {
    // replace [_First, _Last) with _Func()
    // not parallelized at present due to unclear parallelism requirements on _Func
    return _STD generate(_First, _Last, _Pass_fn(_Func));
}
#endif // _HAS_CXX17

_EXPORT_STD template <class _OutIt, class _Diff, class _Fn>
_CONSTEXPR20 _OutIt generate_n(_OutIt _Dest, const _Diff _Count_raw, _Fn _Func) {
    // replace [_Dest, _Dest + _Count) with _Func()
    _Algorithm_int_t<_Diff> _Count = _Count_raw;
    if (0 < _Count) {
        auto _UDest = _Get_unwrapped_n(_Dest, _Count);
        do {
            *_UDest = _Func();
            --_Count;
            ++_UDest;
        } while (0 < _Count);

        _Seek_wrapped(_Dest, _UDest);
    }

    return _Dest;
}

#if _HAS_CXX17
_EXPORT_STD template <class _ExPo, class _FwdIt, class _Diff, class _Fn, _Enable_if_execution_policy_t<_ExPo> = 0>
_FwdIt generate_n(_ExPo&&, const _FwdIt _Dest, const _Diff _Count_raw, _Fn _Func) noexcept /* terminates */ {
    // replace [_Dest, _Dest + _Count) with _Func()
    // not parallelized at present due to unclear parallelism requirements on _Func
    _REQUIRE_CPP17_MUTABLE_ITERATOR(_FwdIt);
    return _STD generate_n(_Dest, _Count_raw, _Pass_fn(_Func));
}
#endif // _HAS_CXX17

_EXPORT_STD template <class _InIt, class _OutIt, class _Ty>
_CONSTEXPR20 _OutIt remove_copy(_InIt _First, _InIt _Last, _OutIt _Dest, const _Ty& _Val) {
    // copy omitting each matching _Val
    _Adl_verify_range(_First, _Last);
    auto _UFirst = _Get_unwrapped(_First);
    const auto _ULast = _Get_unwrapped(_Last);
    auto _UDest = _Get_unwrapped_unverified(_Dest);
    for (; _UFirst != _ULast; ++_UFirst) {
        if (!(*_UFirst == _Val)) {
            *_UDest = *_UFirst;
            ++_UDest;
        }
    }

    _Seek_wrapped(_Dest, _UDest);
    return _Dest;
}

#if _HAS_CXX17
_EXPORT_STD template <class _ExPo, class _FwdIt1, class _FwdIt2, class _Ty, _Enable_if_execution_policy_t<_ExPo> = 0>
_FwdIt2 remove_copy(_ExPo&&, _FwdIt1 _First, _FwdIt1 _Last, _FwdIt2 _Dest, const _Ty& _Val) noexcept /* terminates */ {
    // copy omitting each matching _Val
    // not parallelized at present, parallelism expected to be feasible in a future release
    _REQUIRE_PARALLEL_ITERATOR(_FwdIt1);
    _REQUIRE_CPP17_MUTABLE_ITERATOR(_FwdIt2);
    return _STD remove_copy(_First, _Last, _Dest, _Val);
}
#endif // _HAS_CXX17

_EXPORT_STD template <class _InIt, class _OutIt, class _Pr>
_CONSTEXPR20 _OutIt remove_copy_if(_InIt _First, _InIt _Last, _OutIt _Dest, _Pr _Pred) {
    // copy omitting each element satisfying _Pred
    _Adl_verify_range(_First, _Last);
    auto _UFirst = _Get_unwrapped(_First);
    const auto _ULast = _Get_unwrapped(_Last);
    auto _UDest = _Get_unwrapped_unverified(_Dest);
    for (; _UFirst != _ULast; ++_UFirst) {
        if (!_Pred(*_UFirst)) {
            *_UDest = *_UFirst;
            ++_UDest;
        }
    }

    _Seek_wrapped(_Dest, _UDest);
    return _Dest;
}

#if _HAS_CXX17
_EXPORT_STD template <class _ExPo, class _FwdIt1, class _FwdIt2, class _Pr, _Enable_if_execution_policy_t<_ExPo> = 0>
_FwdIt2 remove_copy_if(_ExPo&&, _FwdIt1 _First, _FwdIt1 _Last, _FwdIt2 _Dest, _Pr _Pred) noexcept /* terminates */ {
    // copy omitting each element satisfying _Pred
    // not parallelized at present, parallelism expected to be feasible in a future release
    _REQUIRE_PARALLEL_ITERATOR(_FwdIt1);
    _REQUIRE_CPP17_MUTABLE_ITERATOR(_FwdIt2);
    return _STD remove_copy_if(_First, _Last, _Dest, _Pass_fn(_Pred));
}
#endif // _HAS_CXX17

#if _HAS_CXX17
_EXPORT_STD template <class _ExPo, class _FwdIt, class _Ty, _Enable_if_execution_policy_t<_ExPo> = 0>
_NODISCARD_REMOVE_ALG _FwdIt remove(_ExPo&& _Exec, _FwdIt _First, _FwdIt _Last, const _Ty& _Val) noexcept; // terminates

_EXPORT_STD template <class _ExPo, class _FwdIt, class _Pr, _Enable_if_execution_policy_t<_ExPo> = 0>
_NODISCARD_REMOVE_ALG _FwdIt remove_if(_ExPo&& _Exec, _FwdIt _First, _FwdIt _Last, _Pr _Pred) noexcept; // terminates
#endif // _HAS_CXX17

#ifdef __cpp_lib_concepts
namespace ranges {
    class _Remove_fn : private _Not_quite_object {
    public:
        using _Not_quite_object::_Not_quite_object;

        template <permutable _It, sentinel_for<_It> _Se, class _Ty, class _Pj = identity>
            requires indirect_binary_predicate<ranges::equal_to, projected<_It, _Pj>, const _Ty*>
        _NODISCARD_REMOVE_ALG constexpr subrange<_It> operator()(
            _It _First, _Se _Last, const _Ty& _Val, _Pj _Proj = {}) const {
            _Adl_verify_range(_First, _Last);
            auto _UResult = _Remove_unchecked(
                _Unwrap_iter<_Se>(_STD move(_First)), _Unwrap_sent<_It>(_STD move(_Last)), _Val, _Pass_fn(_Proj));

            return _Rewrap_subrange<subrange<_It>>(_First, _STD move(_UResult));
        }

        template <forward_range _Rng, class _Ty, class _Pj = identity>
            requires permutable<iterator_t<_Rng>>
        && indirect_binary_predicate<ranges::equal_to, projected<iterator_t<_Rng>, _Pj>, const _Ty*>
            _NODISCARD_REMOVE_ALG constexpr borrowed_subrange_t<_Rng> operator()(
                _Rng&& _Range, const _Ty& _Val, _Pj _Proj = {}) const {
            auto _UResult = _Remove_unchecked(_Ubegin(_Range), _Uend(_Range), _Val, _Pass_fn(_Proj));

            return _Rewrap_subrange<borrowed_subrange_t<_Rng>>(_Range, _STD move(_UResult));
        }

    private:
        template <class _It, class _Se, class _Ty, class _Pj>
        _NODISCARD static constexpr subrange<_It> _Remove_unchecked(
            _It _First, const _Se _Last, const _Ty& _Val, _Pj _Proj) {
            // Remove projected values equal to _Val from [_First, _Last)
            _STL_INTERNAL_STATIC_ASSERT(permutable<_It>);
            _STL_INTERNAL_STATIC_ASSERT(sentinel_for<_Se, _It>);
            _STL_INTERNAL_STATIC_ASSERT(indirect_binary_predicate<ranges::equal_to, projected<_It, _Pj>, const _Ty*>);

            _First = _RANGES _Find_unchecked(_STD move(_First), _Last, _Val, _Proj);
            auto _Next = _First;
            if (_First == _Last) {
                return { _STD move(_Next), _STD move(_First) };
            }

            while (++_First != _Last) {
                if (_STD invoke(_Proj, *_First) != _Val) {
                    *_Next = _RANGES iter_move(_First);
                    ++_Next;
                }
            }

            return { _STD move(_Next), _STD move(_First) };
        }
    };

    _EXPORT_STD inline constexpr _Remove_fn remove{ _Not_quite_object::_Construct_tag{} };

    class _Remove_if_fn : private _Not_quite_object {
    public:
        using _Not_quite_object::_Not_quite_object;

        template <permutable _It, sentinel_for<_It> _Se, class _Pj = identity,
            indirect_unary_predicate<projected<_It, _Pj>> _Pr>
        _NODISCARD_REMOVE_ALG constexpr subrange<_It> operator()(
            _It _First, _Se _Last, _Pr _Pred, _Pj _Proj = {}) const {
            _Adl_verify_range(_First, _Last);
            auto _UResult = _Remove_if_unchecked(_Unwrap_iter<_Se>(_STD move(_First)),
                _Unwrap_sent<_It>(_STD move(_Last)), _Pass_fn(_Pred), _Pass_fn(_Proj));

            return _Rewrap_subrange<subrange<_It>>(_First, _STD move(_UResult));
        }

        template <forward_range _Rng, class _Pj = identity,
            indirect_unary_predicate<projected<iterator_t<_Rng>, _Pj>> _Pr>
            requires permutable<iterator_t<_Rng>>
        _NODISCARD_REMOVE_ALG constexpr borrowed_subrange_t<_Rng> operator()(
            _Rng&& _Range, _Pr _Pred, _Pj _Proj = {}) const {
            auto _UResult = _Remove_if_unchecked(_Ubegin(_Range), _Uend(_Range), _Pass_fn(_Pred), _Pass_fn(_Proj));

            return _Rewrap_subrange<borrowed_subrange_t<_Rng>>(_Range, _STD move(_UResult));
        }

    private:
        template <class _It, class _Se, class _Pr, class _Pj>
        _NODISCARD static constexpr subrange<_It> _Remove_if_unchecked(
            _It _First, const _Se _Last, _Pr _Pred, _Pj _Proj) {
            // Remove values whose projection satisfies _Pred from [_First, _Last)
            _STL_INTERNAL_STATIC_ASSERT(permutable<_It>);
            _STL_INTERNAL_STATIC_ASSERT(sentinel_for<_Se, _It>);
            _STL_INTERNAL_STATIC_ASSERT(indirect_unary_predicate<_Pr, projected<_It, _Pj>>);

            _First = _RANGES _Find_if_unchecked(_STD move(_First), _Last, _Pred, _Proj);
            auto _Next = _First;
            if (_First == _Last) {
                return { _STD move(_Next), _STD move(_First) };
            }

            while (++_First != _Last) {
                if (!_STD invoke(_Pred, _STD invoke(_Proj, *_First))) {
                    *_Next = _RANGES iter_move(_First);
                    ++_Next;
                }
            }

            return { _STD move(_Next), _STD move(_First) };
        }
    };

    _EXPORT_STD inline constexpr _Remove_if_fn remove_if{ _Not_quite_object::_Construct_tag{} };

    _EXPORT_STD template <class _In, class _Out>
        using remove_copy_result = in_out_result<_In, _Out>;

    class _Remove_copy_fn : private _Not_quite_object {
    public:
        using _Not_quite_object::_Not_quite_object;

        template <input_iterator _It, sentinel_for<_It> _Se, weakly_incrementable _Out, class _Ty, class _Pj = identity>
            requires indirectly_copyable<_It, _Out>
        && indirect_binary_predicate<ranges::equal_to, projected<_It, _Pj>, const _Ty*>
            constexpr remove_copy_result<_It, _Out> operator()(
                _It _First, _Se _Last, _Out _Result, const _Ty& _Val, _Pj _Proj = {}) const {
            _Adl_verify_range(_First, _Last);
            auto _UResult =
                _Remove_copy_unchecked(_Unwrap_iter<_Se>(_STD move(_First)), _Unwrap_sent<_It>(_STD move(_Last)),
                    _Get_unwrapped_unverified(_STD move(_Result)), _Val, _Pass_fn(_Proj));

            _Seek_wrapped(_First, _STD move(_UResult.in));
            _Seek_wrapped(_Result, _STD move(_UResult.out));
            return { _STD move(_First), _STD move(_Result) };
        }

        template <input_range _Rng, weakly_incrementable _Out, class _Ty, class _Pj = identity>
            requires indirectly_copyable<iterator_t<_Rng>, _Out>
        && indirect_binary_predicate<ranges::equal_to, projected<iterator_t<_Rng>, _Pj>, const _Ty*>
            constexpr remove_copy_result<borrowed_iterator_t<_Rng>, _Out> operator()(
                _Rng&& _Range, _Out _Result, const _Ty& _Val, _Pj _Proj = {}) const {
            auto _First = _RANGES begin(_Range);
            auto _UResult = _Remove_copy_unchecked(_Unwrap_range_iter<_Rng>(_STD move(_First)), _Uend(_Range),
                _Get_unwrapped_unverified(_STD move(_Result)), _Val, _Pass_fn(_Proj));

            _Seek_wrapped(_First, _STD move(_UResult.in));
            _Seek_wrapped(_Result, _STD move(_UResult.out));
            return { _STD move(_First), _STD move(_Result) };
        }

    private:
        template <class _It, class _Se, class _Out, class _Ty, class _Pj>
        _NODISCARD static constexpr remove_copy_result<_It, _Out> _Remove_copy_unchecked(
            _It _First, const _Se _Last, _Out _Result, const _Ty& _Val, _Pj _Proj) {
            // Copy [_First, _Last) to _Result except projected values equal to _Val
            _STL_INTERNAL_STATIC_ASSERT(input_iterator<_It>);
            _STL_INTERNAL_STATIC_ASSERT(sentinel_for<_Se, _It>);
            _STL_INTERNAL_STATIC_ASSERT(weakly_incrementable<_Out>);
            _STL_INTERNAL_STATIC_ASSERT(indirectly_copyable<_It, _Out>);
            _STL_INTERNAL_STATIC_ASSERT(indirect_binary_predicate<ranges::equal_to, projected<_It, _Pj>, const _Ty*>);

            for (; _First != _Last; ++_First) {
                if (_STD invoke(_Proj, *_First) != _Val) {
                    *_Result = *_First;
                    ++_Result;
                }
            }

            return { _STD move(_First), _STD move(_Result) };
        }
    };

    _EXPORT_STD inline constexpr _Remove_copy_fn remove_copy{ _Not_quite_object::_Construct_tag{} };

    _EXPORT_STD template <class _In, class _Out>
        using remove_copy_if_result = in_out_result<_In, _Out>;

    class _Remove_copy_if_fn : private _Not_quite_object {
    public:
        using _Not_quite_object::_Not_quite_object;

        template <input_iterator _It, sentinel_for<_It> _Se, weakly_incrementable _Out, class _Pj = identity,
            indirect_unary_predicate<projected<_It, _Pj>> _Pr>
            requires indirectly_copyable<_It, _Out>
        constexpr remove_copy_if_result<_It, _Out> operator()(
            _It _First, _Se _Last, _Out _Result, _Pr _Pred, _Pj _Proj = {}) const {
            _Adl_verify_range(_First, _Last);
            auto _UResult =
                _Remove_copy_if_unchecked(_Unwrap_iter<_Se>(_STD move(_First)), _Unwrap_sent<_It>(_STD move(_Last)),
                    _Get_unwrapped_unverified(_STD move(_Result)), _Pass_fn(_Pred), _Pass_fn(_Proj));

            _Seek_wrapped(_First, _STD move(_UResult.in));
            _Seek_wrapped(_Result, _STD move(_UResult.out));
            return { _STD move(_First), _STD move(_Result) };
        }

        template <input_range _Rng, weakly_incrementable _Out, class _Pj = identity,
            indirect_unary_predicate<projected<iterator_t<_Rng>, _Pj>> _Pr>
            requires indirectly_copyable<iterator_t<_Rng>, _Out>
        constexpr remove_copy_if_result<borrowed_iterator_t<_Rng>, _Out> operator()(
            _Rng&& _Range, _Out _Result, _Pr _Pred, _Pj _Proj = {}) const {
            auto _First = _RANGES begin(_Range);
            auto _UResult = _Remove_copy_if_unchecked(_Unwrap_range_iter<_Rng>(_STD move(_First)), _Uend(_Range),
                _Get_unwrapped_unverified(_STD move(_Result)), _Pass_fn(_Pred), _Pass_fn(_Proj));

            _Seek_wrapped(_First, _STD move(_UResult.in));
            _Seek_wrapped(_Result, _STD move(_UResult.out));
            return { _STD move(_First), _STD move(_Result) };
        }

    private:
        template <class _It, class _Se, class _Out, class _Pr, class _Pj>
        _NODISCARD static constexpr remove_copy_if_result<_It, _Out> _Remove_copy_if_unchecked(
            _It _First, const _Se _Last, _Out _Result, _Pr _Pred, _Pj _Proj) {
            // Copy [_First, _Last) to _Result except projected values that satisfy _Pred
            _STL_INTERNAL_STATIC_ASSERT(input_iterator<_It>);
            _STL_INTERNAL_STATIC_ASSERT(sentinel_for<_Se, _It>);
            _STL_INTERNAL_STATIC_ASSERT(weakly_incrementable<_Out>);
            _STL_INTERNAL_STATIC_ASSERT(indirectly_copyable<_It, _Out>);
            _STL_INTERNAL_STATIC_ASSERT(indirect_unary_predicate<_Pr, projected<_It, _Pj>>);

            for (; _First != _Last; ++_First) {
                if (!_STD invoke(_Pred, _STD invoke(_Proj, *_First))) {
                    *_Result = *_First;
                    ++_Result;
                }
            }

            return { _STD move(_First), _STD move(_Result) };
        }
    };

    _EXPORT_STD inline constexpr _Remove_copy_if_fn remove_copy_if{ _Not_quite_object::_Construct_tag{} };
} // namespace ranges
#endif // __cpp_lib_concepts

_EXPORT_STD template <class _FwdIt, class _Pr>
_NODISCARD_UNIQUE_ALG _CONSTEXPR20 _FwdIt unique(_FwdIt _First, _FwdIt _Last, _Pr _Pred) {
    // remove each satisfying _Pred with previous
    _Adl_verify_range(_First, _Last);
    auto _UFirst = _Get_unwrapped(_First);
    const auto _ULast = _Get_unwrapped(_Last);
    if (_UFirst != _ULast) {
        for (auto _UFirstb = _UFirst; ++_UFirst != _ULast; _UFirstb = _UFirst) {
            if (_Pred(*_UFirstb, *_UFirst)) { // copy down
                while (++_UFirst != _ULast) {
                    if (!_Pred(*_UFirstb, *_UFirst)) {
                        *++_UFirstb = _STD move(*_UFirst);
                    }
                }

                _Seek_wrapped(_Last, ++_UFirstb);
                return _Last;
            }
        }
    }

    _Seek_wrapped(_Last, _ULast);
    return _Last;
}

_EXPORT_STD template <class _FwdIt>
_NODISCARD_UNIQUE_ALG _CONSTEXPR20 _FwdIt unique(_FwdIt _First, _FwdIt _Last) { // remove each matching previous
    return _STD unique(_First, _Last, equal_to<>{});
}

#if _HAS_CXX17
_EXPORT_STD template <class _ExPo, class _FwdIt, class _Pr, _Enable_if_execution_policy_t<_ExPo> = 0>
_NODISCARD_UNIQUE_ALG _FwdIt unique(_ExPo&&, _FwdIt _First, _FwdIt _Last, _Pr _Pred) noexcept /* terminates */ {
    // remove each satisfying _Pred with previous
    // not parallelized at present, parallelism expected to be feasible in a future release
    return _STD unique(_First, _Last, _Pass_fn(_Pred));
}

_EXPORT_STD template <class _ExPo, class _FwdIt, _Enable_if_execution_policy_t<_ExPo> = 0>
_NODISCARD_UNIQUE_ALG _FwdIt unique(_ExPo&&, _FwdIt _First, _FwdIt _Last) noexcept /* terminates */ {
    // remove each matching previous
    // not parallelized at present, parallelism expected to be feasible in a future release
    return _STD unique(_First, _Last);
}
#endif // _HAS_CXX17

#ifdef __cpp_lib_concepts
namespace ranges {
    class _Unique_fn : private _Not_quite_object {
    public:
        using _Not_quite_object::_Not_quite_object;

        template <permutable _It, sentinel_for<_It> _Se, class _Pj = identity,
            indirect_equivalence_relation<projected<_It, _Pj>> _Pr = ranges::equal_to>
        _NODISCARD_UNIQUE_ALG constexpr subrange<_It> operator()(
            _It _First, _Se _Last, _Pr _Pred = {}, _Pj _Proj = {}) const {
            _Adl_verify_range(_First, _Last);
            auto _UResult = _Unique_unchecked(_Unwrap_iter<_Se>(_STD move(_First)), _Unwrap_sent<_It>(_STD move(_Last)),
                _Pass_fn(_Pred), _Pass_fn(_Proj));

            return _Rewrap_subrange<subrange<_It>>(_First, _STD move(_UResult));
        }

        template <forward_range _Rng, class _Pj = identity,
            indirect_equivalence_relation<projected<iterator_t<_Rng>, _Pj>> _Pr = ranges::equal_to>
            requires permutable<iterator_t<_Rng>>
        _NODISCARD_UNIQUE_ALG constexpr borrowed_subrange_t<_Rng> operator()(
            _Rng&& _Range, _Pr _Pred = {}, _Pj _Proj = {}) const {
            auto _UResult = _Unique_unchecked(_Ubegin(_Range), _Uend(_Range), _Pass_fn(_Pred), _Pass_fn(_Proj));

            return _Rewrap_subrange<borrowed_subrange_t<_Rng>>(_Range, _STD move(_UResult));
        }

    private:
        template <class _It, class _Se, class _Pj, class _Pr>
        _NODISCARD static constexpr subrange<_It> _Unique_unchecked(_It _First, const _Se _Last, _Pr _Pred, _Pj _Proj) {
            // Remove adjacent elements from [_First, _Last) whose projections satisfy _Pred
            _STL_INTERNAL_STATIC_ASSERT(permutable<_It>);
            _STL_INTERNAL_STATIC_ASSERT(sentinel_for<_Se, _It>);
            _STL_INTERNAL_STATIC_ASSERT(indirect_equivalence_relation<_Pr, projected<_It, _Pj>>);

            auto _Current = _First;
            if (_First == _Last) {
                return { _STD move(_Current), _STD move(_First) };
            }

            for (;; ++_Current) {
                if (++_First == _Last) {
                    ++_Current;
                    return { _STD move(_Current), _STD move(_First) };
                }

                if (_STD invoke(_Pred, _STD invoke(_Proj, *_Current), _STD invoke(_Proj, *_First))) {
                    break;
                }
            }

            while (++_First != _Last) {
                if (!_STD invoke(_Pred, _STD invoke(_Proj, *_Current), _STD invoke(_Proj, *_First))) {
                    ++_Current;
                    *_Current = _RANGES iter_move(_First);
                }
            }
            ++_Current;

            return { _STD move(_Current), _STD move(_First) };
        }
    };

    _EXPORT_STD inline constexpr _Unique_fn unique{ _Not_quite_object::_Construct_tag{} };
} // namespace ranges
#endif // __cpp_lib_concepts

template <class _InIt, class _OutIt>
#ifdef __cpp_lib_concepts
concept
#else
_INLINE_VAR constexpr bool
#endif
_Can_reread_dest = _Is_cpp17_fwd_iter_v<_OutIt> && is_same_v<_Iter_value_t<_InIt>, _Iter_value_t<_OutIt>>;

_EXPORT_STD template <class _InIt, class _OutIt, class _Pr>
_CONSTEXPR20 _OutIt unique_copy(_InIt _First, _InIt _Last, _OutIt _Dest, _Pr _Pred) {
    // copy compressing pairs that match
    _Adl_verify_range(_First, _Last);

    auto _UFirst = _Get_unwrapped(_First);
    const auto _ULast = _Get_unwrapped(_Last);

    if (_UFirst == _ULast) {
        return _Dest;
    }

    auto _UDest = _Get_unwrapped_unverified(_Dest);

    if constexpr (_Is_ranges_fwd_iter_v<_InIt>) { // can reread the source for comparison
        auto _Firstb = _UFirst;

        *_UDest = *_Firstb;
        ++_UDest;

        while (++_UFirst != _ULast) {
            if (!static_cast<bool>(_Pred(*_Firstb, *_UFirst))) { // copy unmatched
                _Firstb = _UFirst;
                *_UDest = *_Firstb;
                ++_UDest;
            }
        }
    }
    else if constexpr (_Can_reread_dest<_InIt, _OutIt>) { // assignment copies T; can reread dest for comparison
        *_UDest = *_UFirst;

        while (++_UFirst != _ULast) {
            if (!static_cast<bool>(_Pred(*_UDest, *_UFirst))) {
                *++_UDest = *_UFirst;
            }
        }

        ++_UDest;
    }
    else { // can't reread source or dest, construct a temporary
        _Iter_value_t<_InIt> _Val = *_UFirst;

        *_UDest = _Val;
        ++_UDest;

        while (++_UFirst != _ULast) {
            if (!static_cast<bool>(_Pred(_Val, *_UFirst))) { // copy unmatched
                _Val = *_UFirst;
                *_UDest = _Val;
                ++_UDest;
            }
        }
    }

    _Seek_wrapped(_Dest, _UDest);
    return _Dest;
}

_EXPORT_STD template <class _InIt, class _OutIt>
_CONSTEXPR20 _OutIt unique_copy(_InIt _First, _InIt _Last, _OutIt _Dest) { // copy compressing pairs that match
    return _STD unique_copy(_First, _Last, _Dest, equal_to<>{});
}

#if _HAS_CXX17
_EXPORT_STD template <class _ExPo, class _FwdIt1, class _FwdIt2, class _Pr, _Enable_if_execution_policy_t<_ExPo> = 0>
_FwdIt2 unique_copy(_ExPo&&, _FwdIt1 _First, _FwdIt1 _Last, _FwdIt2 _Dest, _Pr _Pred) noexcept /* terminates */ {
    // copy compressing pairs that match
    // not parallelized at present, parallelism expected to be feasible in a future release
    _REQUIRE_PARALLEL_ITERATOR(_FwdIt1);
    _REQUIRE_CPP17_MUTABLE_ITERATOR(_FwdIt2);
    return _STD unique_copy(_First, _Last, _Dest, _Pass_fn(_Pred));
}

_EXPORT_STD template <class _ExPo, class _FwdIt1, class _FwdIt2, _Enable_if_execution_policy_t<_ExPo> = 0>
_FwdIt2 unique_copy(_ExPo&&, _FwdIt1 _First, _FwdIt1 _Last, _FwdIt2 _Dest) noexcept /* terminates */ {
    // copy compressing pairs that match
    // not parallelized at present, parallelism expected to be feasible in a future release
    _REQUIRE_PARALLEL_ITERATOR(_FwdIt1);
    _REQUIRE_CPP17_MUTABLE_ITERATOR(_FwdIt2);
    return _STD unique_copy(_First, _Last, _Dest);
}
#endif // _HAS_CXX17

#ifdef __cpp_lib_concepts
namespace ranges {
    _EXPORT_STD template <class _In, class _Out>
        using unique_copy_result = in_out_result<_In, _Out>;

    template <class _It, class _Ty>
    concept _Is_input_with_value_type = input_iterator<_It> && same_as<iter_value_t<_It>, _Ty>;

    template <class _It, class _Out>
    concept _Can_reread_or_store = forward_iterator<_It> //
        || _Is_input_with_value_type<_Out, iter_value_t<_It>> //
        || indirectly_copyable_storable<_It, _Out>;

    class _Unique_copy_fn : private _Not_quite_object {
    public:
        using _Not_quite_object::_Not_quite_object;

        template <input_iterator _It, sentinel_for<_It> _Se, weakly_incrementable _Out, class _Pj = identity,
            indirect_equivalence_relation<projected<_It, _Pj>> _Pr = ranges::equal_to>
            requires indirectly_copyable<_It, _Out>&& _Can_reread_or_store<_It, _Out>
        constexpr unique_copy_result<_It, _Out> operator()(
            _It _First, _Se _Last, _Out _Result, _Pr _Pred = {}, _Pj _Proj = {}) const {
            _Adl_verify_range(_First, _Last);
            auto _UResult =
                _Unique_copy_unchecked(_Unwrap_iter<_Se>(_STD move(_First)), _Unwrap_sent<_It>(_STD move(_Last)),
                    _Get_unwrapped_unverified(_STD move(_Result)), _Pass_fn(_Pred), _Pass_fn(_Proj));

            _Seek_wrapped(_First, _STD move(_UResult.in));
            _Seek_wrapped(_Result, _STD move(_UResult.out));
            return { _STD move(_First), _STD move(_Result) };
        }

        template <input_range _Rng, weakly_incrementable _Out, class _Pj = identity,
            indirect_equivalence_relation<projected<iterator_t<_Rng>, _Pj>> _Pr = ranges::equal_to>
            requires indirectly_copyable<iterator_t<_Rng>, _Out>&& _Can_reread_or_store<iterator_t<_Rng>, _Out>
        constexpr unique_copy_result<borrowed_iterator_t<_Rng>, _Out> operator()(
            _Rng&& _Range, _Out _Result, _Pr _Pred = {}, _Pj _Proj = {}) const {
            auto _First = _RANGES begin(_Range);
            auto _UResult = _Unique_copy_unchecked(_Unwrap_range_iter<_Rng>(_STD move(_First)), _Uend(_Range),
                _Get_unwrapped_unverified(_STD move(_Result)), _Pass_fn(_Pred), _Pass_fn(_Proj));

            _Seek_wrapped(_First, _STD move(_UResult.in));
            _Seek_wrapped(_Result, _STD move(_UResult.out));
            return { _STD move(_First), _STD move(_Result) };
        }

    private:
        template <class _It, class _Se, class _Out, class _Pj, class _Pr>
        _NODISCARD static constexpr unique_copy_result<_It, _Out> _Unique_copy_unchecked(
            _It _First, const _Se _Last, _Out _Result, _Pr _Pred, _Pj _Proj) {
            // Copy elements from [_First, _Last) to _Result, compressing adjacent elements whose projections satisfy
            // _Pred
            _STL_INTERNAL_STATIC_ASSERT(input_iterator<_It>);
            _STL_INTERNAL_STATIC_ASSERT(sentinel_for<_Se, _It>);
            _STL_INTERNAL_STATIC_ASSERT(weakly_incrementable<_Out>);
            _STL_INTERNAL_STATIC_ASSERT(indirect_equivalence_relation<_Pr, projected<_It, _Pj>>);
            _STL_INTERNAL_STATIC_ASSERT(indirectly_copyable<_It, _Out>);
            _STL_INTERNAL_STATIC_ASSERT(_Can_reread_or_store<_It, _Out>);

            if (_First == _Last) {
                return { _STD move(_First), _STD move(_Result) };
            }

            if constexpr (_Is_input_with_value_type<_Out, iter_value_t<_It>>) {
                // Can reread _Result
                *_Result = *_First;

                while (++_First != _Last) {
                    if (!_STD invoke(_Pred, _STD invoke(_Proj, *_Result), _STD invoke(_Proj, *_First))) {
                        ++_Result;
                        *_Result = *_First;
                    }
                }
            }
            else if constexpr (forward_iterator<_It>) {
                // Can reread _First
                auto _Current = _First;
                *_Result = *_First;

                while (++_First != _Last) {
                    if (!_STD invoke(_Pred, _STD invoke(_Proj, *_Current), _STD invoke(_Proj, *_First))) {
                        _Current = _First;
                        ++_Result;
                        *_Result = *_First;
                    }
                }
            }
            else {
                // Neither _First nor _Result can be reread, construct temporary
                iter_value_t<_It> _Val = *_First;

                while (++_First != _Last) {
                    if (!_STD invoke(_Pred, _STD invoke(_Proj, _Val), _STD invoke(_Proj, *_First))) {
                        *_Result = _STD move(_Val);
                        ++_Result;
                        _Val = *_First;
                    }
                }
                *_Result = _STD move(_Val);
            }
            ++_Result;

            return { _STD move(_First), _STD move(_Result) };
        }
    };

    _EXPORT_STD inline constexpr _Unique_copy_fn unique_copy{ _Not_quite_object::_Construct_tag{} };

    // concept-constrained for strict enforcement as it is used by several algorithms
    template <bidirectional_iterator _It>
        requires permutable<_It>
    constexpr void _Reverse_common(_It _First, _It _Last) {
#if _USE_STD_VECTOR_ALGORITHMS
        if constexpr (contiguous_iterator<_It>) {
            using _Elem = remove_reference_t<iter_reference_t<_It>>;
            constexpr size_t _Nx = sizeof(_Elem);
            constexpr bool _Allow_vectorization =
                conjunction_v<_Is_trivially_swappable<_Elem>, negation<is_volatile<_Elem>>>;

            if constexpr (_Allow_vectorization && _Nx <= 8 && (_Nx & (_Nx - 1)) == 0) {
                if (!_STD is_constant_evaluated()) {
                    _Elem* const _First_addr = _STD to_address(_First);
                    _Elem* const _Last_addr = _STD to_address(_Last);
                    if constexpr (_Nx == 1) {
                        __std_reverse_trivially_swappable_1(_First_addr, _Last_addr);
                    }
                    else if constexpr (_Nx == 2) {
                        __std_reverse_trivially_swappable_2(_First_addr, _Last_addr);
                    }
                    else if constexpr (_Nx == 4) {
                        __std_reverse_trivially_swappable_4(_First_addr, _Last_addr);
                    }
                    else {
                        __std_reverse_trivially_swappable_8(_First_addr, _Last_addr);
                    }

                    return;
                }
            }
        }
#endif // _USE_STD_VECTOR_ALGORITHMS

        for (; _First != _Last && _First != --_Last; ++_First) {
            _RANGES iter_swap(_First, _Last);
        }
    }

    class _Reverse_fn : private _Not_quite_object {
    public:
        using _Not_quite_object::_Not_quite_object;

        template <bidirectional_iterator _It, sentinel_for<_It> _Se>
            requires permutable<_It>
        constexpr _It operator()(_It _First, _Se _Last) const {
            _Adl_verify_range(_First, _Last);
            auto _UFirst = _Unwrap_iter<_Se>(_STD move(_First));
            auto _ULast = _Get_final_iterator_unwrapped<_It>(_UFirst, _STD move(_Last));
            _Seek_wrapped(_First, _ULast);
            _Reverse_common(_STD move(_UFirst), _STD move(_ULast));
            return _First;
        }

        template <bidirectional_range _Rng>
            requires permutable<iterator_t<_Rng>>
        constexpr borrowed_iterator_t<_Rng> operator()(_Rng&& _Range) const {
            auto _ULast = _Get_final_iterator_unwrapped(_Range);
            _Reverse_common(_Ubegin(_Range), _ULast);
            return _Rewrap_iterator(_Range, _STD move(_ULast));
        }
    };

    _EXPORT_STD inline constexpr _Reverse_fn reverse{ _Not_quite_object::_Construct_tag{} };
} // namespace ranges
#endif // __cpp_lib_concepts

_EXPORT_STD template <class _BidIt, class _OutIt>
_CONSTEXPR20 _OutIt reverse_copy(_BidIt _First, _BidIt _Last, _OutIt _Dest) {
    // copy reversing elements in [_First, _Last)
    _Adl_verify_range(_First, _Last);
    const auto _UFirst = _Get_unwrapped(_First);
    auto _ULast = _Get_unwrapped(_Last);
    auto _UDest = _Get_unwrapped_n(_Dest, _Idl_distance<_BidIt>(_UFirst, _ULast));

#if _USE_STD_VECTOR_ALGORITHMS
    using _Elem = remove_reference_t<_Iter_ref_t<remove_const_t<decltype(_UFirst)>>>;
    using _DestElem = remove_reference_t<_Iter_ref_t<decltype(_UDest)>>;
    constexpr bool _Allow_vectorization = conjunction_v<is_same<remove_const_t<_Elem>, _DestElem>,
        bool_constant<_Iterators_are_contiguous<decltype(_ULast), decltype(_UDest)>>, is_trivially_copyable<_Elem>,
        negation<is_volatile<_Elem>>>;
    constexpr size_t _Nx = sizeof(_Elem);

    if constexpr (_Allow_vectorization && _Nx <= 8 && (_Nx & (_Nx - 1)) == 0) {
#if _HAS_CXX20
        if (!_STD is_constant_evaluated())
#endif // _HAS_CXX20
        {
            if constexpr (_Nx == 1) {
                __std_reverse_copy_trivially_copyable_1(_To_address(_UFirst), _To_address(_ULast), _To_address(_UDest));
            }
            else if constexpr (_Nx == 2) {
                __std_reverse_copy_trivially_copyable_2(_To_address(_UFirst), _To_address(_ULast), _To_address(_UDest));
            }
            else if constexpr (_Nx == 4) {
                __std_reverse_copy_trivially_copyable_4(_To_address(_UFirst), _To_address(_ULast), _To_address(_UDest));
            }
            else {
                __std_reverse_copy_trivially_copyable_8(_To_address(_UFirst), _To_address(_ULast), _To_address(_UDest));
            }

            _UDest += _ULast - _UFirst;
            _Seek_wrapped(_Dest, _UDest);
            return _Dest;
        }
    }
#endif // _USE_STD_VECTOR_ALGORITHMS

    for (; _UFirst != _ULast; ++_UDest) {
        *_UDest = *--_ULast;
    }

    _Seek_wrapped(_Dest, _UDest);
    return _Dest;
}

#if _HAS_CXX17
_EXPORT_STD template <class _ExPo, class _BidIt, class _FwdIt, _Enable_if_execution_policy_t<_ExPo> = 0>
_FwdIt reverse_copy(_ExPo&&, _BidIt _First, _BidIt _Last, _FwdIt _Dest) noexcept /* terminates */ {
    // copy reversing elements in [_First, _Last)
    // not parallelized as benchmarks show it isn't worth it
    _REQUIRE_PARALLEL_ITERATOR(_BidIt);
    _REQUIRE_CPP17_MUTABLE_ITERATOR(_FwdIt);
    return _STD reverse_copy(_First, _Last, _Dest);
}

#ifdef __cpp_lib_concepts
namespace ranges {
    _EXPORT_STD template <class _In, class _Out>
        using reverse_copy_result = in_out_result<_In, _Out>;

    class _Reverse_copy_fn : private _Not_quite_object {
    public:
        using _Not_quite_object::_Not_quite_object;

        template <bidirectional_iterator _It, sentinel_for<_It> _Se, weakly_incrementable _Out>
            requires indirectly_copyable<_It, _Out>
        constexpr reverse_copy_result<_It, _Out> operator()(_It _First, _Se _Last, _Out _Result) const {
            _Adl_verify_range(_First, _Last);
            auto _UFirst = _Unwrap_iter<_Se>(_STD move(_First));
            auto _ULast = _Get_final_iterator_unwrapped<_It>(_UFirst, _STD move(_Last));
            _Seek_wrapped(_First, _ULast);
            _Result = _Reverse_copy_common(_STD move(_UFirst), _STD move(_ULast), _STD move(_Result));
            return { _STD move(_First), _STD move(_Result) };
        }

        template <bidirectional_range _Rng, weakly_incrementable _Out>
            requires indirectly_copyable<iterator_t<_Rng>, _Out>
        constexpr reverse_copy_result<borrowed_iterator_t<_Rng>, _Out> operator()(_Rng&& _Range, _Out _Result) const {
            if constexpr (common_range<_Rng>) {
                _Result = _Reverse_copy_common(_Ubegin(_Range), _Uend(_Range), _STD move(_Result));
                return { _RANGES end(_Range), _STD move(_Result) };
            }
            else {
                auto _ULast = _Get_final_iterator_unwrapped(_Range);
                _Result = _Reverse_copy_common(_Ubegin(_Range), _ULast, _STD move(_Result));
                return { _Rewrap_iterator(_Range, _STD move(_ULast)), _STD move(_Result) };
            }
        }

    private:
        template <class _It, class _Out>
        _NODISCARD static constexpr _Out _Reverse_copy_common(const _It _First, _It _Last, _Out _Result) {
            _STL_INTERNAL_STATIC_ASSERT(bidirectional_iterator<_It>);
            _STL_INTERNAL_STATIC_ASSERT(weakly_incrementable<_Out>);
            _STL_INTERNAL_STATIC_ASSERT(indirectly_copyable<_It, _Out>);

#if _USE_STD_VECTOR_ALGORITHMS
            if constexpr (contiguous_iterator<_It> && contiguous_iterator<_Out>) {
                using _Elem = remove_reference_t<iter_reference_t<_It>>;
                using _DestElem = remove_reference_t<iter_reference_t<_Out>>;
                constexpr bool _Allow_vectorization = conjunction_v<is_same<remove_const_t<_Elem>, _DestElem>,
                    is_trivially_copyable<_Elem>, negation<is_volatile<_Elem>>>;
                constexpr size_t _Nx = sizeof(_Elem);

                if constexpr (_Allow_vectorization && _Nx <= 8 && (_Nx & (_Nx - 1)) == 0) {
                    if (!_STD is_constant_evaluated()) {
                        _Elem* const _First_addr = _STD to_address(_First);
                        _Elem* const _Last_addr = _STD to_address(_Last);
                        _DestElem* const _Result_addr = _STD to_address(_Result);
                        if constexpr (_Nx == 1) {
                            __std_reverse_copy_trivially_copyable_1(_First_addr, _Last_addr, _Result_addr);
                        }
                        else if constexpr (_Nx == 2) {
                            __std_reverse_copy_trivially_copyable_2(_First_addr, _Last_addr, _Result_addr);
                        }
                        else if constexpr (_Nx == 4) {
                            __std_reverse_copy_trivially_copyable_4(_First_addr, _Last_addr, _Result_addr);
                        }
                        else {
                            __std_reverse_copy_trivially_copyable_8(_First_addr, _Last_addr, _Result_addr);
                        }

                        _Result += _Last - _First;
                        return _Result;
                    }
                }
            }
#endif // _USE_STD_VECTOR_ALGORITHMS

            for (; _First != _Last; ++_Result) {
                *_Result = *--_Last;
            }

            return _Result;
        }
    };

    _EXPORT_STD inline constexpr _Reverse_copy_fn reverse_copy{ _Not_quite_object::_Construct_tag{} };
} // namespace ranges
#endif // __cpp_lib_concepts
#endif // _HAS_CXX17

#ifdef __cpp_lib_concepts
namespace ranges {
    template <permutable _It>
    _NODISCARD constexpr subrange<_It> _Reverse_until_mid_unchecked(_It _First, const _It _Mid, _It _Last) {
        // reverse until either _First or _Last hits _Mid
        _STL_INTERNAL_CHECK(_First != _Mid);
        _STL_INTERNAL_CHECK(_Mid != _Last);

        do {
            _RANGES iter_swap(_First, --_Last);
        } while (++_First != _Mid && _Last != _Mid);

        return { _STD move(_First), _STD move(_Last) };
    }

    template <permutable _It, sentinel_for<_It> _Se>
    _NODISCARD constexpr subrange<_It> _Rotate_unchecked(_It _First, _It _Mid, _Se _Last) {
        // Exchange the ranges [_First, _Mid) and [_Mid, _Last)
        // that is, rotates [_First, _Last) left by distance(_First, _Mid) positions

        if (_First == _Mid) {
            auto _Final = _Get_final_iterator_unwrapped<_It>(_Mid, _STD move(_Last));
            return { _Final, _Final };
        }

        if (_Mid == _Last) {
            return { _STD move(_First), _STD move(_Mid) };
        }

        if constexpr (bidirectional_iterator<_It>) {
            _Reverse_common(_First, _Mid);
            auto _Final = _Get_final_iterator_unwrapped<_It>(_Mid, _STD move(_Last));
            _Reverse_common(_Mid, _Final);

            if constexpr (random_access_iterator<_It>) {
                _Reverse_common(_First, _Final);
                _First += _Final - _Mid;

                return { _STD move(_First), _STD move(_Final) };
            }
            else {
                const auto _Result = _RANGES _Reverse_until_mid_unchecked(_STD move(_First), _Mid, _Final);
                auto _Mid_first = _Result.begin();
                auto _Mid_last = _Result.end();
                _Reverse_common(_Mid_first, _Mid_last);

                if (_Mid_first == _Mid) {
                    return { _STD move(_Mid_last), _STD move(_Final) };
                }
                else {
                    return { _STD move(_Mid_first), _STD move(_Final) };
                }
            }
        }
        else {
            auto _Next = _Mid;
            do { // rotate the first cycle
                _RANGES iter_swap(_First, _Next);
                ++_First;
                ++_Next;
                if (_First == _Mid) {
                    _Mid = _Next;
                }
            } while (_Next != _Last);

            auto _Begin = _First;

            while (_Mid != _Last) { // rotate subsequent cycles
                _Next = _Mid;
                do {
                    _RANGES iter_swap(_First, _Next);
                    ++_First;
                    ++_Next;
                    if (_First == _Mid) {
                        _Mid = _Next;
                    }
                } while (_Next != _Last);
            }
            return { _STD move(_Begin), _STD move(_Mid) };
        }
    }

    class _Rotate_fn : private _Not_quite_object {
    public:
        using _Not_quite_object::_Not_quite_object;

        template <permutable _It, sentinel_for<_It> _Se>
        constexpr subrange<_It> operator()(_It _First, _It _Mid, _Se _Last) const {
            _Adl_verify_range(_First, _Mid);
            _Adl_verify_range(_Mid, _Last);
            auto _UResult = _RANGES _Rotate_unchecked(_Unwrap_iter<_Se>(_STD move(_First)),
                _Unwrap_iter<_Se>(_STD move(_Mid)), _Unwrap_sent<_It>(_STD move(_Last)));

            return _Rewrap_subrange<subrange<_It>>(_First, _STD move(_UResult));
        }

        template <forward_range _Rng>
            requires permutable<iterator_t<_Rng>>
        constexpr borrowed_subrange_t<_Rng> operator()(_Rng&& _Range, iterator_t<_Rng> _Mid) const {
            _Adl_verify_range(_RANGES begin(_Range), _Mid);
            _Adl_verify_range(_Mid, _RANGES end(_Range));
            auto _UResult =
                _RANGES _Rotate_unchecked(_Ubegin(_Range), _Unwrap_range_iter<_Rng>(_STD move(_Mid)), _Uend(_Range));

            return _Rewrap_subrange<borrowed_subrange_t<_Rng>>(_Mid, _STD move(_UResult));
        }
    };

    _EXPORT_STD inline constexpr _Rotate_fn rotate{ _Not_quite_object::_Construct_tag{} };
} // namespace ranges
#endif // __cpp_lib_concepts

_EXPORT_STD template <class _FwdIt, class _OutIt>
_CONSTEXPR20 _OutIt rotate_copy(_FwdIt _First, _FwdIt _Mid, _FwdIt _Last, _OutIt _Dest) {
    // copy rotating [_First, _Last)
    _Adl_verify_range(_First, _Mid);
    _Adl_verify_range(_Mid, _Last);
    const auto _UFirst = _Get_unwrapped(_First);
    const auto _UMid = _Get_unwrapped(_Mid);
    const auto _ULast = _Get_unwrapped(_Last);
    auto _UDest = _Get_unwrapped_n(_Dest, _Idl_distance<_FwdIt>(_UFirst, _ULast));
    _UDest = _STD _Copy_unchecked(_UMid, _ULast, _UDest);
    _Seek_wrapped(_Dest, _STD _Copy_unchecked(_UFirst, _UMid, _UDest));
    return _Dest;
}

#if _HAS_CXX17
_EXPORT_STD template <class _ExPo, class _FwdIt1, class _FwdIt2, _Enable_if_execution_policy_t<_ExPo> = 0>
_FwdIt2 rotate_copy(_ExPo&&, _FwdIt1 _First, _FwdIt1 _Mid, _FwdIt1 _Last, _FwdIt2 _Dest) noexcept /* terminates */ {
    // copy rotating [_First, _Last)
    // not parallelized as benchmarks show it isn't worth it
    _REQUIRE_PARALLEL_ITERATOR(_FwdIt1);
    _REQUIRE_CPP17_MUTABLE_ITERATOR(_FwdIt2);
    return _STD rotate_copy(_First, _Mid, _Last, _Dest);
}

#ifdef __cpp_lib_concepts
namespace ranges {
    _EXPORT_STD template <class _In, class _Out>
        using rotate_copy_result = in_out_result<_In, _Out>;

    class _Rotate_copy_fn : private _Not_quite_object {
    public:
        using _Not_quite_object::_Not_quite_object;

        template <forward_iterator _It, sentinel_for<_It> _Se, weakly_incrementable _Out>
            requires indirectly_copyable<_It, _Out>
        constexpr rotate_copy_result<_It, _Out> operator()(_It _First, _It _Mid, _Se _Last, _Out _Result) const {
            _Adl_verify_range(_First, _Mid);
            _Adl_verify_range(_Mid, _Last);
            auto _UResult = _Rotate_copy_unchecked(_Unwrap_iter<_Se>(_STD move(_First)),
                _Unwrap_iter<_Se>(_STD move(_Mid)), _Unwrap_sent<_It>(_STD move(_Last)), _STD move(_Result));

            _Seek_wrapped(_First, _STD move(_UResult.in));
            return { _STD move(_First), _STD move(_UResult.out) };
        }

        template <forward_range _Rng, weakly_incrementable _Out>
            requires indirectly_copyable<iterator_t<_Rng>, _Out>
        constexpr rotate_copy_result<borrowed_iterator_t<_Rng>, _Out> operator()(
            _Rng&& _Range, iterator_t<_Rng> _Mid, _Out _Result) const {
            _Adl_verify_range(_RANGES begin(_Range), _Mid);
            _Adl_verify_range(_Mid, _RANGES end(_Range));
            auto _UResult = _Rotate_copy_unchecked(
                _Ubegin(_Range), _Unwrap_range_iter<_Rng>(_STD move(_Mid)), _Uend(_Range), _STD move(_Result));

            return { _Rewrap_iterator(_Range, _STD move(_UResult.in)), _STD move(_UResult.out) };
        }

    private:
        template <class _It, class _Se, class _Out>
        _NODISCARD static constexpr rotate_copy_result<_It, _Out> _Rotate_copy_unchecked(
            _It _First, _It _Mid, _Se _Last, _Out _Result) {
            // Copy the content of [_Mid, _Last) and [_First, _Mid) to _Result
            _STL_INTERNAL_STATIC_ASSERT(forward_iterator<_It>);
            _STL_INTERNAL_STATIC_ASSERT(sentinel_for<_Se, _It>);
            _STL_INTERNAL_STATIC_ASSERT(weakly_incrementable<_Out>);
            _STL_INTERNAL_STATIC_ASSERT(indirectly_copyable<_It, _Out>);

            auto _UResult1 = _RANGES _Copy_unchecked(_Mid, _STD move(_Last), _STD move(_Result));
            auto _UResult2 = _RANGES _Copy_unchecked(_STD move(_First), _STD move(_Mid), _STD move(_UResult1.out));
            return { _STD move(_UResult1.in), _STD move(_UResult2.out) };
        }
    };

    _EXPORT_STD inline constexpr _Rotate_copy_fn rotate_copy{ _Not_quite_object::_Construct_tag{} };
} // namespace ranges
#endif // __cpp_lib_concepts
#endif // _HAS_CXX17

template <class _Diff, class _Urng>
class _Rng_from_urng { // wrap a URNG as an RNG
public:
    using _Ty0 = make_unsigned_t<_Diff>;
    using _Ty1 = _Invoke_result_t<_Urng&>;

    using _Udiff = conditional_t < sizeof(_Ty1) < sizeof(_Ty0), _Ty0, _Ty1 > ;

    explicit _Rng_from_urng(_Urng& _Func)
        : _Ref(_Func), _Bits(CHAR_BIT * sizeof(_Udiff)), _Bmask(static_cast<_Udiff>(-1)) {
        for (; static_cast<_Udiff>((_Urng::max)() - (_Urng::min)()) < _Bmask; _Bmask >>= 1) {
            --_Bits;
        }
    }

    _Diff operator()(_Diff _Index) { // adapt _Urng closed range to [0, _Index)
        for (;;) { // try a sample random value
            _Udiff _Ret = 0; // random bits
            _Udiff _Mask = 0; // 2^N - 1, _Ret is within [0, _Mask]

            while (_Mask < static_cast<_Udiff>(_Index - 1)) { // need more random bits
                _Ret <<= _Bits - 1; // avoid full shift
                _Ret <<= 1;
                _Ret |= _Get_bits();
                _Mask <<= _Bits - 1; // avoid full shift
                _Mask <<= 1;
                _Mask |= _Bmask;
            }

            // _Ret is [0, _Mask], _Index - 1 <= _Mask, return if unbiased
            if (_Ret / _Index < _Mask / _Index || _Mask % _Index == static_cast<_Udiff>(_Index - 1)) {
                return static_cast<_Diff>(_Ret % _Index);
            }
        }
    }

    _Udiff _Get_all_bits() {
        _Udiff _Ret = 0;

        for (size_t _Num = 0; _Num < CHAR_BIT * sizeof(_Udiff); _Num += _Bits) { // don't mask away any bits
            _Ret <<= _Bits - 1; // avoid full shift
            _Ret <<= 1;
            _Ret |= _Get_bits();
        }

        return _Ret;
    }

    _Rng_from_urng(const _Rng_from_urng&) = delete;
    _Rng_from_urng& operator=(const _Rng_from_urng&) = delete;

private:
    _Udiff _Get_bits() { // return a random value within [0, _Bmask]
        for (;;) { // repeat until random value is in range
            const _Udiff _Val = static_cast<_Udiff>(_Ref() - (_Urng::min)());

            if (_Val <= _Bmask) {
                return _Val;
            }
        }
    }

    _Urng& _Ref; // reference to URNG
    size_t _Bits; // number of random bits generated by _Get_bits()
    _Udiff _Bmask; // 2^_Bits - 1
};

#if _HAS_CXX17
template <class _PopIt, class _SampleIt, class _Diff, class _RngFn>
_SampleIt _Sample_reservoir_unchecked(
    _PopIt _First, const _PopIt _Last, const _SampleIt _Dest, const _Diff _Count, _RngFn& _RngFunc) {
    // source is input: use reservoir sampling (unstable)
    // pre: _SampleIt is random-access && 0 < _Count && the range [_Dest, _Dest + _Count) is valid
    using _Diff_sample = _Iter_diff_t<_SampleIt>;
    const auto _SCount = static_cast<_Diff_sample>(_Count);
    _Iter_diff_t<_PopIt> _Pop_size{};
    for (; _Pop_size < _SCount; ++_Pop_size, (void) ++_First) {
        // _Pop_size is less than _SCount, and [_Dest, _Dest + _SCount) is valid,
        // so [_Dest, _Dest + _Pop_size) must be valid, so narrowing to _Diff_sample
        // can't overflow
        const auto _Sample_pop = static_cast<_Diff_sample>(_Pop_size);
        if (_First == _Last) {
            return _Dest + _Sample_pop;
        }

        *(_Dest + _Sample_pop) = *_First;
    }
    for (; _First != _Last; ++_First) {
        const auto _Idx = _RngFunc(++_Pop_size);
        if (_Idx < _SCount) {
            *(_Dest + static_cast<_Diff_sample>(_Idx)) = *_First; // again, valid narrowing because _Idx < _SCount
        }
    }
    return _Dest + _SCount;
}

template <class _PopIt, class _SampleIt, class _Diff, class _RngFn>
_SampleIt _Sample_selection_unchecked(
    _PopIt _First, _Iter_diff_t<_PopIt> _Pop_size, _SampleIt _Dest, _Diff _Count, _RngFn& _RngFunc) {
    // source is forward *and* we know the source range size: use selection sampling (stable)
    // pre: _PopIt is forward && _Count <= _Pop_size
    using _CT = common_type_t<_Iter_diff_t<_PopIt>, _Diff>;
    for (; _Pop_size > 0; ++_First, (void) --_Pop_size) {
        if (static_cast<_CT>(_RngFunc(_Pop_size)) < static_cast<_CT>(_Count)) {
            --_Count;
            *_Dest = *_First;
            ++_Dest;
        }
    }
    return _Dest;
}

_EXPORT_STD template <class _PopIt, class _SampleIt, class _Diff, class _Urng>
_SampleIt sample(_PopIt _First, _PopIt _Last, _SampleIt _Dest, _Diff _Count, _Urng&& _Func) {
    // randomly select _Count elements from [_First, _Last) into _Dest
    static_assert(_Is_ranges_fwd_iter_v<_PopIt> || _Is_cpp17_random_iter_v<_SampleIt>,
        "If the source range is not forward, the destination range must be a Cpp17RandomAccessIterator.");

    static_assert(is_integral_v<_Diff>, "The sample size must have an integer type.");
    _Adl_verify_range(_First, _Last);
    if (0 < _Count) {
        auto _UFirst = _Get_unwrapped(_First);
        auto _ULast = _Get_unwrapped(_Last);
        using _PopDiff = _Iter_diff_t<_PopIt>;
        _Rng_from_urng<_PopDiff, remove_reference_t<_Urng>> _RngFunc(_Func);
        if constexpr (_Is_ranges_fwd_iter_v<_PopIt>) {
            // source is forward: use selection sampling (stable)
            using _CT = common_type_t<_Diff, _PopDiff>;
            const auto _Pop_size = _STD distance(_UFirst, _ULast);
            if (static_cast<_CT>(_Count) > static_cast<_CT>(_Pop_size)) {
                _Count = static_cast<_Diff>(_Pop_size); // narrowing OK because _Count is getting smaller
            }

            _Seek_wrapped(_Dest,
                _Sample_selection_unchecked(_UFirst, _Pop_size, _Get_unwrapped_n(_Dest, _Count), _Count, _RngFunc));
        }
        else {
            static_assert(_Is_ranges_input_iter_v<_PopIt>, "Source iterators must be at least input iterators");
            // source is input: use reservoir sampling (unstable)
            _Seek_wrapped(_Dest,
                _Sample_reservoir_unchecked(_UFirst, _ULast, _Get_unwrapped_unverified(_Dest), _Count, _RngFunc));
        }
    }

    return _Dest;
}

#ifdef __cpp_lib_concepts
// clang-format off
_EXPORT_STD template <class _Ty>
concept uniform_random_bit_generator = invocable<_Ty&>
&& unsigned_integral<invoke_result_t<_Ty&>>
&& requires {
    { (_Ty::min)() } -> same_as<invoke_result_t<_Ty&>>;
    { (_Ty::max)() } -> same_as<invoke_result_t<_Ty&>>;
        requires bool_constant < (_Ty::min)() < (_Ty::max)() > ::value;
};
// clang-format on

namespace ranges {
    class _Sample_fn : private _Not_quite_object {
    public:
        using _Not_quite_object::_Not_quite_object;

        template <input_iterator _It, sentinel_for<_It> _Se, weakly_incrementable _Out, class _Urng>
            requires (forward_iterator<_It> || random_access_iterator<_Out>)
        && indirectly_copyable<_It, _Out>&& uniform_random_bit_generator<remove_reference_t<_Urng>>
            _Out operator()(_It _First, _Se _Last, _Out _Result, iter_difference_t<_It> _Count, _Urng&& _Func) const {
            _Adl_verify_range(_First, _Last);
            if (_Count <= 0) {
                return _Result;
            }

            _Rng_from_urng<iter_difference_t<_It>, remove_reference_t<_Urng>> _RngFunc(_Func);
            if constexpr (forward_iterator<_It>) {
                auto _UFirst = _Unwrap_iter<_Se>(_STD move(_First));
                auto _Pop_size = _RANGES distance(_UFirst, _Unwrap_sent<_It>(_STD move(_Last)));
                return _Sample_selection_unchecked(_STD move(_UFirst), _Pop_size, _STD move(_Result), _Count, _RngFunc);
            }
            else {
                return _Sample_reservoir_unchecked(_Unwrap_iter<_Se>(_STD move(_First)),
                    _Unwrap_sent<_It>(_STD move(_Last)), _STD move(_Result), _Count, _RngFunc);
            }
        }

        template <input_range _Rng, weakly_incrementable _Out, class _Urng>
            requires (forward_range<_Rng> || random_access_iterator<_Out>)
        && indirectly_copyable<iterator_t<_Rng>, _Out>
            && uniform_random_bit_generator<remove_reference_t<_Urng>>
            _Out operator()(_Rng&& _Range, _Out _Result, range_difference_t<_Rng> _Count, _Urng&& _Func) const {
            if (_Count <= 0) {
                return _Result;
            }

            _Rng_from_urng<range_difference_t<_Rng>, remove_reference_t<_Urng>> _RngFunc(_Func);
            if constexpr (forward_range<_Rng>) {
                auto _UFirst = _Ubegin(_Range);
                auto _Pop_size = _RANGES distance(_UFirst, _Uend(_Range));
                return _Sample_selection_unchecked(_STD move(_UFirst), _Pop_size, _STD move(_Result), _Count, _RngFunc);
            }
            else {
                return _Sample_reservoir_unchecked(
                    _Ubegin(_Range), _Uend(_Range), _STD move(_Result), _Count, _RngFunc);
            }
        }

    private:
        template <class _It, class _Out, class _Rng>
        _NODISCARD static _Out _Sample_selection_unchecked(
            _It _First, iter_difference_t<_It> _Pop_size, _Out _Result, iter_difference_t<_It> _Count, _Rng& _RngFunc) {
            // randomly select _Count elements from [_First, _First + _Pop_size) into _Result
            _STL_INTERNAL_STATIC_ASSERT(forward_iterator<_It>);
            _STL_INTERNAL_STATIC_ASSERT(weakly_incrementable<_Out>);
            _STL_INTERNAL_STATIC_ASSERT(indirectly_copyable<_It, _Out>);

            if (_Count > _Pop_size) {
                _Count = _Pop_size;
            }

            for (; _Pop_size > 0; ++_First, (void) --_Pop_size) {
                if (_RngFunc(_Pop_size) < _Count) {
                    *_Result = *_First;
                    ++_Result;
                    if (--_Count == 0) {
                        break;
                    }
                }
            }

            return _Result;
        }

        template <class _It, class _Se, class _Out, class _Rng>
        _NODISCARD static _Out _Sample_reservoir_unchecked(
            _It _First, const _Se _Last, _Out _Result, const iter_difference_t<_It> _Count, _Rng& _RngFunc) {
            // randomly select _Count elements from [_First, _Last) into _Result
            _STL_INTERNAL_STATIC_ASSERT(input_iterator<_It>);
            _STL_INTERNAL_STATIC_ASSERT(sentinel_for<_Se, _It>);
            _STL_INTERNAL_STATIC_ASSERT(random_access_iterator<_Out>);
            _STL_INTERNAL_STATIC_ASSERT(indirectly_copyable<_It, _Out>);

            iter_difference_t<_It> _Pop_size{};
            for (; _Pop_size < _Count; ++_Pop_size, (void) ++_First) {
                if (_First == _Last) {
                    return _Result + _Pop_size;
                }

                *(_Result + _Pop_size) = *_First;
            }
            for (; _First != _Last; ++_First) {
                const auto _Idx = _RngFunc(++_Pop_size);
                if (_Idx < _Count) {
                    *(_Result + _Idx) = *_First;
                }
            }

            return _Result + _Count;
        }
    };

    _EXPORT_STD inline constexpr _Sample_fn sample{ _Not_quite_object::_Construct_tag{} };
} // namespace ranges
#endif // __cpp_lib_concepts
#endif // _HAS_CXX17

template <class _RanIt, class _RngFn>
void _Random_shuffle1(_RanIt _First, _RanIt _Last, _RngFn& _RngFunc) {
    // shuffle [_First, _Last) using random function _RngFunc
    _Adl_verify_range(_First, _Last);
    auto _UFirst = _Get_unwrapped(_First);
    const auto _ULast = _Get_unwrapped(_Last);
    if (_UFirst == _ULast) {
        return;
    }

    using _Diff = _Iter_diff_t<_RanIt>;
    auto _UTarget = _UFirst;
    _Diff _Target_index = 1;
    for (; ++_UTarget != _ULast; ++_Target_index) { // randomly place an element from [_First, _Target] at _Target
        _Diff _Off = _RngFunc(static_cast<_Diff>(_Target_index + 1));
        _STL_ASSERT(0 <= _Off && _Off <= _Target_index, "random value out of range");
        if (_Off != _Target_index) { // avoid self-move-assignment
            swap(*_UTarget, *(_UFirst + _Off)); // intentional ADL
        }
    }
}

_EXPORT_STD template <class _RanIt, class _Urng>
void shuffle(_RanIt _First, _RanIt _Last, _Urng&& _Func) { // shuffle [_First, _Last) using URNG _Func
    using _Urng0 = remove_reference_t<_Urng>;
    _Rng_from_urng<_Iter_diff_t<_RanIt>, _Urng0> _RngFunc(_Func);
    _Random_shuffle1(_First, _Last, _RngFunc);
}

#ifdef __cpp_lib_concepts
namespace ranges {
    class _Shuffle_fn : private _Not_quite_object {
    public:
        using _Not_quite_object::_Not_quite_object;

        template <random_access_iterator _It, sentinel_for<_It> _Se, class _Urng>
            requires permutable<_It>&& uniform_random_bit_generator<remove_reference_t<_Urng>>
        _It operator()(_It _First, _Se _Last, _Urng&& _Func) const {
            _Adl_verify_range(_First, _Last);

            _Rng_from_urng<iter_difference_t<_It>, remove_reference_t<_Urng>> _RngFunc(_Func);
            auto _UResult =
                _Shuffle_unchecked(_Unwrap_iter<_Se>(_STD move(_First)), _Unwrap_sent<_It>(_STD move(_Last)), _RngFunc);

            _Seek_wrapped(_First, _STD move(_UResult));
            return _First;
        }

        template <random_access_range _Rng, class _Urng>
            requires permutable<iterator_t<_Rng>>&& uniform_random_bit_generator<remove_reference_t<_Urng>>
        borrowed_iterator_t<_Rng> operator()(_Rng&& _Range, _Urng&& _Func) const {
            _Rng_from_urng<range_difference_t<_Rng>, remove_reference_t<_Urng>> _RngFunc(_Func);

            return _Rewrap_iterator(_Range, _Shuffle_unchecked(_Ubegin(_Range), _Uend(_Range), _RngFunc));
        }

    private:
        template <class _It, class _Se, class _Rng>
        _NODISCARD static _It _Shuffle_unchecked(_It _First, const _Se _Last, _Rng& _Func) {
            // shuffle [_First, _Last) using random function _Func
            _STL_INTERNAL_STATIC_ASSERT(random_access_iterator<_It>);
            _STL_INTERNAL_STATIC_ASSERT(sentinel_for<_Se, _It>);
            _STL_INTERNAL_STATIC_ASSERT(permutable<_It>);

            if (_First == _Last) {
                return _First;
            }
            using _Diff = iter_difference_t<_It>;

            auto _Target = _First;
            _Diff _Target_index = 1;
            for (; ++_Target != _Last; ++_Target_index) {
                // randomly place an element from [_First, _Target] at _Target
                const _Diff _Off = _Func(_Target_index + 1);
                _STL_ASSERT(0 <= _Off && _Off <= _Target_index, "random value out of range");
                if (_Off != _Target_index) { // avoid self-move-assignment
                    _RANGES iter_swap(_Target, _First + _Off);
                }
            }
            return _Target;
        }
    };

    _EXPORT_STD inline constexpr _Shuffle_fn shuffle{ _Not_quite_object::_Construct_tag{} };
} // namespace ranges
#endif // __cpp_lib_concepts

#if _HAS_AUTO_PTR_ETC
_EXPORT_STD template <class _RanIt, class _RngFn>
void random_shuffle(_RanIt _First, _RanIt _Last, _RngFn&& _RngFunc) {
    // shuffle [_First, _Last) using random function _RngFunc
    _Random_shuffle1(_First, _Last, _RngFunc);
}

struct _Rand_urng_from_func { // wrap rand() as a URNG
    using result_type = unsigned int;

    static result_type(min)() { // return minimum possible generated value
        return 0;
    }

    static result_type(max)() { // return maximum possible generated value
        return RAND_MAX;
    }

    result_type operator()() { // invoke rand()
        return static_cast<result_type>(_CSTD rand());
    }
};

_EXPORT_STD template <class _RanIt>
void random_shuffle(_RanIt _First, _RanIt _Last) { // shuffle [_First, _Last) using rand()
    _Rand_urng_from_func _Func;
    _STD shuffle(_First, _Last, _Func);
}
#endif // _HAS_AUTO_PTR_ETC

#if _HAS_CXX20
_EXPORT_STD template <class _FwdIt>
constexpr _FwdIt shift_left(_FwdIt _First, const _FwdIt _Last, _Iter_diff_t<_FwdIt> _Pos_to_shift) {
    // shift [_First, _Last) left by _Pos_to_shift
    // positions; returns the end of the resulting range
    _STL_ASSERT(_Pos_to_shift >= 0, "shift count must be non-negative (N4950 [alg.shift]/1)");

    _Adl_verify_range(_First, _Last);

    if (_Pos_to_shift == 0) {
        return _Last;
    }

    const auto _UFirst = _Get_unwrapped(_First);
    const auto _ULast = _Get_unwrapped(_Last);
    auto _Start_at = _UFirst;

    if constexpr (_Is_cpp17_random_iter_v<_FwdIt>) {
        if (_Pos_to_shift >= _ULast - _UFirst) {
            return _First;
        }
        _Start_at += _Pos_to_shift;
    }
    else {
        for (; 0 < _Pos_to_shift; --_Pos_to_shift) {
            if (_Start_at == _ULast) {
                return _First;
            }
            ++_Start_at;
        }
    }

    _Seek_wrapped(_First, _STD _Move_unchecked(_Start_at, _ULast, _UFirst));
    return _First;
}

_EXPORT_STD template <class _ExPo, class _FwdIt, _Enable_if_execution_policy_t<_ExPo> = 0>
_FwdIt shift_left(_ExPo&&, _FwdIt _First, _FwdIt _Last, _Iter_diff_t<_FwdIt> _Pos_to_shift) noexcept /* terminates */ {
    // shift [_First, _Last) left by _Pos_to_shift positions
    // not parallelized as benchmarks show it isn't worth it
    return _STD shift_left(_First, _Last, _Pos_to_shift);
}

_EXPORT_STD template <class _FwdIt>
constexpr _FwdIt shift_right(_FwdIt _First, const _FwdIt _Last, _Iter_diff_t<_FwdIt> _Pos_to_shift) {
    // shift [_First, _Last) right by _Pos_to_shift
    // positions; returns the beginning of the resulting range
    _STL_ASSERT(_Pos_to_shift >= 0, "shift count must be non-negative (N4950 [alg.shift]/5)");

    _Adl_verify_range(_First, _Last);

    if (_Pos_to_shift == 0) {
        return _First;
    }

    const auto _UFirst = _Get_unwrapped(_First);
    const auto _ULast = _Get_unwrapped(_Last);

    if constexpr (_Is_cpp17_bidi_iter_v<_FwdIt>) {
        auto _UEnd_at = _ULast;
        if constexpr (_Is_cpp17_random_iter_v<_FwdIt>) {
            if (_Pos_to_shift >= _ULast - _UFirst) {
                return _Last;
            }
            _UEnd_at -= _Pos_to_shift;
        }
        else {
            for (; 0 < _Pos_to_shift; --_Pos_to_shift) {
                if (_UEnd_at == _UFirst) {
                    return _Last;
                }
                --_UEnd_at;
            }
        }

        _Seek_wrapped(_First, _Move_backward_unchecked(_UFirst, _UEnd_at, _ULast));
        return _First;
    }
    else {
        auto _UResult = _UFirst;

        for (; 0 < _Pos_to_shift; --_Pos_to_shift) {
            if (_UResult == _ULast) {
                return _Last;
            }
            ++_UResult;
        }
        _Seek_wrapped(_First, _UResult);

        auto _Trail = _UFirst;
        auto _Lead = _UResult;

        for (; _Trail != _UResult; ++_Trail, (void) ++_Lead) {
            if (_Lead == _ULast) {
                _STD _Move_unchecked(_UFirst, _Trail, _UResult);

                return _First;
            }
        }

        // Here, _Trail = _UFirst + original _Pos_to_shift
        // Here, _Lead = _UFirst + 2 * original _Pos_to_shift

        for (;;) {
            // This loop swaps the range [_UFirst, _UResult) with [_Trail, _Lead),
            // advancing _Trail and _Lead by _Pos_to_shift
            for (auto _Mid = _UFirst; _Mid != _UResult; ++_Mid, (void) ++_Trail, ++_Lead) {
                if (_Lead == _ULast) {
                    _Trail = _STD _Move_unchecked(_Mid, _UResult, _Trail);
                    _STD _Move_unchecked(_UFirst, _Mid, _Trail);

                    return _First;
                }
                swap(*_Mid, *_Trail); // intentional ADL
            }
        }
    }
}

_EXPORT_STD template <class _ExPo, class _FwdIt, _Enable_if_execution_policy_t<_ExPo> = 0>
_FwdIt shift_right(_ExPo&&, _FwdIt _First, _FwdIt _Last, _Iter_diff_t<_FwdIt> _Pos_to_shift) noexcept /* terminates */ {
    // shift [_First, _Last) right by _Pos_to_shift positions
    // not parallelized as benchmarks show it isn't worth it
    return _STD shift_right(_First, _Last, _Pos_to_shift);
}
#endif // _HAS_CXX20

#if _HAS_CXX23 && defined(__cpp_lib_concepts) // TRANSITION, GH-395
namespace ranges {
    class _Shift_left_fn : private _Not_quite_object {
    public:
        using _Not_quite_object::_Not_quite_object;

        template <permutable _It, sentinel_for<_It> _Se>
        constexpr subrange<_It> operator()(_It _First, const _Se _Last, iter_difference_t<_It> _Pos_to_shift) const {
            _STL_ASSERT(_Pos_to_shift >= 0, "shift count must be non-negative (N4950 [alg.shift]/1)");

            _Adl_verify_range(_First, _Last);
            auto _Result = _First;
            _Unwrap_iter_t<_It, _Se> _UResult;

            if (_Pos_to_shift == 0) {
                _UResult = _Get_final_iterator_unwrapped<_It>(_Unwrap_iter<_Se>(_Result), _Last);
            }
            else {
                _UResult = _Shift_left_impl(_Unwrap_iter<_Se>(_First), _Unwrap_sent<_It>(_Last), _Pos_to_shift);
            }
            _Seek_wrapped(_Result, _UResult);
            return { _STD move(_First), _STD move(_Result) };
        }

        template <forward_range _Rng>
            requires permutable<iterator_t<_Rng>>
        constexpr borrowed_subrange_t<_Rng> operator()(_Rng&& _Range, range_difference_t<_Rng> _Pos_to_shift) const {
            _STL_ASSERT(_Pos_to_shift >= 0, "shift count must be non-negative (N4950 [alg.shift]/1)");

            if (_Pos_to_shift == 0) {
                auto _Last = _Rewrap_iterator(_Range, _Get_final_iterator_unwrapped(_Range));
                return { _RANGES begin(_Range), _STD move(_Last) };
            }

            if constexpr (sized_range<_Rng>) {
                auto _First = _RANGES begin(_Range);
                if (_RANGES distance(_Range) <= _Pos_to_shift) {
                    return { _First, _First };
                }

                auto _UFirst = _Unwrap_range_iter<_Rng>(_First);
                auto _Start_at = _RANGES next(_UFirst, _Pos_to_shift);
                auto _Result = _RANGES _Move_unchecked(_STD move(_Start_at), _Uend(_Range), _UFirst).out;
                return { _STD move(_First), _Rewrap_iterator(_Range, _STD move(_Result)) };
            }
            else {
                auto _Result = _Shift_left_impl(_Ubegin(_Range), _Uend(_Range), _Pos_to_shift);
                return { _RANGES begin(_Range), _Rewrap_iterator(_Range, _STD move(_Result)) };
            }
        }

    private:
        template <class _It, class _Se>
        _NODISCARD static constexpr _It _Shift_left_impl(_It _First, _Se _Last, iter_difference_t<_It> _Pos_to_shift) {
            _STL_INTERNAL_STATIC_ASSERT(permutable<_It>);
            _STL_INTERNAL_STATIC_ASSERT(sentinel_for<_Se, _It>);
            _STL_INTERNAL_CHECK(_Pos_to_shift > 0);

            auto _Start_at = _First;
            if (_RANGES advance(_Start_at, _Pos_to_shift, _Last) != 0) {
                return _First;
            }

            return _RANGES _Move_unchecked(_STD move(_Start_at), _STD move(_Last), _STD move(_First)).out;
        }
    };

    _EXPORT_STD inline constexpr _Shift_left_fn shift_left{ _Not_quite_object::_Construct_tag{} };

    class _Shift_right_fn : private _Not_quite_object {
    public:
        using _Not_quite_object::_Not_quite_object;

        template <permutable _It, sentinel_for<_It> _Se>
        constexpr subrange<_It> operator()(_It _First, const _Se _Last, iter_difference_t<_It> _Pos_to_shift) const {
            _Adl_verify_range(_First, _Last);
            auto _UFirst = _Unwrap_iter<_Se>(_First);
            auto _ULast = _Unwrap_sent<_It>(_Last);
            const auto _Size = _Size_helper(_UFirst, _ULast);
            auto _Result = _Shift_right_impl(_STD move(_UFirst), _STD move(_ULast), _Pos_to_shift, _Size);
            _Seek_wrapped(_First, _Result.begin());
            auto _Final = _First;
            _Seek_wrapped(_Final, _Result.end());
            return { _STD move(_First), _STD move(_Final) };
        }

        template <forward_range _Rng>
            requires permutable<iterator_t<_Rng>>
        constexpr borrowed_subrange_t<_Rng> operator()(_Rng&& _Range, range_difference_t<_Rng> _Pos_to_shift) const {
            const auto _Size = _Size_helper(_Range);
            auto _Result = _Shift_right_impl(_Ubegin(_Range), _Uend(_Range), _Pos_to_shift, _Size);
            auto _First = _Rewrap_iterator(_Range, _Result.begin());
            auto _Final = _Rewrap_iterator(_Range, _Result.end());
            return { _STD move(_First), _STD move(_Final) };
        }

    private:
        struct _Unsized {};

        template <range _Rng>
        _NODISCARD static constexpr auto _Size_helper(_Rng&& _Range) {
            if constexpr (sized_range<_Rng>) {
                return _RANGES distance(_Range);
            }
            else {
                return _Unsized{};
            }
        }

        template <class _It, sentinel_for<_It> _Se>
        _NODISCARD static constexpr auto _Size_helper(const _It& _First, const _Se& _Last) {
            if constexpr (sized_sentinel_for<_Se, _It>) {
                return _Last - _First;
            }
            else {
                return _Unsized{};
            }
        }

        template <input_iterator _It, weakly_incrementable _Out>
            requires indirectly_movable<_It, _Out>
        _NODISCARD static constexpr _Out _Move_n_helper(_It _First, iter_difference_t<_It> _Count, _Out _Result) {
            for (; _Count > 0; ++_First, (void) --_Count, ++_Result) {
                *_Result = _RANGES iter_move(_First);
            }

            return _Result;
        }

        template <class _It, class _Se, class _SizeTy>
        _NODISCARD static constexpr subrange<_It> _Shift_right_impl(
            _It _First, _Se _Last, iter_difference_t<_It> _Pos_to_shift, const _SizeTy _Size) {
            _STL_INTERNAL_STATIC_ASSERT(permutable<_It>);
            _STL_INTERNAL_STATIC_ASSERT(sentinel_for<_Se, _It>);
            _STL_ASSERT(_Pos_to_shift >= 0, "shift count must be non-negative (N4950 [alg.shift]/5)");

            if (_Pos_to_shift == 0) {
                return { _First, _RANGES next(_First, _Last) };
            }

            constexpr bool _Is_sized = !same_as<_SizeTy, _Unsized>;

            if constexpr (_Is_sized) {
                if (_Pos_to_shift >= _Size) {
                    if constexpr (same_as<_It, _Se>) {
                        return { _Last, _Last };
                    }
                    else if constexpr (random_access_iterator<_It>) {
                        _First += _Size;
                        return { _First, _First };
                    }
                    else {
                        _RANGES advance(_First, _Last);
                        return { _First, _First };
                    }
                }
            }

            if constexpr (_Bidi_common<_It, _Se>) {
                auto _Mid = _Last;
                if constexpr (_Is_sized) {
                    // We validated above that the length of the range is more than _Pos_to_shift
                    _RANGES advance(_Mid, -_Pos_to_shift);
                }
                else {
                    if (_RANGES advance(_Mid, -_Pos_to_shift, _First) != 0) {
                        return { _Last, _Last };
                    }
                }
                return { _RANGES _Move_backward_common(_STD move(_First), _STD move(_Mid), _Last), _Last };
            }
            else if constexpr (_Is_sized && random_access_iterator<_It>) {
                auto _Final = _First + _Size;
                auto _Mid = _Final - _Pos_to_shift;
                return { _RANGES _Move_backward_common(_STD move(_First), _STD move(_Mid), _Final), _Final };
            }
            else {
                auto _Buf = _First;
                if constexpr (_Is_sized) {
                    _RANGES advance(_Buf, _Pos_to_shift);
                }
                else {
                    if (_RANGES advance(_Buf, _Pos_to_shift, _Last) != 0) {
                        return { _Buf, _Buf };
                    }
                }

                if constexpr (_Is_sized) {
                    if (_Size < 2 * _Pos_to_shift) {
                        return { _Buf, _Move_n_helper(_First, _Size - _Pos_to_shift, _Buf) };
                    }
                }

                auto _Lead = _Buf;
                if constexpr (_Is_sized) {
                    // We validated above that the length of the range is at least 2 * _Pos_to_shift
                    _RANGES advance(_Lead, _Pos_to_shift);
                }
                else {
                    if (auto _Rem = _RANGES advance(_Lead, _Pos_to_shift, _Last); _Rem != 0) {
                        return { _Buf, _Move_n_helper(_First, _Pos_to_shift - _Rem, _Buf) };
                    }
                }

                auto _Trail = _Buf;

                // The range is now partitioned like:
                //
                // | _Pos_to_shift elements |           | _Pos_to_shift elements |
                // |------------------------|-----------|------------------------|------------|
                // ^_First              _Buf^     _Trail^                   _Lead^       _Last^
                //
                // From now on, _Mid repeatedly cycles through the [_First, _Buf) window while the
                // [_Trail, _Lead) window advances toward _Last.

                for (;;) {
                    for (auto _Mid = _First; _Mid != _Buf; ++_Mid, (void) ++_Trail, ++_Lead) {
                        if (_Lead == _Last) {
                            // The leading edge of the [_Trail, _Lead) window has hit the end of
                            // the range, so we have only the final _Pos_to_shift elements to process.
                            // Their original values are lost - shifted off the end of the range -
                            // so we only need to fill them with new values from [_First, _Buf).
                            _Trail = _RANGES _Move_unchecked(_Mid, _Buf, _STD move(_Trail)).out;
                            _Trail = _RANGES _Move_unchecked(_First, _Mid, _STD move(_Trail)).out;
                            _STL_INTERNAL_CHECK(_Trail == _Lead);
                            return { _STD move(_Buf), _STD move(_Trail) };
                        }

                        _RANGES iter_swap(_Mid, _Trail);
                    }
                }
            }
        }
    };

    _EXPORT_STD inline constexpr _Shift_right_fn shift_right{ _Not_quite_object::_Construct_tag{} };
} // namespace ranges
#endif // _HAS_CXX23 && defined(__cpp_lib_concepts)

_EXPORT_STD template <class _FwdIt, class _Pr>
_CONSTEXPR20 _FwdIt partition(_FwdIt _First, const _FwdIt _Last, _Pr _Pred) {
    // move elements satisfying _Pred to beginning of sequence
    _Adl_verify_range(_First, _Last);
    auto _UFirst = _Get_unwrapped(_First);
    auto _ULast = _Get_unwrapped(_Last);
    if constexpr (_Is_cpp17_bidi_iter_v<_FwdIt>) {
        for (;;) { // find any out-of-order pair
            for (;;) { // skip in-place elements at beginning
                if (_UFirst == _ULast) {
                    _Seek_wrapped(_First, _UFirst);
                    return _First;
                }

                if (!_Pred(*_UFirst)) {
                    break;
                }

                ++_UFirst;
            }

            do { // skip in-place elements at end
                --_ULast;
                if (_UFirst == _ULast) {
                    _Seek_wrapped(_First, _UFirst);
                    return _First;
                }
            } while (!_Pred(*_ULast));

            swap(*_UFirst, *_ULast); // out of place, swap and loop; intentional ADL
            ++_UFirst;
        }
    }
    else {
        for (;;) { // skip in-place elements at beginning
            if (_UFirst == _ULast) {
                _Seek_wrapped(_First, _UFirst);
                return _First;
            }

            if (!_Pred(*_UFirst)) {
                break;
            }

            ++_UFirst;
        }

        for (auto _UNext = _UFirst; ++_UNext != _ULast;) {
            if (_Pred(*_UNext)) {
                swap(*_UFirst, *_UNext); // out of place, swap and loop; intentional ADL
                ++_UFirst;
            }
        }
    }

    _Seek_wrapped(_First, _UFirst);
    return _First;
}

#if _HAS_CXX17
_EXPORT_STD template <class _ExPo, class _FwdIt, class _Pr, _Enable_if_execution_policy_t<_ExPo> = 0>
_FwdIt partition(_ExPo&& _Exec, _FwdIt _First, _FwdIt _Last, _Pr _Pred) noexcept; // terminates

#ifdef __cpp_lib_concepts
namespace ranges {
    class _Partition_fn : private _Not_quite_object {
    public:
        using _Not_quite_object::_Not_quite_object;

        template <permutable _It, sentinel_for<_It> _Se, class _Pj = identity,
            indirect_unary_predicate<projected<_It, _Pj>> _Pr>
        constexpr subrange<_It> operator()(_It _First, _Se _Last, _Pr _Pred, _Pj _Proj = {}) const {
            _Adl_verify_range(_First, _Last);
            auto _UResult = _Partition_unchecked(_Unwrap_iter<_Se>(_STD move(_First)),
                _Unwrap_sent<_It>(_STD move(_Last)), _Pass_fn(_Pred), _Pass_fn(_Proj));
            return _Rewrap_subrange<subrange<_It>>(_First, _STD move(_UResult));
        }

        template <forward_range _Rng, class _Pj = identity,
            indirect_unary_predicate<projected<iterator_t<_Rng>, _Pj>> _Pr>
            requires permutable<iterator_t<_Rng>>
        constexpr borrowed_subrange_t<_Rng> operator()(_Rng&& _Range, _Pr _Pred, _Pj _Proj = {}) const {
            auto _UResult = _Partition_unchecked(_Ubegin(_Range), _Uend(_Range), _Pass_fn(_Pred), _Pass_fn(_Proj));
            return _Rewrap_subrange<borrowed_subrange_t<_Rng>>(_Range, _STD move(_UResult));
        }

    private:
        template <class _It, class _Se, class _Pr, class _Pj>
        _NODISCARD static constexpr subrange<_It> _Partition_unchecked(_It _First, _Se _Last, _Pr _Pred, _Pj _Proj) {
            _STL_INTERNAL_STATIC_ASSERT(permutable<_It>);
            _STL_INTERNAL_STATIC_ASSERT(sentinel_for<_Se, _It>);
            _STL_INTERNAL_STATIC_ASSERT(indirect_unary_predicate<_Pr, projected<_It, _Pj>>);

            if constexpr (_Bidi_common<_It, _Se>) {
                auto _Saved_last = _Last;
                for (;; ++_First) { // find any out-of-order pair
                    for (;; ++_First) { // skip in-place elements at beginning
                        if (_First == _Last) {
                            return { _STD move(_First), _STD move(_Saved_last) };
                        }

                        if (!_STD invoke(_Pred, _STD invoke(_Proj, *_First))) {
                            break;
                        }
                    }

                    do { // skip in-place elements at end
                        --_Last;
                        if (_First == _Last) {
                            return { _STD move(_First), _STD move(_Saved_last) };
                        }
                    } while (!_STD invoke(_Pred, _STD invoke(_Proj, *_Last)));

                    _RANGES iter_swap(_First, _Last); // out of place, swap and loop
                }

                return { _STD move(_First), _STD move(_Saved_last) };
            }
            else {
                for (;; ++_First) { // skip in-place elements at beginning
                    if (_First == _Last) {
                        return { _First, _First };
                    }

                    if (!_STD invoke(_Pred, _STD invoke(_Proj, *_First))) {
                        break;
                    }
                }

                auto _Next = _First;
                while (++_Next != _Last) {
                    if (_STD invoke(_Pred, _STD invoke(_Proj, *_Next))) {
                        _RANGES iter_swap(_First, _Next); // out of place, swap and loop
                        ++_First;
                    }
                }

                return { _STD move(_First), _STD move(_Next) };
            }
        }
    };

    _EXPORT_STD inline constexpr _Partition_fn partition{ _Not_quite_object::_Construct_tag{} };
} // namespace ranges
#endif // __cpp_lib_concepts
#endif // _HAS_CXX17

template <class _BidIt>
_BidIt _Buffered_rotate_unchecked(const _BidIt _First, const _BidIt _Mid, const _BidIt _Last,
    const _Iter_diff_t<_BidIt> _Count1, const _Iter_diff_t<_BidIt> _Count2, _Iter_value_t<_BidIt>* const _Temp_ptr,
    const ptrdiff_t _Capacity) { // rotate [_First, _Last) using temp buffer
    // precondition: _Count1 == distance(_First, _Mid)
    // precondition: _Count2 == distance(_Mid, _Last)
    if (_Count1 == 0) {
        return _Last;
    }

    if (_Count2 == 0) {
        return _First;
    }

    if (_Count1 <= _Count2 && _Count1 <= _Capacity) { // buffer left range, then copy parts
        _Uninitialized_backout<_Iter_value_t<_BidIt>*> _Backout{
            _Temp_ptr, _STD _Uninitialized_move_unchecked(_First, _Mid, _Temp_ptr) };
        const _BidIt _New_mid = _STD _Move_unchecked(_Mid, _Last, _First);
        _STD _Move_unchecked(_Backout._First, _Backout._Last, _New_mid);
        return _New_mid; // _Backout destroys elements in temporary buffer
    }

    if (_Count2 <= _Capacity) { // buffer right range, then copy parts
        _Uninitialized_backout<_Iter_value_t<_BidIt>*> _Backout{
            _Temp_ptr, _STD _Uninitialized_move_unchecked(_Mid, _Last, _Temp_ptr) };
        _Move_backward_unchecked(_First, _Mid, _Last);
        return _STD _Move_unchecked(_Backout._First, _Backout._Last, _First); // ditto _Backout destroys elements
    }

    // buffer too small, rotate in place
    return _STD rotate(_First, _Mid, _Last);
}

template <class _BidIt, class _Pr>
pair<_BidIt, _Iter_diff_t<_BidIt>> _Stable_partition_unchecked1(_BidIt _First, _BidIt _Last, _Pr _Pred,
    const _Iter_diff_t<_BidIt> _Count, _Iter_value_t<_BidIt>* const _Temp_ptr, const ptrdiff_t _Capacity) {
    // implement stable_partition of [_First, _Last] (note: closed range)
    // precondition: !_Pred(*_First)
    // precondition: _Pred(*_Last)
    // precondition: distance(_First, _Last) + 1 == _Count
    // note: _Count >= 2 and _First != _Last
    // returns: a pair such that first is the partition point, and second is distance(_First, partition point)
    using _Diff = _Iter_diff_t<_BidIt>;
    if (_Count - static_cast<_Diff>(1) <= _Capacity) { // - 1 since we never need to store *_Last
        _Uninitialized_backout<_Iter_value_t<_BidIt>*> _Backout{ _Temp_ptr };
        _BidIt _Next = _First;
        _Backout._Emplace_back(_STD move(*_First));
        while (++_First != _Last) { // test each element, copying to _Temp_ptr if it's in the false range, or assigning
            // backwards if it's in the true range
            if (_Pred(*_First)) {
                *_Next = _STD move(*_First);
                ++_Next;
            }
            else {
                _Backout._Emplace_back(_STD move(*_First));
            }
        }

        // move the last true element, *_Last, to the end of the true range
        *_Next = _STD move(*_Last);
        ++_Next;
        _STD _Move_unchecked(_Backout._First, _Backout._Last, _Next); // copy back the false range
        _Diff _True_distance = static_cast<_Diff>(_Count - static_cast<_Diff>(_Backout._Last - _Backout._First));
        return pair<_BidIt, _Diff>(_Next, _True_distance); // _Backout destroys elements
    }

    const _Diff _Mid_offset = _Count / static_cast<_Diff>(2); // note: >= 1 because _Count >= 2
    const _BidIt _Mid = _STD next(_First, _Mid_offset);

    // form [_First, _Left) true range, [_Left, _Mid) false range
    _BidIt _Left = _Mid;
    _Diff _Left_true_count = _Mid_offset;
    for (;;) { // skip over the trailing false range before _Mid
        --_Left;
        if (_First == _Left) { // the entire left range is false
            --_Left_true_count; // to exclude *_First
            break;
        }

        if (_Pred(*_Left)) { // excluded the false range before _Mid, invariants reestablished, recurse
            const pair<_BidIt, _Diff> _Low =
                _Stable_partition_unchecked1(_First, _Left, _Pred, _Left_true_count, _Temp_ptr, _Capacity);
            _Left = _Low.first;
            _Left_true_count = _Low.second;
            break;
        }

        --_Left_true_count;
    }

    // form [_Mid, _Right) true range, [_Right, next(_Last)) false range
    _BidIt _Right = _Mid;
    _Diff _Right_true_count = 0;
    for (;;) { // skip over the leading true range after and including _Mid
        if (_Right == _Last) { // the entire right range is true
            ++_Right; // to include _Last
            ++_Right_true_count;
            break;
        }

        if (!_Pred(*_Right)) { // excluded the true range after and including _Mid, invariants reestablished, recurse
            const _Diff _Right_count = _Count - _Mid_offset;
            const _Diff _Remaining = _Right_count - _Right_true_count;
            const pair<_BidIt, _Diff> _High =
                _Stable_partition_unchecked1(_Right, _Last, _Pred, _Remaining, _Temp_ptr, _Capacity);
            _Right = _High.first;
            _Right_true_count += _High.second;
            break;
        }

        ++_Right;
        ++_Right_true_count;
    }

    // swap the [_Left, _Mid) false range with the [_Mid, _Right) true range
    const _BidIt _Partition_point = _Buffered_rotate_unchecked(_Left, _Mid, _Right,
        static_cast<_Diff>(_Mid_offset - _Left_true_count), _Right_true_count, _Temp_ptr, _Capacity);
    return pair<_BidIt, _Diff>(_Partition_point, static_cast<_Diff>(_Left_true_count + _Right_true_count));
}

template <class _BidIt, class _Pr>
_BidIt _Stable_partition_unchecked(_BidIt _First, _BidIt _Last, _Pr _Pred) {
    // partition preserving order of equivalents
    for (;;) {
        if (_First == _Last) { // the input range is true (already partitioned)
            return _First;
        }

        if (!_Pred(*_First)) { // excluded the leading true range
            break;
        }

        ++_First;
    }

    for (;;) {
        --_Last;
        if (_First == _Last) { // the input range is already partitioned
            return _First;
        }

        if (_Pred(*_Last)) { // excluded the trailing false range
            break;
        }
    }

    using _Diff = _Iter_diff_t<_BidIt>;
    const _Diff _Temp_count = _STD distance(_First, _Last); // _Total_count - 1 since we never need to store *_Last
    const _Diff _Total_count = _Temp_count + static_cast<_Diff>(1);
    _Optimistic_temporary_buffer<_Iter_value_t<_BidIt>> _Temp_buf{ _Temp_count };
    return _Stable_partition_unchecked1(_First, _Last, _Pred, _Total_count, _Temp_buf._Data, _Temp_buf._Capacity).first;
}

_EXPORT_STD template <class _BidIt, class _Pr>
_BidIt stable_partition(_BidIt _First, _BidIt _Last, _Pr _Pred) {
    // partition preserving order of equivalents
    _Adl_verify_range(_First, _Last);
    _Seek_wrapped(_First, _Stable_partition_unchecked(_Get_unwrapped(_First), _Get_unwrapped(_Last), _Pass_fn(_Pred)));
    return _First;
}

#if _HAS_CXX17
_EXPORT_STD template <class _ExPo, class _BidIt, class _Pr, _Enable_if_execution_policy_t<_ExPo> = 0>
_BidIt stable_partition(_ExPo&&, _BidIt _First, _BidIt _Last, _Pr _Pred) noexcept /* terminates */ {
    // partition preserving order of equivalents
    // not parallelized at present, parallelism expected to be feasible in a future release
    return _STD stable_partition(_First, _Last, _Pass_fn(_Pred));
}
#endif // _HAS_CXX17

#ifdef __cpp_lib_concepts
namespace ranges {
    template <bidirectional_iterator _It>
    _It _Buffered_rotate_common(const _It _First, const _It _Mid, const _It _Last, const iter_difference_t<_It> _Count1,
        const iter_difference_t<_It> _Count2, iter_value_t<_It>* const _Temp_ptr, const ptrdiff_t _Capacity) {
        // rotate [_First, _Last) using temp buffer
        _STL_INTERNAL_CHECK(_Count1 == _RANGES distance(_First, _Mid));
        _STL_INTERNAL_CHECK(_Count2 == _RANGES distance(_Mid, _Last));

        if (_Count1 == 0) {
            return _Last;
        }

        if (_Count2 == 0) {
            return _First;
        }

        if (_Count1 <= _Count2 && _Count1 <= _Capacity) { // buffer left range, then move parts
            _Uninitialized_backout<iter_value_t<_It>*> _Backout{
                _Temp_ptr, _RANGES _Uninitialized_move_unchecked(_First, _Mid, _Temp_ptr, _Temp_ptr + _Count1).out };
            const _It _New_mid = _RANGES _Move_unchecked(_STD move(_Mid), _STD move(_Last), _STD move(_First)).out;
            _RANGES _Move_unchecked(_Backout._First, _Backout._Last, _New_mid);
            return _New_mid;
        }

        if (_Count2 <= _Capacity) { // buffer right range, then move parts
            _Uninitialized_backout<iter_value_t<_It>*> _Backout{
                _Temp_ptr, _RANGES _Uninitialized_move_unchecked(_Mid, _Last, _Temp_ptr, _Temp_ptr + _Count2).out };
            _RANGES _Move_backward_common(_First, _STD move(_Mid), _STD move(_Last));
            return _RANGES _Move_unchecked(_Backout._First, _Backout._Last, _STD move(_First)).out;
        }

        // buffer too small, rotate in place
        return _RANGES _Rotate_unchecked(_STD move(_First), _STD move(_Mid), _STD move(_Last)).begin();
    }

    class _Stable_partition_fn : private _Not_quite_object {
    public:
        using _Not_quite_object::_Not_quite_object;

        template <bidirectional_iterator _It, sentinel_for<_It> _Se, class _Pj = identity,
            indirect_unary_predicate<projected<_It, _Pj>> _Pr>
            requires permutable<_It>
        subrange<_It> operator()(_It _First, _Se _Last, _Pr _Pred, _Pj _Proj = {}) const {
            _Adl_verify_range(_First, _Last);
            auto _UFirst = _Unwrap_iter<_Se>(_STD move(_First));
            auto _ULast = _Get_final_iterator_unwrapped<_It>(_UFirst, _STD move(_Last));

            auto _UResult =
                _Stable_partition_common(_STD move(_UFirst), _STD move(_ULast), _Pass_fn(_Pred), _Pass_fn(_Proj));
            return _Rewrap_subrange<subrange<_It>>(_First, _STD move(_UResult));
        }

        template <bidirectional_range _Rng, class _Pj = identity,
            indirect_unary_predicate<projected<iterator_t<_Rng>, _Pj>> _Pr>
            requires permutable<iterator_t<_Rng>>
        borrowed_subrange_t<_Rng> operator()(_Rng&& _Range, _Pr _Pred, _Pj _Proj = {}) const {
            auto _ULast = _Get_final_iterator_unwrapped(_Range);
            auto _UResult =
                _Stable_partition_common(_Ubegin(_Range), _STD move(_ULast), _Pass_fn(_Pred), _Pass_fn(_Proj));
            return _Rewrap_subrange<borrowed_subrange_t<_Rng>>(_Range, _STD move(_UResult));
        }

    private:
        template <class _It, class _Pr, class _Pj>
        _NODISCARD static subrange<_It> _Stable_partition_common(_It _First, _It _Last, _Pr _Pred, _Pj _Proj) {
            _STL_INTERNAL_STATIC_ASSERT(bidirectional_iterator<_It>);
            _STL_INTERNAL_STATIC_ASSERT(permutable<_It>);
            _STL_INTERNAL_STATIC_ASSERT(indirect_unary_predicate<_Pr, projected<_It, _Pj>>);

            // partition preserving order of equivalents
            for (;;) { // skip in-place elements at front
                if (_First == _Last) { // the input range is true (already partitioned)
                    return { _STD move(_First), _STD move(_Last) };
                }

                if (!_STD invoke(_Pred, _STD invoke(_Proj, *_First))) {
                    break;
                }
                ++_First;
            }

            auto _Saved_last = _Last;
            do { // skip in-place elements at end
                --_Last;
                if (_First == _Last) {
                    return { _STD move(_First), _STD move(_Saved_last) };
                }
            } while (!_STD invoke(_Pred, _STD invoke(_Proj, *_Last)));

            const iter_difference_t<_It> _Temp_count = _RANGES distance(_First, _Last);
            _Optimistic_temporary_buffer<iter_value_t<_It>> _Temp_buf{ _Temp_count };

            // _Temp_count + 1 since we work on closed ranges
            const auto _Total_count = static_cast<iter_difference_t<_It>>(_Temp_count + 1);

            auto _Result = _Stable_partition_common_buffered(
                _STD move(_First), _STD move(_Last), _Pred, _Proj, _Total_count, _Temp_buf._Data, _Temp_buf._Capacity);
            return { _STD move(_Result.first), _STD move(_Saved_last) };
        }

        template <class _It, class _Pr, class _Pj>
        _NODISCARD static pair<_It, iter_difference_t<_It>> _Stable_partition_common_buffered(_It _First, _It _Last,
            _Pr _Pred, _Pj _Proj, const iter_difference_t<_It> _Count, iter_value_t<_It>* const _Temp_ptr,
            const ptrdiff_t _Capacity) {
            // implement stable_partition of [_First, _Last] (note: closed range)
            // note: _Count >= 2 and _First != _Last
            _STL_INTERNAL_STATIC_ASSERT(permutable<_It>);
            _STL_INTERNAL_STATIC_ASSERT(indirect_unary_predicate<_Pr, projected<_It, _Pj>>);
            _STL_INTERNAL_STATIC_ASSERT(bidirectional_iterator<_It>);
            _STL_INTERNAL_CHECK(!_STD invoke(_Pred, _STD invoke(_Proj, *_First)));
            _STL_INTERNAL_CHECK(_STD invoke(_Pred, _STD invoke(_Proj, *_Last)));
            _STL_INTERNAL_CHECK(_Count == _RANGES distance(_First, _Last) + 1);

            using _Diff = iter_difference_t<_It>;
            if (_Count - 1 <= _Capacity) { // - 1 since we never need to store *_Last
                _Uninitialized_backout<iter_value_t<_It>*> _Backout{ _Temp_ptr };
                _It _Next = _First;
                _Backout._Emplace_back(_RANGES iter_move(_First));
                while (++_First != _Last) {
                    // test each element, moving into the temporary buffer if it's in the false range, or
                    // assigning backwards if it's in the true range
                    if (_STD invoke(_Pred, _STD invoke(_Proj, *_First))) {
                        *_Next = _RANGES iter_move(_First);
                        ++_Next;
                    }
                    else {
                        _Backout._Emplace_back(_RANGES iter_move(_First));
                    }
                }

                // move the last true element, *_Last, to the end of the true range
                *_Next = _RANGES iter_move(_Last);
                ++_Next;
                // copy back the false range
                _RANGES _Move_unchecked(_Backout._First, _Backout._Last, _Next);
                const auto _True_distance = static_cast<_Diff>(_Count - (_Backout._Last - _Backout._First));
                return { _STD move(_Next), _True_distance };
            }

            const _Diff _Mid_offset = _Count >> 1; // _Mid_offset >= 1 because _Count >= 2
            const _It _Mid = _RANGES next(_First, _Mid_offset);
            // form [_First, _Left) true range, [_Left, _Mid) false range
            _It _Left = _Mid;
            _Diff _Left_true_count = _Mid_offset;
            for (;;) { // skip over the trailing false range before _Mid
                --_Left;
                --_Left_true_count;
                if (_First == _Left) { // the entire left range is false
                    break;
                }

                if (_STD invoke(_Pred, _STD invoke(_Proj, *_Left))) {
                    // excluded the false range before _Mid, invariants reestablished, recurse
                    ++_Left_true_count; // to include *_First
                    const auto _Low = _Stable_partition_common_buffered(
                        _First, _STD move(_Left), _Pred, _Proj, _Left_true_count, _Temp_ptr, _Capacity);
                    _Left = _STD move(_Low.first);
                    _Left_true_count = _Low.second;
                    break;
                }
            }

            // form [_Mid, _Right) true range, [_Right, next(_Last)) false range
            _It _Right = _Mid;
            _Diff _Right_true_count = 0;
            for (;;) { // skip over the leading true range after and including _Mid
                if (_Right == _Last) { // the entire right range is true
                    ++_Right; // to include _Last
                    ++_Right_true_count;
                    break;
                }

                if (!_STD invoke(_Pred, _STD invoke(_Proj, *_Right))) {
                    // excluded the true range after and including _Mid, invariants reestablished, recurse
                    const auto _Right_count = static_cast<_Diff>(_Count - _Mid_offset);
                    const auto _Remaining = static_cast<_Diff>(_Right_count - _Right_true_count);
                    const auto _High = _Stable_partition_common_buffered(
                        _STD move(_Right), _Last, _Pred, _Proj, _Remaining, _Temp_ptr, _Capacity);
                    _Right = _STD move(_High.first);
                    _Right_true_count += _High.second;
                    break;
                }

                ++_Right;
                ++_Right_true_count;
            }

            // swap the [_Left, _Mid) false range with the [_Mid, _Right) true range
            auto _Partition_point =
                _RANGES _Buffered_rotate_common(_STD move(_Left), _STD move(_Mid), _STD move(_Right),
                    static_cast<_Diff>(_Mid_offset - _Left_true_count), _Right_true_count, _Temp_ptr, _Capacity);
            return { _STD move(_Partition_point), static_cast<_Diff>(_Left_true_count + _Right_true_count) };
        }
    };

    _EXPORT_STD inline constexpr _Stable_partition_fn stable_partition{ _Not_quite_object::_Construct_tag{} };
} // namespace ranges
#endif // __cpp_lib_concepts

template <class _RanIt, class _Ty, class _Pr>
_CONSTEXPR20 void _Push_heap_by_index(
    _RanIt _First, _Iter_diff_t<_RanIt> _Hole, _Iter_diff_t<_RanIt> _Top, _Ty&& _Val, _Pr _Pred) {
    // percolate _Hole to _Top or where _Val belongs
    using _Diff = _Iter_diff_t<_RanIt>;
    for (_Diff _Idx = (_Hole - 1) >> 1; // shift for codegen
        _Top < _Hole && _DEBUG_LT_PRED(_Pred, *(_First + _Idx), _Val); //
        _Idx = (_Hole - 1) >> 1) { // shift for codegen
        // move _Hole up to parent
        *(_First + _Hole) = _STD move(*(_First + _Idx));
        _Hole = _Idx;
    }

    *(_First + _Hole) = _STD forward<_Ty>(_Val); // drop _Val into final hole
}

_EXPORT_STD template <class _RanIt, class _Pr>
_CONSTEXPR20 void push_heap(_RanIt _First, _RanIt _Last, _Pr _Pred) {
    // push *(_Last - 1) onto heap at [_First, _Last - 1)
    _Adl_verify_range(_First, _Last);
    const auto _UFirst = _Get_unwrapped(_First);
    auto _ULast = _Get_unwrapped(_Last);
    using _Diff = _Iter_diff_t<_RanIt>;
    _Diff _Count = _ULast - _UFirst;
    if (2 <= _Count) {
        _Iter_value_t<_RanIt> _Val = _STD move(*--_ULast);
        _STD _Push_heap_by_index(_UFirst, --_Count, _Diff(0), _STD move(_Val), _Pass_fn(_Pred));
    }
}

_EXPORT_STD template <class _RanIt>
_CONSTEXPR20 void push_heap(_RanIt _First, _RanIt _Last) {
    // push *(_Last - 1) onto heap at [_First, _Last - 1)
    _STD push_heap(_First, _Last, less<>{});
}

#ifdef __cpp_lib_concepts
namespace ranges {
    template <random_access_iterator _It, class _Ty, class _Pr, class _Pj1, class _Pj2>
        requires sortable<_It, _Pr, _Pj1>&& indirectly_writable<_It, _Ty>
    && indirect_strict_weak_order<_Pr, projected<_It, _Pj1>, projected<remove_reference_t<_Ty>*, _Pj2>>
        constexpr void _Push_heap_by_index(const _It _First, iter_difference_t<_It> _Hole,
            const iter_difference_t<_It> _Top, _Ty&& _Val, _Pr _Pred, _Pj1 _Proj1, _Pj2 _Proj2) {
        // percolate _Hole to _Top or where _Val belongs
        while (_Top < _Hole) {
            const auto _Idx = static_cast<iter_difference_t<_It>>((_Hole - 1) >> 1); // shift for codegen
            if (!_STD invoke(_Pred, _STD invoke(_Proj1, *(_First + _Idx)), _STD invoke(_Proj2, _Val))) {
                break;
            }

            // move _Hole up to parent
            *(_First + _Hole) = _RANGES iter_move(_First + _Idx);
            _Hole = _Idx;
        }

        *(_First + _Hole) = _STD forward<_Ty>(_Val); // drop _Val into final hole
    }

    class _Push_heap_fn : private _Not_quite_object {
    public:
        using _Not_quite_object::_Not_quite_object;

        template <random_access_iterator _It, sentinel_for<_It> _Se, class _Pr = ranges::less, class _Pj = identity>
            requires sortable<_It, _Pr, _Pj>
        constexpr _It operator()(_It _First, _Se _Last, _Pr _Pred = {}, _Pj _Proj = {}) const {
            _Adl_verify_range(_First, _Last);
            auto _UFirst = _Unwrap_iter<_Se>(_STD move(_First));
            auto _ULast = _Get_final_iterator_unwrapped<_It>(_UFirst, _STD move(_Last));
            _Seek_wrapped(_First, _ULast);
            _Push_heap_unchecked(_STD move(_UFirst), _STD move(_ULast), _Pass_fn(_Pred), _Pass_fn(_Proj));
            return _First;
        }

        template <random_access_range _Rng, class _Pr = ranges::less, class _Pj = identity>
            requires sortable<iterator_t<_Rng>, _Pr, _Pj>
        constexpr borrowed_iterator_t<_Rng> operator()(_Rng&& _Range, _Pr _Pred = {}, _Pj _Proj = {}) const {
            if constexpr (common_range<_Rng>) {
                _Push_heap_unchecked(_Ubegin(_Range), _Uend(_Range), _Pass_fn(_Pred), _Pass_fn(_Proj));
                return _RANGES end(_Range);
            }
            else {
                auto _ULast = _Get_final_iterator_unwrapped(_Range);
                _Push_heap_unchecked(_Ubegin(_Range), _ULast, _Pass_fn(_Pred), _Pass_fn(_Proj));
                return _Rewrap_iterator(_Range, _STD move(_ULast));
            }
        }

    private:
        template <class _It, class _Pr, class _Pj>
        static constexpr void _Push_heap_unchecked(_It _First, _It _Last, _Pr _Pred, _Pj _Proj) {
            _STL_INTERNAL_STATIC_ASSERT(random_access_iterator<_It>);
            _STL_INTERNAL_STATIC_ASSERT(sortable<_It, _Pr, _Pj>);

            const auto _Count = _Last - _First;
            if (_Count < 2) {
                return;
            }

            --_Last;
            iter_value_t<_It> _Val = _RANGES iter_move(_Last);
            // NB: if _Proj is a _Ref_fn, this aliases the _Proj1 and _Proj2 parameters of _Push_heap_by_index
            _RANGES _Push_heap_by_index(_STD move(_First), _Count - 1, 0, _STD move(_Val), _Pred, _Proj, _Proj);
        }
    };

    _EXPORT_STD inline constexpr _Push_heap_fn push_heap{ _Not_quite_object::_Construct_tag{} };
} // namespace ranges
#endif // __cpp_lib_concepts

template <class _RanIt, class _Ty, class _Pr>
_CONSTEXPR20 void _Pop_heap_hole_by_index(
    _RanIt _First, _Iter_diff_t<_RanIt> _Hole, _Iter_diff_t<_RanIt> _Bottom, _Ty&& _Val, _Pr _Pred) {
    // percolate _Hole to _Bottom, then push _Val
    _STL_INTERNAL_CHECK(_Bottom > 0);

    using _Diff = _Iter_diff_t<_RanIt>;
    const _Diff _Top = _Hole;
    _Diff _Idx = _Hole;

    // Check whether _Idx can have a child before calculating that child's index, since
    // calculating the child's index can trigger integer overflows
    const _Diff _Max_sequence_non_leaf = (_Bottom - 1) >> 1; // shift for codegen
    while (_Idx < _Max_sequence_non_leaf) { // move _Hole down to larger child
        _Idx = 2 * _Idx + 2;
        if (_DEBUG_LT_PRED(_Pred, *(_First + _Idx), *(_First + (_Idx - 1)))) {
            --_Idx;
        }
        *(_First + _Hole) = _STD move(*(_First + _Idx));
        _Hole = _Idx;
    }

    if (_Idx == _Max_sequence_non_leaf && _Bottom % 2 == 0) { // only child at bottom, move _Hole down to it
        *(_First + _Hole) = _STD move(*(_First + (_Bottom - 1)));
        _Hole = _Bottom - 1;
    }

    _STD _Push_heap_by_index(_First, _Hole, _Top, _STD forward<_Ty>(_Val), _Pred);
}

template <class _RanIt, class _Ty, class _Pr>
_CONSTEXPR20 void _Pop_heap_hole_unchecked(_RanIt _First, _RanIt _Last, _RanIt _Dest, _Ty&& _Val, _Pr _Pred) {
    // pop *_First to *_Dest and reheap
    // precondition: _First != _Last
    // precondition: _First != _Dest
    *_Dest = _STD move(*_First);
    using _Diff = _Iter_diff_t<_RanIt>;
    _STD _Pop_heap_hole_by_index(
        _First, static_cast<_Diff>(0), static_cast<_Diff>(_Last - _First), _STD forward<_Ty>(_Val), _Pred);
}

template <class _RanIt, class _Pr>
_CONSTEXPR20 void _Pop_heap_unchecked(_RanIt _First, _RanIt _Last, _Pr _Pred) {
    // pop *_First to *(_Last - 1) and reheap
    if (2 <= _Last - _First) {
        --_Last;
        _Iter_value_t<_RanIt> _Val = _STD move(*_Last);
        _STD _Pop_heap_hole_unchecked(_First, _Last, _Last, _STD move(_Val), _Pred);
    }
}

_EXPORT_STD template <class _RanIt, class _Pr>
_CONSTEXPR20 void pop_heap(_RanIt _First, _RanIt _Last, _Pr _Pred) {
    // pop *_First to *(_Last - 1) and reheap
    _Adl_verify_range(_First, _Last);
    _STD _Pop_heap_unchecked(_Get_unwrapped(_First), _Get_unwrapped(_Last), _Pass_fn(_Pred));
}

_EXPORT_STD template <class _RanIt>
_CONSTEXPR20 void pop_heap(_RanIt _First, _RanIt _Last) {
    // pop *_First to *(_Last - 1) and reheap
    _STD pop_heap(_First, _Last, less<>{});
}

#ifdef __cpp_lib_concepts
namespace ranges {
    template <random_access_iterator _It, class _Ty, class _Pr, class _Pj1, class _Pj2>
        requires sortable<_It, _Pr, _Pj1>&& indirectly_writable<_It, _Ty>
    && indirect_strict_weak_order<_Pr, projected<_It, _Pj1>, projected<remove_reference_t<_Ty>*, _Pj2>>
        constexpr void _Pop_heap_hole_by_index(_It _First, iter_difference_t<_It> _Hole,
            const iter_difference_t<_It> _Bottom, _Ty&& _Val, _Pr _Pred, _Pj1 _Proj1, _Pj2 _Proj2) {
        // percolate _Hole to _Bottom, then push _Val
        _STL_INTERNAL_CHECK(_Hole >= 0);
        _STL_INTERNAL_CHECK(_Bottom > 0);

        using _Diff = iter_difference_t<_It>;
        const _Diff _Top = _Hole;
        _Diff _Idx = _Hole;

        // Check whether _Idx can have a child before calculating that child's index, since
        // calculating the child's index can trigger integer overflows
        const _Diff _Max_sequence_non_leaf = (_Bottom - 1) >> 1; // shift for codegen
        while (_Idx < _Max_sequence_non_leaf) { // move _Hole down to larger child
            _Idx = 2 * _Idx + 2;
            auto _Mid = _First + _Idx;
            if (_STD invoke(_Pred, _STD invoke(_Proj1, *_Mid), _STD invoke(_Proj1, *_RANGES prev(_Mid)))) {
                --_Idx;
                --_Mid;
            }
            *(_First + _Hole) = _RANGES iter_move(_Mid);
            _Hole = _Idx;
        }

        if (_Idx == _Max_sequence_non_leaf && _Bottom % 2 == 0) { // only child at bottom, move _Hole down to it
            *(_First + _Hole) = _RANGES iter_move(_First + (_Bottom - 1));
            _Hole = _Bottom - 1;
        }

        _RANGES _Push_heap_by_index(_STD move(_First), _Hole, _Top, _STD forward<_Ty>(_Val), _Pred, _Proj1, _Proj2);
    }

    template <random_access_iterator _It, class _Ty, class _Pr, class _Pj1, class _Pj2>
        requires sortable<_It, _Pr, _Pj1>&& indirectly_writable<_It, _Ty>
    && indirect_strict_weak_order<_Pr, projected<_It, _Pj1>, projected<remove_reference_t<_Ty>*, _Pj2>>
        constexpr void _Pop_heap_hole_unchecked(
            _It _First, const _It _Last, const _It _Dest, _Ty&& _Val, _Pr _Pred, _Pj1 _Proj1, _Pj2 _Proj2) {
        // pop *_First to *_Dest and reheap
        _STL_INTERNAL_CHECK(_First != _Last);
        _STL_INTERNAL_CHECK(_First != _Dest);

        *_Dest = _RANGES iter_move(_First);
        const auto _Count = _Last - _First;
        _RANGES _Pop_heap_hole_by_index(_STD move(_First), 0, _Count, _STD forward<_Ty>(_Val), _Pred, _Proj1, _Proj2);
    }

    template <random_access_iterator _It, class _Pr, class _Pj>
        requires sortable<_It, _Pr, _Pj>
    constexpr void _Pop_heap_unchecked(_It _First, _It _Last, _Pr _Pred, _Pj _Proj) {
        // pop *_First to *(_Last - 1) and reheap
        if (_Last - _First < 2) {
            return;
        }

        --_Last;
        iter_value_t<_It> _Val = _RANGES iter_move(_Last);
        // NB: if _Proj is a _Ref_fn, this aliases the _Proj1 and _Proj2 parameters of _Pop_heap_hole_unchecked
        _RANGES _Pop_heap_hole_unchecked(_STD move(_First), _Last, _Last, _STD move(_Val), _Pred, _Proj, _Proj);
    }

    class _Pop_heap_fn : private _Not_quite_object {
    public:
        using _Not_quite_object::_Not_quite_object;

        template <random_access_iterator _It, sentinel_for<_It> _Se, class _Pr = ranges::less, class _Pj = identity>
            requires sortable<_It, _Pr, _Pj>
        constexpr _It operator()(_It _First, _Se _Last, _Pr _Pred = {}, _Pj _Proj = {}) const {
            _Adl_verify_range(_First, _Last);
            auto _UFirst = _Unwrap_iter<_Se>(_STD move(_First));
            auto _ULast = _Get_final_iterator_unwrapped<_It>(_UFirst, _STD move(_Last));
            _Seek_wrapped(_First, _ULast);
            _RANGES _Pop_heap_unchecked(_STD move(_UFirst), _STD move(_ULast), _Pass_fn(_Pred), _Pass_fn(_Proj));
            return _First;
        }

        template <random_access_range _Rng, class _Pr = ranges::less, class _Pj = identity>
            requires sortable<iterator_t<_Rng>, _Pr, _Pj>
        constexpr borrowed_iterator_t<_Rng> operator()(_Rng&& _Range, _Pr _Pred = {}, _Pj _Proj = {}) const {
            if constexpr (common_range<_Rng>) {
                _RANGES _Pop_heap_unchecked(_Ubegin(_Range), _Uend(_Range), _Pass_fn(_Pred), _Pass_fn(_Proj));
                return _RANGES end(_Range);
            }
            else {
                auto _ULast = _Get_final_iterator_unwrapped(_Range);
                _RANGES _Pop_heap_unchecked(_Ubegin(_Range), _ULast, _Pass_fn(_Pred), _Pass_fn(_Proj));
                return _Rewrap_iterator(_Range, _STD move(_ULast));
            }
        }
    };

    _EXPORT_STD inline constexpr _Pop_heap_fn pop_heap{ _Not_quite_object::_Construct_tag{} };
} // namespace ranges
#endif // __cpp_lib_concepts

template <class _RanIt, class _Pr>
_CONSTEXPR20 void _Make_heap_unchecked(_RanIt _First, _RanIt _Last, _Pr _Pred) {
    // make [_First, _Last) into a heap
    using _Diff = _Iter_diff_t<_RanIt>;
    _Diff _Bottom = _Last - _First;
    for (_Diff _Hole = _Bottom >> 1; _Hole > 0;) { // shift for codegen
        // reheap top half, bottom to top
        --_Hole;
        _Iter_value_t<_RanIt> _Val = _STD move(*(_First + _Hole));
        _STD _Pop_heap_hole_by_index(_First, _Hole, _Bottom, _STD move(_Val), _Pred);
    }
}

_EXPORT_STD template <class _RanIt, class _Pr>
_CONSTEXPR20 void make_heap(_RanIt _First, _RanIt _Last, _Pr _Pred) { // make [_First, _Last) into a heap
    _Adl_verify_range(_First, _Last);
    _Make_heap_unchecked(_Get_unwrapped(_First), _Get_unwrapped(_Last), _Pass_fn(_Pred));
}

_EXPORT_STD template <class _RanIt>
_CONSTEXPR20 void make_heap(_RanIt _First, _RanIt _Last) { // make [_First, _Last) into a heap
    _STD make_heap(_First, _Last, less<>{});
}

#ifdef __cpp_lib_concepts
namespace ranges {
    template <random_access_iterator _It, class _Pr, class _Pj>
        requires sortable<_It, _Pr, _Pj>
    constexpr void _Make_heap_common(_It _First, _It _Last, _Pr _Pred, _Pj _Proj) {
        // make [_First, _Last) into a heap with respect to _Pred and _Proj
        using _Diff = iter_difference_t<_It>;
        const _Diff _Bottom = _Last - _First;
        for (_Diff _Hole = _Bottom >> 1; _Hole > 0;) { // shift for codegen
            // reheap top half, bottom to top
            --_Hole;
            iter_value_t<_It> _Val = _RANGES iter_move(_First + _Hole);
            // NB: if _Proj is a _Ref_fn, this aliases the _Proj1 and _Proj2 parameters of _Pop_heap_hole_by_index
            _RANGES _Pop_heap_hole_by_index(_First, _Hole, _Bottom, _STD move(_Val), _Pred, _Proj, _Proj);
        }
    }

    class _Make_heap_fn : private _Not_quite_object {
    public:
        using _Not_quite_object::_Not_quite_object;

        template <random_access_iterator _It, sentinel_for<_It> _Se, class _Pr = ranges::less, class _Pj = identity>
            requires sortable<_It, _Pr, _Pj>
        constexpr _It operator()(_It _First, _Se _Last, _Pr _Pred = {}, _Pj _Proj = {}) const {
            _Adl_verify_range(_First, _Last);
            auto _UFirst = _Unwrap_iter<_Se>(_STD move(_First));
            auto _ULast = _Get_final_iterator_unwrapped<_It>(_UFirst, _STD move(_Last));
            _Seek_wrapped(_First, _ULast);
            _Make_heap_common(_STD move(_UFirst), _STD move(_ULast), _Pass_fn(_Pred), _Pass_fn(_Proj));
            return _First;
        }

        template <random_access_range _Rng, class _Pr = ranges::less, class _Pj = identity>
            requires sortable<iterator_t<_Rng>, _Pr, _Pj>
        constexpr borrowed_iterator_t<_Rng> operator()(_Rng&& _Range, _Pr _Pred = {}, _Pj _Proj = {}) const {
            if constexpr (common_range<_Rng>) {
                _Make_heap_common(_Ubegin(_Range), _Uend(_Range), _Pass_fn(_Pred), _Pass_fn(_Proj));
                return _RANGES end(_Range);
            }
            else {
                auto _ULast = _Get_final_iterator_unwrapped(_Range);
                _Make_heap_common(_Ubegin(_Range), _ULast, _Pass_fn(_Pred), _Pass_fn(_Proj));
                return _Rewrap_iterator(_Range, _STD move(_ULast));
            }
        }
    };

    _EXPORT_STD inline constexpr _Make_heap_fn make_heap{ _Not_quite_object::_Construct_tag{} };
} // namespace ranges
#endif // __cpp_lib_concepts

template <class _RanIt, class _Pr>
_CONSTEXPR20 _RanIt _Is_heap_until_unchecked(_RanIt _First, _RanIt _Last, _Pr _Pred) {
    // find extent of range that is a heap
    using _Diff = _Iter_diff_t<_RanIt>;
    const _Diff _Size = _Last - _First;
    for (_Diff _Off = 1; _Off < _Size; ++_Off) {
        if (_DEBUG_LT_PRED(_Pred, *(_First + ((_Off - 1) >> 1)), *(_First + _Off))) { // shift for codegen
            return _First + _Off;
        }
    }

    return _Last;
}

_EXPORT_STD template <class _RanIt, class _Pr>
_NODISCARD _CONSTEXPR20 _RanIt is_heap_until(_RanIt _First, _RanIt _Last, _Pr _Pred) {
    // find extent of range that is a heap
    _Adl_verify_range(_First, _Last);
    _Seek_wrapped(
        _First, _STD _Is_heap_until_unchecked(_Get_unwrapped(_First), _Get_unwrapped(_Last), _Pass_fn(_Pred)));
    return _First;
}

_EXPORT_STD template <class _RanIt, class _Pr>
_NODISCARD _CONSTEXPR20 bool is_heap(_RanIt _First, _RanIt _Last, _Pr _Pred) {
    // test if range is a heap
    _Adl_verify_range(_First, _Last);
    const auto _UFirst = _Get_unwrapped(_First);
    const auto _ULast = _Get_unwrapped(_Last);
    return _STD _Is_heap_until_unchecked(_UFirst, _ULast, _Pass_fn(_Pred)) == _ULast;
}

_EXPORT_STD template <class _RanIt>
_NODISCARD _CONSTEXPR20 _RanIt is_heap_until(_RanIt _First, _RanIt _Last) {
    // find extent of range that is a heap ordered by operator<
    return _STD is_heap_until(_First, _Last, less<>{});
}

_EXPORT_STD template <class _RanIt>
_NODISCARD _CONSTEXPR20 bool is_heap(_RanIt _First, _RanIt _Last) { // test if range is a heap ordered by operator<
    return _STD is_heap(_First, _Last, less<>{});
}

#if _HAS_CXX17
_EXPORT_STD template <class _ExPo, class _RanIt, class _Pr, _Enable_if_execution_policy_t<_ExPo> = 0>
_NODISCARD _RanIt is_heap_until(_ExPo&& _Exec, _RanIt _First, _RanIt _Last, _Pr _Pred) noexcept; // terminates

_EXPORT_STD template <class _ExPo, class _RanIt, class _Pr, _Enable_if_execution_policy_t<_ExPo> = 0>
_NODISCARD bool is_heap(_ExPo&& _Exec, _RanIt _First, _RanIt _Last, _Pr _Pred) noexcept /* terminates */ {
    // test if range is a heap
    return _STD is_heap_until(_STD forward<_ExPo>(_Exec), _First, _Last, _Pass_fn(_Pred)) == _Last;
}

_EXPORT_STD template <class _ExPo, class _RanIt, _Enable_if_execution_policy_t<_ExPo> = 0>
_NODISCARD _RanIt is_heap_until(_ExPo&& _Exec, _RanIt _First, _RanIt _Last) noexcept /* terminates */ {
    // find extent of range that is a heap ordered by operator<
    return _STD is_heap_until(_STD forward<_ExPo>(_Exec), _First, _Last, less{});
}

_EXPORT_STD template <class _ExPo, class _RanIt, _Enable_if_execution_policy_t<_ExPo> = 0>
_NODISCARD bool is_heap(_ExPo&& _Exec, _RanIt _First, _RanIt _Last) noexcept /* terminates */ {
    // test if range is a heap ordered by operator<
    return _STD is_heap(_STD forward<_ExPo>(_Exec), _First, _Last, less{});
}

#ifdef __cpp_lib_concepts
namespace ranges {
    template <class _It, class _Pr, class _Pj>
    _NODISCARD constexpr _It _Is_heap_until_unchecked(
        _It _First, const iter_difference_t<_It> _Size, _Pr _Pred, _Pj _Proj) {
        // find extent of counted range that is a heap
        _STL_INTERNAL_STATIC_ASSERT(random_access_iterator<_It>);
        _STL_INTERNAL_STATIC_ASSERT(indirect_strict_weak_order<_Pr, projected<_It, _Pj>>);

        if (_Size == 0) {
            return _First;
        }

        using _Diff = iter_difference_t<_It>;
        _Diff _Off = 1;
        for (; _Off < _Size; ++_Off) {
            const auto _Mid1 = _First + static_cast<_Diff>((_Off - 1) >> 1); // shift for codegen
            auto _Mid2 = _First + _Off;
            if (_STD invoke(_Pred, _STD invoke(_Proj, *_Mid1), _STD invoke(_Proj, *_Mid2))) {
                return _Mid2;
            }
        }

        return _First + _Off;
    }

    class _Is_heap_fn : private _Not_quite_object {
    public:
        using _Not_quite_object::_Not_quite_object;

        template <random_access_iterator _It, sentinel_for<_It> _Se, class _Pj = identity,
            indirect_strict_weak_order<projected<_It, _Pj>> _Pr = ranges::less>
        _NODISCARD constexpr bool operator()(_It _First, _Se _Last, _Pr _Pred = {}, _Pj _Proj = {}) const {
            _Adl_verify_range(_First, _Last);
            auto _UFirst = _Unwrap_iter<_Se>(_STD move(_First));
            const auto _ULast = _Unwrap_sent<_It>(_STD move(_Last));
            const auto _Size = _RANGES distance(_UFirst, _ULast);
            const auto _UResult =
                _RANGES _Is_heap_until_unchecked(_STD move(_UFirst), _Size, _Pass_fn(_Pred), _Pass_fn(_Proj));
            return _UResult == _ULast;
        }

        template <random_access_range _Rng, class _Pj = identity,
            indirect_strict_weak_order<projected<iterator_t<_Rng>, _Pj>> _Pr = ranges::less>
        _NODISCARD constexpr bool operator()(_Rng&& _Range, _Pr _Pred = {}, _Pj _Proj = {}) const {
            const auto _Size = _RANGES distance(_Range);
            const auto _UResult =
                _RANGES _Is_heap_until_unchecked(_Ubegin(_Range), _Size, _Pass_fn(_Pred), _Pass_fn(_Proj));
            return _UResult == _Uend(_Range);
        }
    };

    _EXPORT_STD inline constexpr _Is_heap_fn is_heap{ _Not_quite_object::_Construct_tag{} };

    class _Is_heap_until_fn : private _Not_quite_object {
    public:
        using _Not_quite_object::_Not_quite_object;

        template <random_access_iterator _It, sentinel_for<_It> _Se, class _Pj = identity,
            indirect_strict_weak_order<projected<_It, _Pj>> _Pr = ranges::less>
        _NODISCARD constexpr _It operator()(_It _First, _Se _Last, _Pr _Pred = {}, _Pj _Proj = {}) const {
            _Adl_verify_range(_First, _Last);
            auto _UFirst = _Unwrap_iter<_Se>(_STD move(_First));
            const auto _Size = _RANGES distance(_UFirst, _Unwrap_sent<_It>(_STD move(_Last)));
            auto _UResult =
                _RANGES _Is_heap_until_unchecked(_STD move(_UFirst), _Size, _Pass_fn(_Pred), _Pass_fn(_Proj));
            _Seek_wrapped(_First, _STD move(_UResult));
            return _First;
        }

        template <random_access_range _Rng, class _Pj = identity,
            indirect_strict_weak_order<projected<iterator_t<_Rng>, _Pj>> _Pr = ranges::less>
        _NODISCARD constexpr borrowed_iterator_t<_Rng> operator()(_Rng&& _Range, _Pr _Pred = {}, _Pj _Proj = {}) const {
            const auto _Size = _RANGES distance(_Range);
            auto _UResult = _RANGES _Is_heap_until_unchecked(_Ubegin(_Range), _Size, _Pass_fn(_Pred), _Pass_fn(_Proj));
            return _Rewrap_iterator(_Range, _STD move(_UResult));
        }
    };

    _EXPORT_STD inline constexpr _Is_heap_until_fn is_heap_until{ _Not_quite_object::_Construct_tag{} };
} // namespace ranges
#endif // __cpp_lib_concepts
#endif // _HAS_CXX17

template <class _RanIt, class _Pr>
_CONSTEXPR20 void _Sort_heap_unchecked(_RanIt _First, _RanIt _Last, _Pr _Pred) {
    // order heap by repeatedly popping
    for (; _Last - _First >= 2; --_Last) {
        _STD _Pop_heap_unchecked(_First, _Last, _Pred);
    }
}

_EXPORT_STD template <class _RanIt, class _Pr>
_CONSTEXPR20 void sort_heap(_RanIt _First, _RanIt _Last, _Pr _Pred) { // order heap by repeatedly popping
    _Adl_verify_range(_First, _Last);
    const auto _UFirst = _Get_unwrapped(_First);
    const auto _ULast = _Get_unwrapped(_Last);
#if _ITERATOR_DEBUG_LEVEL == 2
    const auto _Counterexample = _STD _Is_heap_until_unchecked(_UFirst, _ULast, _Pass_fn(_Pred));
    if (_Counterexample != _ULast) {
        _STL_REPORT_ERROR("invalid heap in sort_heap()");
    }
#endif // _ITERATOR_DEBUG_LEVEL == 2
    _Sort_heap_unchecked(_UFirst, _ULast, _Pass_fn(_Pred));
}

_EXPORT_STD template <class _RanIt>
_CONSTEXPR20 void sort_heap(_RanIt _First, _RanIt _Last) { // order heap by repeatedly popping
    _STD sort_heap(_First, _Last, less<>{});
}

#ifdef __cpp_lib_concepts
namespace ranges {
    template <random_access_iterator _It, class _Pr, class _Pj>
        requires sortable<_It, _Pr, _Pj>
    constexpr void _Sort_heap_common(const _It _First, _It _Last, _Pr _Pred, _Pj _Proj) {
        // order heap by repeatedly popping
        for (; _Last - _First >= 2; --_Last) {
            _RANGES _Pop_heap_unchecked(_First, _Last, _Pred, _Proj);
        }
    }

    class _Sort_heap_fn : private _Not_quite_object {
    public:
        using _Not_quite_object::_Not_quite_object;

        template <random_access_iterator _It, sentinel_for<_It> _Se, class _Pr = ranges::less, class _Pj = identity>
            requires sortable<_It, _Pr, _Pj>
        constexpr _It operator()(_It _First, _Se _Last, _Pr _Pred = {}, _Pj _Proj = {}) const {
            _Adl_verify_range(_First, _Last);
            auto _UFirst = _Unwrap_iter<_Se>(_STD move(_First));
            auto _ULast = _Get_final_iterator_unwrapped<_It>(_UFirst, _STD move(_Last));
            _Seek_wrapped(_First, _ULast);
            _Sort_heap_common(_STD move(_UFirst), _STD move(_ULast), _Pass_fn(_Pred), _Pass_fn(_Proj));
            return _First;
        }

        template <random_access_range _Rng, class _Pr = ranges::less, class _Pj = identity>
            requires sortable<iterator_t<_Rng>, _Pr, _Pj>
        constexpr borrowed_iterator_t<_Rng> operator()(_Rng&& _Range, _Pr _Pred = {}, _Pj _Proj = {}) const {
            if constexpr (common_range<_Rng>) {
                _Sort_heap_common(_Ubegin(_Range), _Uend(_Range), _Pass_fn(_Pred), _Pass_fn(_Proj));
                return _RANGES end(_Range);
            }
            else {
                auto _ULast = _Get_final_iterator_unwrapped(_Range);
                _Sort_heap_common(_Ubegin(_Range), _ULast, _Pass_fn(_Pred), _Pass_fn(_Proj));
                return _Rewrap_iterator(_Range, _STD move(_ULast));
            }
        }
    };

    _EXPORT_STD inline constexpr _Sort_heap_fn sort_heap{ _Not_quite_object::_Construct_tag{} };

    template <class _It, class _Ty, class _Pr, class _Pj>
    _NODISCARD constexpr _It _Lower_bound_unchecked(
        _It _First, iter_difference_t<_It> _Count, const _Ty& _Val, _Pr _Pred, _Pj _Proj) {
        _STL_INTERNAL_STATIC_ASSERT(forward_iterator<_It>);
        _STL_INTERNAL_STATIC_ASSERT(indirect_strict_weak_order<_Pr, const _Ty*, projected<_It, _Pj>>);

        using _Diff = iter_difference_t<_It>;

        while (_Count > 0) { // divide and conquer, check midpoint
            const auto _Half = static_cast<_Diff>(_Count / 2);
            auto _Mid = _RANGES next(_First, _Half);
            if (_STD invoke(_Pred, _STD invoke(_Proj, *_Mid), _Val)) { // try top half
                _First = _STD move(_Mid);
                ++_First;
                _Count -= static_cast<_Diff>(_Half + 1);
            }
            else { // try bottom half
                _Count = _Half;
            }
        }

        return _First;
    }

    class _Lower_bound_fn : private _Not_quite_object {
    public:
        using _Not_quite_object::_Not_quite_object;

        template <forward_iterator _It, sentinel_for<_It> _Se, class _Ty, class _Pj = identity,
            indirect_strict_weak_order<const _Ty*, projected<_It, _Pj>> _Pr = ranges::less>
        _NODISCARD constexpr _It operator()(
            _It _First, _Se _Last, const _Ty& _Val, _Pr _Pred = {}, _Pj _Proj = {}) const {
            _Adl_verify_range(_First, _Last);
            auto _UFirst = _Unwrap_iter<_Se>(_STD move(_First));
            const auto _Count = _RANGES distance(_UFirst, _Unwrap_sent<_It>(_STD move(_Last)));
            _UFirst =
                _RANGES _Lower_bound_unchecked(_STD move(_UFirst), _Count, _Val, _Pass_fn(_Pred), _Pass_fn(_Proj));
            _Seek_wrapped(_First, _STD move(_UFirst));
            return _First;
        }

        template <forward_range _Rng, class _Ty, class _Pj = identity,
            indirect_strict_weak_order<const _Ty*, projected<iterator_t<_Rng>, _Pj>> _Pr = ranges::less>
        _NODISCARD constexpr borrowed_iterator_t<_Rng> operator()(
            _Rng&& _Range, const _Ty& _Val, _Pr _Pred = {}, _Pj _Proj = {}) const {
            const auto _Count = _RANGES distance(_Range);
            auto _UResult =
                _RANGES _Lower_bound_unchecked(_Ubegin(_Range), _Count, _Val, _Pass_fn(_Pred), _Pass_fn(_Proj));
            return _Rewrap_iterator(_Range, _STD move(_UResult));
        }
    };

    _EXPORT_STD inline constexpr _Lower_bound_fn lower_bound{ _Not_quite_object::_Construct_tag{} };

    template <class _It, class _Ty, class _Pr, class _Pj>
    _NODISCARD constexpr _It _Upper_bound_unchecked(
        _It _First, iter_difference_t<_It> _Count, const _Ty& _Val, _Pr _Pred, _Pj _Proj) {
        _STL_INTERNAL_STATIC_ASSERT(forward_iterator<_It>);
        _STL_INTERNAL_STATIC_ASSERT(indirect_strict_weak_order<_Pr, const _Ty*, projected<_It, _Pj>>);

        using _Diff = iter_difference_t<_It>;

        while (_Count > 0) { // divide and conquer: find half that contains answer
            const auto _Half = static_cast<_Diff>(_Count / 2);
            auto _Mid = _RANGES next(_First, _Half);
            if (_STD invoke(_Pred, _Val, _STD invoke(_Proj, *_Mid))) {
                _Count = _Half;
            }
            else { // try top half
                _First = _STD move(_Mid);
                ++_First;
                _Count -= static_cast<_Diff>(_Half + 1);
            }
        }

        return _First;
    }

    class _Upper_bound_fn : private _Not_quite_object {
    public:
        using _Not_quite_object::_Not_quite_object;

        template <forward_iterator _It, sentinel_for<_It> _Se, class _Ty, class _Pj = identity,
            indirect_strict_weak_order<const _Ty*, projected<_It, _Pj>> _Pr = ranges::less>
        _NODISCARD constexpr _It operator()(
            _It _First, _Se _Last, const _Ty& _Val, _Pr _Pred = {}, _Pj _Proj = {}) const {
            _Adl_verify_range(_First, _Last);
            auto _UFirst = _Unwrap_iter<_Se>(_STD move(_First));
            const auto _Count = _RANGES distance(_UFirst, _Unwrap_sent<_It>(_STD move(_Last)));
            _UFirst =
                _RANGES _Upper_bound_unchecked(_STD move(_UFirst), _Count, _Val, _Pass_fn(_Pred), _Pass_fn(_Proj));
            _Seek_wrapped(_First, _STD move(_UFirst));
            return _First;
        }

        template <forward_range _Rng, class _Ty, class _Pj = identity,
            indirect_strict_weak_order<const _Ty*, projected<iterator_t<_Rng>, _Pj>> _Pr = ranges::less>
        _NODISCARD constexpr borrowed_iterator_t<_Rng> operator()(
            _Rng&& _Range, const _Ty& _Val, _Pr _Pred = {}, _Pj _Proj = {}) const {
            const auto _Count = _RANGES distance(_Range);
            auto _UResult =
                _RANGES _Upper_bound_unchecked(_Ubegin(_Range), _Count, _Val, _Pass_fn(_Pred), _Pass_fn(_Proj));
            return _Rewrap_iterator(_Range, _STD move(_UResult));
        }
    };

    _EXPORT_STD inline constexpr _Upper_bound_fn upper_bound{ _Not_quite_object::_Construct_tag{} };
} // namespace ranges
#endif // __cpp_lib_concepts

_EXPORT_STD template <class _FwdIt, class _Ty, class _Pr>
_NODISCARD _CONSTEXPR20 pair<_FwdIt, _FwdIt> equal_range(_FwdIt _First, _FwdIt _Last, const _Ty& _Val, _Pr _Pred) {
    // find range equivalent to _Val
    _Adl_verify_range(_First, _Last);
    auto _UFirst = _Get_unwrapped(_First);
    const auto _ULast = _Get_unwrapped(_Last);

    using _Diff = _Iter_diff_t<_FwdIt>;
    _Diff _Count = _STD distance(_UFirst, _ULast);

    for (;;) { // divide and conquer, check midpoint
        if (_Count <= 0) {
            _Seek_wrapped(_Last, _UFirst); // empty range
            _Seek_wrapped(_First, _UFirst);
            break;
        }

        _Diff _Count2 = _Count / 2;
        const auto _UMid = _STD next(_UFirst, _Count2);
        if (_DEBUG_LT_PRED(_Pred, *_UMid, _Val)) { // range begins above _UMid, loop
            _UFirst = _Next_iter(_UMid);
            _Count -= _Count2 + 1;
        }
        else if (_Pred(_Val, *_UMid)) {
            _Count = _Count2; // range in first half, loop
        }
        else { // range straddles _UMid, find each end and return
            auto _UFirst2 = _STD lower_bound(_UFirst, _UMid, _Val, _Pass_fn(_Pred));
            _STD advance(_UFirst, _Count);
            auto _ULast2 = _STD upper_bound(_Next_iter(_UMid), _UFirst, _Val, _Pass_fn(_Pred));
            _Seek_wrapped(_Last, _ULast2);
            _Seek_wrapped(_First, _UFirst2);
            break;
        }
    }

    return { _First, _Last };
}

_EXPORT_STD template <class _FwdIt, class _Ty>
_NODISCARD _CONSTEXPR20 pair<_FwdIt, _FwdIt> equal_range(_FwdIt _First, _FwdIt _Last, const _Ty& _Val) {
    // find range equivalent to _Val
    return _STD equal_range(_First, _Last, _Val, less<>{});
}

#ifdef __cpp_lib_concepts
namespace ranges {
    class _Equal_range_fn : private _Not_quite_object {
    public:
        using _Not_quite_object::_Not_quite_object;

        template <forward_iterator _It, sentinel_for<_It> _Se, class _Ty, class _Pj = identity,
            indirect_strict_weak_order<const _Ty*, projected<_It, _Pj>> _Pr = ranges::less>
        _NODISCARD constexpr subrange<_It> operator()(
            _It _First, _Se _Last, const _Ty& _Val, _Pr _Pred = {}, _Pj _Proj = {}) const {
            _Adl_verify_range(_First, _Last);
            auto _UFirst = _Unwrap_iter<_Se>(_STD move(_First));
            const auto _Count = _RANGES distance(_UFirst, _Unwrap_sent<_It>(_STD move(_Last)));
            auto _UResult = _Equal_range_unchecked(_STD move(_UFirst), _Count, _Val, _Pass_fn(_Pred), _Pass_fn(_Proj));
            return _Rewrap_subrange<subrange<_It>>(_First, _STD move(_UResult));
        }

        template <forward_range _Rng, class _Ty, class _Pj = identity,
            indirect_strict_weak_order<const _Ty*, projected<iterator_t<_Rng>, _Pj>> _Pr = ranges::less>
        _NODISCARD constexpr borrowed_subrange_t<_Rng> operator()(
            _Rng&& _Range, const _Ty& _Val, _Pr _Pred = {}, _Pj _Proj = {}) const {
            const auto _Count = _RANGES distance(_Range);
            auto _UResult = _Equal_range_unchecked(_Ubegin(_Range), _Count, _Val, _Pass_fn(_Pred), _Pass_fn(_Proj));
            auto _Result = _RANGES begin(_Range);
            return _Rewrap_subrange<borrowed_subrange_t<_Rng>>(_Result, _STD move(_UResult));
        }

    private:
        template <class _It, class _Ty, class _Pr, class _Pj>
        _NODISCARD static constexpr subrange<_It> _Equal_range_unchecked(
            _It _First, iter_difference_t<_It> _Count, const _Ty& _Val, _Pr _Pred, _Pj _Proj) {
            _STL_INTERNAL_STATIC_ASSERT(forward_iterator<_It>);
            _STL_INTERNAL_STATIC_ASSERT(indirect_strict_weak_order<_Pr, const _Ty*, projected<_It, _Pj>>);

            using _Diff = iter_difference_t<_It>;

            while (_Count > 0) { // divide and conquer, check midpoint
                const auto _Half = static_cast<_Diff>(_Count / 2);
                auto _Mid = _RANGES next(_First, _Half);

                if (_STD invoke(_Pred, _STD invoke(_Proj, *_Mid), _Val)) { // range in second half
                    _First = _STD move(_Mid);
                    ++_First;
                    _Count -= static_cast<_Diff>(_Half + 1);
                }
                else if (_STD invoke(_Pred, _Val, _STD invoke(_Proj, *_Mid))) {
                    _Count = _Half; // range in first half
                }
                else { // range straddles _Mid, find the ends
                    _First = _RANGES _Lower_bound_unchecked(_STD move(_First), _Half, _Val, _Pred, _Proj);
                    ++_Mid;
                    _Count -= static_cast<_Diff>(_Half + 1);
                    _Mid = _RANGES _Upper_bound_unchecked(_STD move(_Mid), _Count, _Val, _Pred, _Proj);
                    return { _STD move(_First), _STD move(_Mid) };
                }
            }

            return { _First, _First };
        }
    };

    _EXPORT_STD inline constexpr _Equal_range_fn equal_range{ _Not_quite_object::_Construct_tag{} };
} // namespace ranges
#endif // __cpp_lib_concepts

_EXPORT_STD template <class _FwdIt, class _Ty, class _Pr>
_NODISCARD _CONSTEXPR20 bool binary_search(_FwdIt _First, _FwdIt _Last, const _Ty& _Val, _Pr _Pred) {
    // test if _Val equivalent to some element
    _Adl_verify_range(_First, _Last);
    auto _UFirst = _Get_unwrapped(_First);
    const auto _ULast = _Get_unwrapped(_Last);
    _UFirst = _STD lower_bound(_UFirst, _ULast, _Val, _Pass_fn(_Pred));
    return _UFirst != _ULast && !_Pred(_Val, *_UFirst);
}

_EXPORT_STD template <class _FwdIt, class _Ty>
_NODISCARD _CONSTEXPR20 bool binary_search(_FwdIt _First, _FwdIt _Last, const _Ty& _Val) {
    // test if _Val equivalent to some element
    return _STD binary_search(_First, _Last, _Val, less<>{});
}

#ifdef __cpp_lib_concepts
namespace ranges {
    class _Binary_search_fn : private _Not_quite_object {
    public:
        using _Not_quite_object::_Not_quite_object;

        template <forward_iterator _It, sentinel_for<_It> _Se, class _Ty, class _Pj = identity,
            indirect_strict_weak_order<const _Ty*, projected<_It, _Pj>> _Pr = ranges::less>
        _NODISCARD constexpr bool operator()(
            _It _First, _Se _Last, const _Ty& _Val, _Pr _Pred = {}, _Pj _Proj = {}) const {
            _Adl_verify_range(_First, _Last);
            auto _UFirst = _Unwrap_iter<_Se>(_STD move(_First));
            const auto _ULast = _Unwrap_sent<_It>(_STD move(_Last));
            const auto _Count = _RANGES distance(_UFirst, _ULast);
            _UFirst =
                _RANGES _Lower_bound_unchecked(_STD move(_UFirst), _Count, _Val, _Pass_fn(_Pred), _Pass_fn(_Proj));
            return _UFirst != _ULast && !_STD invoke(_Pred, _Val, _STD invoke(_Proj, *_UFirst));
        }

        template <forward_range _Rng, class _Ty, class _Pj = identity,
            indirect_strict_weak_order<const _Ty*, projected<iterator_t<_Rng>, _Pj>> _Pr = ranges::less>
        _NODISCARD constexpr bool operator()(_Rng&& _Range, const _Ty& _Val, _Pr _Pred = {}, _Pj _Proj = {}) const {
            const auto _Count = _RANGES distance(_Range);
            const auto _UFirst =
                _RANGES _Lower_bound_unchecked(_Ubegin(_Range), _Count, _Val, _Pass_fn(_Pred), _Pass_fn(_Proj));
            return _UFirst != _Uend(_Range) && !_STD invoke(_Pred, _Val, _STD invoke(_Proj, *_UFirst));
        }
    };

    _EXPORT_STD inline constexpr _Binary_search_fn binary_search{ _Not_quite_object::_Construct_tag{} };
} // namespace ranges
#endif // __cpp_lib_concepts

_NODISCARD constexpr _Distance_unknown _Idl_dist_add(_Distance_unknown, _Distance_unknown) {
    return {};
}

template <class _Diff1>
_NODISCARD constexpr _Distance_unknown _Idl_dist_add(_Diff1, _Distance_unknown) {
    return {};
}

template <class _Diff2>
_NODISCARD constexpr _Distance_unknown _Idl_dist_add(_Distance_unknown, _Diff2) {
    return {};
}

template <class _Diff1, class _Diff2>
_NODISCARD constexpr auto _Idl_dist_add(_Diff1 _Lhs, _Diff2 _Rhs) {
    return _Lhs + _Rhs;
}

_EXPORT_STD template <class _InIt1, class _InIt2, class _OutIt, class _Pr>
_CONSTEXPR20 _OutIt merge(_InIt1 _First1, _InIt1 _Last1, _InIt2 _First2, _InIt2 _Last2, _OutIt _Dest, _Pr _Pred) {
    // copy merging ranges
    _Adl_verify_range(_First1, _Last1);
    _Adl_verify_range(_First2, _Last2);
    auto _UFirst1 = _Get_unwrapped(_First1);
    const auto _ULast1 = _Get_unwrapped(_Last1);
    auto _UFirst2 = _Get_unwrapped(_First2);
    const auto _ULast2 = _Get_unwrapped(_Last2);
    _DEBUG_ORDER_SET_UNWRAPPED(_InIt2, _UFirst1, _ULast1, _Pred);
    _DEBUG_ORDER_SET_UNWRAPPED(_InIt1, _UFirst2, _ULast2, _Pred);
    const auto _Count1 = _Idl_distance<_InIt1>(_UFirst1, _ULast1);
    const auto _Count2 = _Idl_distance<_InIt2>(_UFirst2, _ULast2);
    auto _UDest = _Get_unwrapped_n(_Dest, _Idl_dist_add(_Count1, _Count2));
    if (_UFirst1 != _ULast1 && _UFirst2 != _ULast2) {
        for (;;) {
            if (_DEBUG_LT_PRED(_Pred, *_UFirst2, *_UFirst1)) {
                *_UDest = *_UFirst2;
                ++_UDest;
                ++_UFirst2;

                if (_UFirst2 == _ULast2) {
                    break;
                }
            }
            else {
                *_UDest = *_UFirst1;
                ++_UDest;
                ++_UFirst1;

                if (_UFirst1 == _ULast1) {
                    break;
                }
            }
        }
    }

    _UDest = _STD _Copy_unchecked(_UFirst1, _ULast1, _UDest); // copy any tail
    _Seek_wrapped(_Dest, _STD _Copy_unchecked(_UFirst2, _ULast2, _UDest));
    return _Dest;
}

_EXPORT_STD template <class _InIt1, class _InIt2, class _OutIt>
_CONSTEXPR20 _OutIt merge(_InIt1 _First1, _InIt1 _Last1, _InIt2 _First2, _InIt2 _Last2, _OutIt _Dest) {
    // copy merging ranges
    return _STD merge(_First1, _Last1, _First2, _Last2, _Dest, less<>{});
}

#if _HAS_CXX17
_EXPORT_STD template <class _ExPo, class _FwdIt1, class _FwdIt2, class _FwdIt3, class _Pr,
    _Enable_if_execution_policy_t<_ExPo> = 0>
    _FwdIt3 merge(_ExPo&&, _FwdIt1 _First1, _FwdIt1 _Last1, _FwdIt2 _First2, _FwdIt2 _Last2, _FwdIt3 _Dest,
        _Pr _Pred) noexcept /* terminates */ {
    // copy merging ranges
    // not parallelized at present, parallelism expected to be feasible in a future release
    _REQUIRE_PARALLEL_ITERATOR(_FwdIt1);
    _REQUIRE_PARALLEL_ITERATOR(_FwdIt2);
    _REQUIRE_CPP17_MUTABLE_ITERATOR(_FwdIt3);
    return _STD merge(_First1, _Last1, _First2, _Last2, _Dest, _Pass_fn(_Pred));
}

_EXPORT_STD template <class _ExPo, class _FwdIt1, class _FwdIt2, class _FwdIt3,
    _Enable_if_execution_policy_t<_ExPo> = 0>
    _FwdIt3 merge(_ExPo&&, _FwdIt1 _First1, _FwdIt1 _Last1, _FwdIt2 _First2, _FwdIt2 _Last2, _FwdIt3 _Dest) noexcept
    /* terminates */ {
    // copy merging ranges
    // not parallelized at present, parallelism expected to be feasible in a future release
    _REQUIRE_PARALLEL_ITERATOR(_FwdIt1);
    _REQUIRE_PARALLEL_ITERATOR(_FwdIt2);
    _REQUIRE_CPP17_MUTABLE_ITERATOR(_FwdIt3);
    return _STD merge(_First1, _Last1, _First2, _Last2, _Dest);
}

#ifdef __cpp_lib_concepts
namespace ranges {
    _EXPORT_STD template <class _In1, class _In2, class _Out>
        using merge_result = in_in_out_result<_In1, _In2, _Out>;

    class _Merge_fn : private _Not_quite_object {
    public:
        using _Not_quite_object::_Not_quite_object;

        template <input_iterator _It1, sentinel_for<_It1> _Se1, input_iterator _It2, sentinel_for<_It2> _Se2,
            weakly_incrementable _Out, class _Pr = ranges::less, class _Pj1 = identity, class _Pj2 = identity>
            requires mergeable<_It1, _It2, _Out, _Pr, _Pj1, _Pj2>
        constexpr merge_result<_It1, _It2, _Out> operator()(_It1 _First1, _Se1 _Last1, _It2 _First2, _Se2 _Last2,
            _Out _Result, _Pr _Pred = {}, _Pj1 _Proj1 = {}, _Pj2 _Proj2 = {}) const {
            _Adl_verify_range(_First1, _Last1);
            _Adl_verify_range(_First2, _Last2);
            auto _UResult =
                _Merge_unchecked(_Unwrap_iter<_Se1>(_STD move(_First1)), _Unwrap_sent<_It1>(_STD move(_Last1)),
                    _Unwrap_iter<_Se2>(_STD move(_First2)), _Unwrap_sent<_It2>(_STD move(_Last2)), _STD move(_Result),
                    _Pass_fn(_Pred), _Pass_fn(_Proj1), _Pass_fn(_Proj2));
            _Seek_wrapped(_First1, _STD move(_UResult.in1));
            _Seek_wrapped(_First2, _STD move(_UResult.in2));
            return { _STD move(_First1), _STD move(_First2), _STD move(_UResult.out) };
        }

        template <input_range _Rng1, input_range _Rng2, weakly_incrementable _Out, class _Pr = ranges::less,
            class _Pj1 = identity, class _Pj2 = identity>
            requires mergeable<iterator_t<_Rng1>, iterator_t<_Rng2>, _Out, _Pr, _Pj1, _Pj2>
        constexpr merge_result<borrowed_iterator_t<_Rng1>, borrowed_iterator_t<_Rng2>, _Out> operator()(
            _Rng1&& _Range1, _Rng2&& _Range2, _Out _Result, _Pr _Pred = {}, _Pj1 _Proj1 = {}, _Pj2 _Proj2 = {}) const {
            auto _First1 = _RANGES begin(_Range1);
            auto _First2 = _RANGES begin(_Range2);
            auto _UResult = _Merge_unchecked(_Unwrap_range_iter<_Rng1>(_STD move(_First1)), _Uend(_Range1),
                _Unwrap_range_iter<_Rng2>(_STD move(_First2)), _Uend(_Range2), _STD move(_Result), _Pass_fn(_Pred),
                _Pass_fn(_Proj1), _Pass_fn(_Proj2));
            _Seek_wrapped(_First1, _STD move(_UResult.in1));
            _Seek_wrapped(_First2, _STD move(_UResult.in2));
            return { _STD move(_First1), _STD move(_First2), _STD move(_UResult.out) };
        }

    private:
        template <class _It1, class _Se1, class _It2, class _Se2, class _Out, class _Pr, class _Pj1, class _Pj2>
        _NODISCARD static constexpr merge_result<_It1, _It2, _Out> _Merge_unchecked(_It1 _First1, const _Se1 _Last1,
            _It2 _First2, const _Se2 _Last2, _Out _Result, _Pr _Pred, _Pj1 _Proj1, _Pj2 _Proj2) {
            _STL_INTERNAL_STATIC_ASSERT(input_iterator<_It1>);
            _STL_INTERNAL_STATIC_ASSERT(sentinel_for<_Se1, _It1>);
            _STL_INTERNAL_STATIC_ASSERT(input_iterator<_It2>);
            _STL_INTERNAL_STATIC_ASSERT(sentinel_for<_Se2, _It2>);
            _STL_INTERNAL_STATIC_ASSERT(weakly_incrementable<_Out>);
            _STL_INTERNAL_STATIC_ASSERT(mergeable<_It1, _It2, _Out, _Pr, _Pj1, _Pj2>);

            for (;; ++_Result) {
                if (_First1 == _Last1) {
                    auto _Copy_result =
                        _RANGES _Copy_unchecked(_STD move(_First2), _STD move(_Last2), _STD move(_Result));
                    return { _STD move(_First1), _STD move(_Copy_result.in), _STD move(_Copy_result.out) };
                }

                if (_First2 == _Last2) {
                    auto _Copy_result =
                        _RANGES _Copy_unchecked(_STD move(_First1), _STD move(_Last1), _STD move(_Result));
                    return { _STD move(_Copy_result.in), _STD move(_First2), _STD move(_Copy_result.out) };
                }

                if (_STD invoke(_Pred, _STD invoke(_Proj2, *_First2), _STD invoke(_Proj1, *_First1))) {
                    *_Result = *_First2;
                    ++_First2;
                }
                else {
                    *_Result = *_First1;
                    ++_First1;
                }
            }
        }
    };

    _EXPORT_STD inline constexpr _Merge_fn merge{ _Not_quite_object::_Construct_tag{} };
} // namespace ranges
#endif // __cpp_lib_concepts
#endif // _HAS_CXX17

// The "usual invariants" for the inplace_merge helpers below are:
// [_First, _Mid) and [_Mid, _Last) are sorted
// _Pred(*_Mid, *_First)             note: this means *_Mid is the "lowest" element
// _Pred(*prev(_Last), *prev(_Mid))  note: this means *prev(_Mid) is the "highest" element
// _Count1 == distance(_First, _Mid)
// _Count2 == distance(_Mid, _Last)
// _Count1 > 1
// _Count2 > 1
template <class _BidIt>
void _Rotate_one_right(_BidIt _First, _BidIt _Mid, _BidIt _Last) {
    // exchanges the range [_First, _Mid) with [_Mid, _Last)
    // pre: distance(_Mid, _Last) is 1
    _Iter_value_t<_BidIt> _Temp(_STD move(*_Mid));
    _Move_backward_unchecked(_First, _Mid, _Last);
    *_First = _STD move(_Temp);
}

template <class _BidIt>
void _Rotate_one_left(_BidIt _First, _BidIt _Mid, _BidIt _Last) {
    // exchanges the range [_First, _Mid) with [_Mid, _Last)
    // pre: distance(_First, _Mid) is 1
    _Iter_value_t<_BidIt> _Temp(_STD move(*_First));
    *_STD _Move_unchecked(_Mid, _Last, _First) = _STD move(_Temp);
}

template <class _BidIt, class _Pr>
void _Inplace_merge_buffer_left(
    _BidIt _First, _BidIt _Mid, _BidIt _Last, _Iter_value_t<_BidIt>* const _Temp_ptr, _Pr _Pred) {
    // move the range [_First, _Mid) to _Temp_ptr, and merge it with [_Mid, _Last) to _First
    // usual invariants apply
    using _Ptr_ty = _Iter_value_t<_BidIt>*;
    _Uninitialized_backout<_Ptr_ty> _Backout{ _Temp_ptr, _STD _Uninitialized_move_unchecked(_First, _Mid, _Temp_ptr) };
    _Ptr_ty _Left_first = _Temp_ptr;
    const _Ptr_ty _Left_last = _Backout._Last - 1; // avoid a compare with the highest element
    *_First = _STD move(*_Mid); // the lowest element is now in position
    ++_First;
    ++_Mid;
    for (;;) {
        if (_Pred(*_Mid, *_Left_first)) { // take element from the right partition
            *_First = _STD move(*_Mid);
            ++_First;
            ++_Mid;
            if (_Mid == _Last) {
                _STD _Move_unchecked(_Left_first, _Backout._Last, _First); // move any tail (and the highest element)
                return;
            }
        }
        else { // take element from the left partition
            *_First = _STD move(*_Left_first);
            ++_First;
            ++_Left_first;
            if (_Left_first == _Left_last) {
                // move the remaining right partition and highest element, since *_Left_first is highest
                *_STD _Move_unchecked(_Mid, _Last, _First) = _STD move(*_Left_last);
                return;
            }
        }
    }
}

template <class _BidIt, class _Pr>
void _Inplace_merge_buffer_right(
    _BidIt _First, _BidIt _Mid, _BidIt _Last, _Iter_value_t<_BidIt>* const _Temp_ptr, _Pr _Pred) {
    // move the range [_Mid, _Last) to _Temp_ptr, and merge it with [_First, _Mid) to _Last
    // usual invariants apply
    using _Ptr_ty = _Iter_value_t<_BidIt>*;
    _Uninitialized_backout<_Ptr_ty> _Backout{ _Temp_ptr, _STD _Uninitialized_move_unchecked(_Mid, _Last, _Temp_ptr) };
    *--_Last = _STD move(*--_Mid); // move the highest element into position
    const _Ptr_ty _Right_first = _Temp_ptr;
    _Ptr_ty _Right_last = _Backout._Last - 1;
    --_Mid;
    for (;;) {
        if (_Pred(*_Right_last, *_Mid)) { // merge from the left partition
            *--_Last = _STD move(*_Mid);
            if (_First == _Mid) {
                *--_Last = _STD move(*_Right_last); // to make [_Right_first, _Right_last) a half-open range
                _Move_backward_unchecked(_Right_first, _Right_last, _Last); // move any head (and lowest element)
                return;
            }

            --_Mid;
        }
        else { // merge from the right partition
            *--_Last = _STD move(*_Right_last);
            --_Right_last;
            if (_Right_first == _Right_last) { // we can't compare with *_Right_first, but we know it is lowest
                *--_Last = _STD move(*_Mid); // restore half-open range [_First, _Mid)
                _Move_backward_unchecked(_First, _Mid, _Last);
                *_First = _STD move(*_Right_first);
                return;
            }
        }
    }
}

template <class _BidIt, class _Pr>
void _Buffered_inplace_merge_unchecked(_BidIt _First, _BidIt _Mid, _BidIt _Last, _Iter_diff_t<_BidIt> _Count1,
    _Iter_diff_t<_BidIt> _Count2, _Iter_value_t<_BidIt>* _Temp_ptr, ptrdiff_t _Capacity, _Pr _Pred);

template <class _BidIt, class _Pr>
void _Buffered_inplace_merge_divide_and_conquer2(_BidIt _First, _BidIt _Mid, _BidIt _Last, _Iter_diff_t<_BidIt> _Count1,
    _Iter_diff_t<_BidIt> _Count2, _Iter_value_t<_BidIt>* const _Temp_ptr, const ptrdiff_t _Capacity, _Pr _Pred,
    _BidIt _Firstn, _BidIt _Lastn, _Iter_diff_t<_BidIt> _Count1n, _Iter_diff_t<_BidIt> _Count2n) {
    // common block of _Buffered_inplace_merge_divide_and_conquer, below
    using _Diff = _Iter_diff_t<_BidIt>;
    _BidIt _Midn = _Buffered_rotate_unchecked(_Firstn, _Mid, _Lastn, static_cast<_Diff>(_Count1 - _Count1n), _Count2n,
        _Temp_ptr, _Capacity); // rearrange middle
    _Buffered_inplace_merge_unchecked(
        _First, _Firstn, _Midn, _Count1n, _Count2n, _Temp_ptr, _Capacity, _Pred); // merge each new part
    _Buffered_inplace_merge_unchecked(_Midn, _Lastn, _Last, static_cast<_Diff>(_Count1 - _Count1n),
        static_cast<_Diff>(_Count2 - _Count2n), _Temp_ptr, _Capacity, _Pred);
}

template <class _BidIt, class _Pr>
void _Buffered_inplace_merge_divide_and_conquer(_BidIt _First, _BidIt _Mid, _BidIt _Last, _Iter_diff_t<_BidIt> _Count1,
    _Iter_diff_t<_BidIt> _Count2, _Iter_value_t<_BidIt>* const _Temp_ptr, const ptrdiff_t _Capacity, _Pr _Pred) {
    // merge sorted [_First, _Mid) with sorted [_Mid, _Last)
    // usual invariants apply
    using _Diff = _Iter_diff_t<_BidIt>;
    if (_Count1 <= _Count2) {
        const _Diff _Count1n = _Count1 >> 1; // shift for codegen
        const _BidIt _Firstn = _STD next(_First, _Count1n);
        const _BidIt _Lastn = _STD lower_bound(_Mid, _Last, *_Firstn, _Pred);
        _STD _Buffered_inplace_merge_divide_and_conquer2(_First, _Mid, _Last, _Count1, _Count2, _Temp_ptr, _Capacity,
            _Pred, _Firstn, _Lastn, _Count1n, _STD distance(_Mid, _Lastn));
    }
    else {
        const _Diff _Count2n = _Count2 >> 1; // shift for codegen
        const _BidIt _Lastn = _STD next(_Mid, _Count2n);
        const _BidIt _Firstn = _STD upper_bound(_First, _Mid, *_Lastn, _Pred);
        _STD _Buffered_inplace_merge_divide_and_conquer2(_First, _Mid, _Last, _Count1, _Count2, _Temp_ptr, _Capacity,
            _Pred, _Firstn, _Lastn, _STD distance(_First, _Firstn), _Count2n);
    }
}

template <class _BidIt, class _Pr>
void _Buffered_inplace_merge_unchecked_impl(_BidIt _First, _BidIt _Mid, _BidIt _Last, _Iter_diff_t<_BidIt> _Count1,
    _Iter_diff_t<_BidIt> _Count2, _Iter_value_t<_BidIt>* const _Temp_ptr, const ptrdiff_t _Capacity, _Pr _Pred) {
    // merge sorted [_First, _Mid) with sorted [_Mid, _Last)
    // usual invariants apply
    if (_Count1 <= _Count2 && _Count1 <= _Capacity) {
        _STD _Inplace_merge_buffer_left(_First, _Mid, _Last, _Temp_ptr, _Pred);
    }
    else if (_Count2 <= _Capacity) {
        _STD _Inplace_merge_buffer_right(_First, _Mid, _Last, _Temp_ptr, _Pred);
    }
    else {
        _STD _Buffered_inplace_merge_divide_and_conquer(
            _First, _Mid, _Last, _Count1, _Count2, _Temp_ptr, _Capacity, _Pred);
    }
}

template <class _BidIt, class _Pr>
void _Buffered_inplace_merge_unchecked(_BidIt _First, _BidIt _Mid, _BidIt _Last, _Iter_diff_t<_BidIt> _Count1,
    _Iter_diff_t<_BidIt> _Count2, _Iter_value_t<_BidIt>* const _Temp_ptr, const ptrdiff_t _Capacity, _Pr _Pred) {
    // merge sorted [_First, _Mid) with sorted [_Mid, _Last)
    // usual invariants *do not* apply; only sortedness applies
    // establish the usual invariants (explained in inplace_merge)
    if (_Mid == _Last) {
        return;
    }

    for (;;) {
        if (_First == _Mid) {
            return;
        }

        if (_Pred(*_Mid, *_First)) {
            break;
        }

        ++_First;
        --_Count1;
    }

    const auto _Highest = _Prev_iter(_Mid);
    do {
        --_Last;
        --_Count2;
        if (_Mid == _Last) {
            _STD _Rotate_one_right(_First, _Mid, ++_Last);
            return;
        }
    } while (!_Pred(*_Last, *_Highest));

    ++_Last;
    ++_Count2;

    if (_Count1 == 1) {
        _STD _Rotate_one_left(_First, _Mid, _Last);
        return;
    }

    _Buffered_inplace_merge_unchecked_impl(_First, _Mid, _Last, _Count1, _Count2, _Temp_ptr, _Capacity, _Pred);
}

_EXPORT_STD template <class _BidIt, class _Pr>
void inplace_merge(_BidIt _First, _BidIt _Mid, _BidIt _Last, _Pr _Pred) {
    // merge [_First, _Mid) with [_Mid, _Last)
    _Adl_verify_range(_First, _Mid);
    _Adl_verify_range(_Mid, _Last);
    auto _UFirst = _Get_unwrapped(_First);
    auto _UMid = _Get_unwrapped(_Mid);
    auto _ULast = _Get_unwrapped(_Last);
    _DEBUG_ORDER_UNWRAPPED(_UFirst, _UMid, _Pred);

    // establish the usual invariants:
    if (_UMid == _ULast) {
        return;
    }

    for (;;) {
        if (_UFirst == _UMid) {
            return;
        }

        if (_Pred(*_UMid, *_UFirst)) { // found that *_UMid goes in *_UFirst's position
            break;
        }

        ++_UFirst;
    }

    const auto _Highest = _Prev_iter(_UMid);
    do {
        --_ULast;
        if (_UMid == _ULast) { // rotate only element remaining in right partition to the beginning, without allocating
            _STD _Rotate_one_right(_UFirst, _UMid, ++_ULast);
            return;
        }
    } while (!_Pred(*_ULast, *_Highest)); // found that *_Highest goes in *_ULast's position

    ++_ULast;

    using _Diff = _Iter_diff_t<_BidIt>;
    const _Diff _Count1 = _STD distance(_UFirst, _UMid);
    if (_Count1 == 1) { // rotate only element remaining in left partition to the end, without allocating
        _STD _Rotate_one_left(_UFirst, _UMid, _ULast);
        return;
    }

    const _Diff _Count2 = _STD distance(_UMid, _ULast);
    _Optimistic_temporary_buffer<_Iter_value_t<_BidIt>> _Temp_buf{ (_STD min)(_Count1, _Count2) };
    _Buffered_inplace_merge_unchecked_impl(
        _UFirst, _UMid, _ULast, _Count1, _Count2, _Temp_buf._Data, _Temp_buf._Capacity, _Pass_fn(_Pred));
}

_EXPORT_STD template <class _BidIt>
void inplace_merge(_BidIt _First, _BidIt _Mid, _BidIt _Last) {
    // merge [_First, _Mid) with [_Mid, _Last)
    _STD inplace_merge(_First, _Mid, _Last, less<>{});
}

#if _HAS_CXX17
_EXPORT_STD template <class _ExPo, class _BidIt, class _Pr, _Enable_if_execution_policy_t<_ExPo> = 0>
void inplace_merge(_ExPo&&, _BidIt _First, _BidIt _Mid, _BidIt _Last, _Pr _Pred) noexcept /* terminates */ {
    // merge [_First, _Mid) with [_Mid, _Last)
    // not parallelized at present, parallelism expected to be feasible in a future release
    _STD inplace_merge(_First, _Mid, _Last, _Pass_fn(_Pred));
}

_EXPORT_STD template <class _ExPo, class _BidIt, _Enable_if_execution_policy_t<_ExPo> = 0>
void inplace_merge(_ExPo&&, _BidIt _First, _BidIt _Mid, _BidIt _Last) noexcept /* terminates */ {
    // merge [_First, _Mid) with [_Mid, _Last)
    // not parallelized at present, parallelism expected to be feasible in a future release
    _STD inplace_merge(_First, _Mid, _Last);
}
#endif // _HAS_CXX17

#ifdef __cpp_lib_concepts
namespace ranges {
    template <forward_iterator _It, sentinel_for<_It> _Se, class _Pr, class _Pj>
        requires indirect_strict_weak_order<_Pr, projected<_It, _Pj>>
    _NODISCARD constexpr _It _Is_sorted_until_unchecked(_It _First, const _Se _Last, _Pr _Pred, _Pj _Proj) {
        if (_First == _Last) {
            return _First;
        }

        for (auto _Prev = _First; ++_First != _Last; ++_Prev) {
            if (_STD invoke(_Pred, _STD invoke(_Proj, *_First), _STD invoke(_Proj, *_Prev))) {
                break;
            }
        }

        return _First;
    }

    template <bidirectional_iterator _It>
    void _Rotate_one_right(_It _First, _It _Mid, _It _Last) {
        // exchanges the range [_First, _Mid) with [_Mid, _Last)
        _STL_INTERNAL_CHECK(_RANGES next(_Mid) == _Last);
        auto _Temp = _RANGES iter_move(_Mid);
        _RANGES _Move_backward_common(_First, _STD move(_Mid), _STD move(_Last));
        *_First = _STD move(_Temp);
    }

    template <bidirectional_iterator _It>
    void _Rotate_one_left(_It _First, _It _Mid, _It _Last) {
        // exchanges the range [_First, _Mid) with [_Mid, _Last)
        _STL_INTERNAL_CHECK(_RANGES next(_First) == _Mid);
        auto _Temp = _RANGES iter_move(_First);
        auto _Result = _RANGES _Move_unchecked(_STD move(_Mid), _STD move(_Last), _STD move(_First));
        *_Result.out = _STD move(_Temp);
    }

    template <bidirectional_iterator _It, class _Pr, class _Pj>
        requires sortable<_It, _Pr, _Pj>
    void _Inplace_merge_buffer_left(_It _First, _It _Mid, _It _Last, iter_value_t<_It>* _Left_first,
        const ptrdiff_t _Capacity, _Pr _Pred, _Pj _Proj) {
        // move the range [_First, _Mid) to _Left_first, and merge it with [_Mid, _Last) to _First
        // usual invariants apply
        using _Ty = iter_value_t<_It>;

        _Ty* _Left_last = _RANGES _Uninitialized_move_unchecked(_First, _Mid, _Left_first, _Left_first + _Capacity).out;
        _Uninitialized_backout<_Ty*> _Backout{ _Left_first, _Left_last };

        // We already know that _Backout._Last - 1 is the highest element, so do not compare against it again.
        --_Left_last;

        // We already know that _Mid points to the lowest element and that there is more than 1 element left.
        *_First = _RANGES iter_move(_Mid);
        ++_First;
        ++_Mid;

        for (;;) {
            if (_STD invoke(_Pred, _STD invoke(_Proj, *_Mid), _STD invoke(_Proj, *_Left_first))) {
                *_First = _RANGES iter_move(_Mid); // the lowest element is now in position
                ++_First;
                ++_Mid;
                if (_Mid == _Last) {
                    // move the remaining left partition
                    _RANGES _Move_unchecked(_Left_first, _Backout._Last, _First);
                    return;
                }
            }
            else {
                *_First = _RANGES iter_move(_Left_first);
                ++_First;
                ++_Left_first;
                if (_Left_first == _Left_last) {
                    // move the remaining right partition and highest element, since *_Left_first is highest
                    const auto _Final = _RANGES _Move_unchecked(_Mid, _Last, _First);
                    *_Final.out = _RANGES iter_move(_Left_first);
                    return;
                }
            }
        }
    }

    template <bidirectional_iterator _It, class _Pr, class _Pj>
        requires sortable<_It, _Pr, _Pj>
    void _Inplace_merge_buffer_right(_It _First, _It _Mid, _It _Last, iter_value_t<_It>* _Right_first,
        const ptrdiff_t _Capacity, _Pr _Pred, _Pj _Proj) {
        // move the range [_Mid, _Last) to _Right_first, and merge it with [_First, _Mid) to _Last
        // usual invariants apply
        using _Ty = iter_value_t<_It>;

        _Ty* _Right_last =
            _RANGES _Uninitialized_move_unchecked(_Mid, _Last, _Right_first, _Right_first + _Capacity).out;
        _Uninitialized_backout<_Ty*> _Backout{ _Right_first, _Right_last };

        // We already know that _Mid points to the next highest element and that there is more than 1 element left.
        *--_Last = _RANGES iter_move(--_Mid);

        // We already know that _Backout._Last - 1 is the highest element, so do not compare against it again.
        --_Mid;
        --_Right_last;
        for (;;) {
            if (_STD invoke(_Pred, _STD invoke(_Proj, *_Right_last), _STD invoke(_Proj, *_Mid))) {
                *--_Last = _RANGES iter_move(_Mid); // the lowest element is now in position
                if (_First == _Mid) {
                    ++_Right_last; // to make [_Right_first, _Right_last) a half-open range
                    _RANGES _Move_backward_common(_Right_first, _Right_last, _STD move(_Last));
                    return;
                }
                --_Mid;
            }
            else {
                *--_Last = _RANGES iter_move(_Right_last);
                --_Right_last;
                if (_Right_first == _Right_last) { // we can't compare with *_Right_first, but we know it is lowest
                    ++_Mid; // restore half-open range [_First, _Mid)
                    _RANGES _Move_backward_common(_First, _STD move(_Mid), _STD move(_Last));
                    *_First = _RANGES iter_move(_Right_first);
                    return;
                }
            }
        }
    }

    template <bidirectional_iterator _It, class _Pr, class _Pj>
        requires sortable<_It, _Pr, _Pj>
    void _Buffered_inplace_merge_common(_It _First, _It _Mid, _It _Last, iter_difference_t<_It> _Count1,
        iter_difference_t<_It> _Count2, iter_value_t<_It>* _Temp_ptr, ptrdiff_t _Capacity, _Pr _Pred, _Pj _Proj);

    template <bidirectional_iterator _It, class _Pr, class _Pj>
        requires sortable<_It, _Pr, _Pj>
    void _Buffered_inplace_merge_divide_and_conquer2(_It _First, _It _Mid, _It _Last,
        const iter_difference_t<_It> _Count1, const iter_difference_t<_It> _Count2, iter_value_t<_It>* const _Temp_ptr,
        const ptrdiff_t _Capacity, _Pr _Pred, _Pj _Proj, _It _Firstn, _It _Lastn, const iter_difference_t<_It> _Count1n,
        const iter_difference_t<_It> _Count2n) {
        // common block of _Buffered_inplace_merge_divide_and_conquer, below
        _It _Midn = _RANGES _Buffered_rotate_common(_Firstn, _Mid, _Lastn,
            static_cast<iter_difference_t<_It>>(_Count1 - _Count1n), _Count2n, _Temp_ptr,
            _Capacity); // rearrange middle
        _RANGES _Buffered_inplace_merge_common(
            _First, _Firstn, _Midn, _Count1n, _Count2n, _Temp_ptr, _Capacity, _Pred, _Proj); // merge each new part
        _RANGES _Buffered_inplace_merge_common(_Midn, _Lastn, _Last,
            static_cast<iter_difference_t<_It>>(_Count1 - _Count1n),
            static_cast<iter_difference_t<_It>>(_Count2 - _Count2n), _Temp_ptr, _Capacity, _Pred, _Proj);
    }

    template <bidirectional_iterator _It, class _Pr, class _Pj>
        requires sortable<_It, _Pr, _Pj>
    void _Buffered_inplace_merge_divide_and_conquer(_It _First, _It _Mid, _It _Last,
        const iter_difference_t<_It> _Count1, const iter_difference_t<_It> _Count2, iter_value_t<_It>* const _Temp_ptr,
        const ptrdiff_t _Capacity, _Pr _Pred, _Pj _Proj) {
        // merge sorted [_First, _Mid) with sorted [_Mid, _Last)
        // usual invariants apply
        if (_Count1 <= _Count2) {
            const iter_difference_t<_It> _Count1n = _Count1 >> 1; // shift for codegen
            _It _Firstn = _RANGES next(_First, _Count1n);
            _It _Lastn = _RANGES _Lower_bound_unchecked(_Mid, _Count1, _STD invoke(_Proj, *_Firstn), _Pred, _Proj);
            const auto _Count2n = _RANGES distance(_Mid, _Lastn);
            _RANGES _Buffered_inplace_merge_divide_and_conquer2(_STD move(_First), _STD move(_Mid), _STD move(_Last),
                _Count1, _Count2, _Temp_ptr, _Capacity, _Pred, _Proj, _STD move(_Firstn), _STD move(_Lastn), _Count1n,
                _Count2n);
        }
        else {
            const iter_difference_t<_It> _Count2n = _Count2 >> 1; // shift for codegen
            _It _Lastn = _RANGES next(_Mid, _Count2n);
            _It _Firstn = _RANGES _Upper_bound_unchecked(_First, _Count2, _STD invoke(_Proj, *_Lastn), _Pred, _Proj);
            const auto _Count1n = _RANGES distance(_First, _Firstn);
            _RANGES _Buffered_inplace_merge_divide_and_conquer2(_STD move(_First), _STD move(_Mid), _STD move(_Last),
                _Count1, _Count2, _Temp_ptr, _Capacity, _Pred, _Proj, _STD move(_Firstn), _STD move(_Lastn), _Count1n,
                _Count2n);
        }
    }

    template <bidirectional_iterator _It, class _Pr, class _Pj>
        requires sortable<_It, _Pr, _Pj>
    void _Buffered_inplace_merge_common(_It _First, _It _Mid, _It _Last, iter_difference_t<_It> _Count1,
        iter_difference_t<_It> _Count2, iter_value_t<_It>* const _Temp_ptr, const ptrdiff_t _Capacity, _Pr _Pred,
        _Pj _Proj) {
        // merge sorted [_First, _Mid) with sorted [_Mid, _Last)
        // usual invariants *do not* apply; only sortedness applies
        // establish the usual invariants
        if (_First == _Mid || _Mid == _Last) {
            return;
        }

        // Find first element in [_First, _Mid) that is greater than *_Mid
        while (!_STD invoke(_Pred, _STD invoke(_Proj, *_Mid), _STD invoke(_Proj, *_First))) {
            --_Count1;
            if (++_First == _Mid) {
                return;
            }
        }

        // Find last element in [_Mid, _Last) that is less than *--_Mid
        const auto _Highest = _RANGES prev(_Mid);
        do {
            // Fast early return if there is only one element to be moved
            if (_Mid == --_Last) {
                // rotate only element remaining in right partition to the beginning, without allocating
                _RANGES _Rotate_one_right(_STD move(_First), _STD move(_Mid), _STD move(++_Last));
                return;
            }
            --_Count2;
        } while (!_STD invoke(_Pred, _STD invoke(_Proj, *_Last), _STD invoke(_Proj, *_Highest)));
        ++_Last;
        ++_Count2;

        if (_Count1 == 1) {
            _RANGES _Rotate_one_left(_STD move(_First), _STD move(_Mid), _STD move(_Last));
            return;
        }

        if (_Count1 <= _Count2 && _Count1 <= _Capacity) {
            _RANGES _Inplace_merge_buffer_left(
                _STD move(_First), _STD move(_Mid), _STD move(_Last), _Temp_ptr, _Capacity, _Pred, _Proj);
        }
        else if (_Count2 <= _Capacity) {
            _RANGES _Inplace_merge_buffer_right(
                _STD move(_First), _STD move(_Mid), _STD move(_Last), _Temp_ptr, _Capacity, _Pred, _Proj);
        }
        else {
            _RANGES _Buffered_inplace_merge_divide_and_conquer(_STD move(_First), _STD move(_Mid), _STD move(_Last),
                _Count1, _Count2, _Temp_ptr, _Capacity, _Pred, _Proj);
        }
    }

    class _Inplace_merge_fn : private _Not_quite_object {
    public:
        using _Not_quite_object::_Not_quite_object;

        template <bidirectional_iterator _It, sentinel_for<_It> _Se, class _Pr = ranges::less, class _Pj = identity>
            requires sortable<_It, _Pr, _Pj>
        _It operator()(_It _First, _It _Mid, _Se _Last, _Pr _Pred = {}, _Pj _Proj = {}) const {
            _Adl_verify_range(_First, _Mid);
            _Adl_verify_range(_Mid, _Last);

            auto _UFirst = _Unwrap_iter<_Se>(_STD move(_First));
            auto _ULast = _Get_final_iterator_unwrapped<_It>(_UFirst, _STD move(_Last));
            _Seek_wrapped(_First, _ULast);

            _Inplace_merge_common(_STD move(_UFirst), _Unwrap_iter<_Se>(_STD move(_Mid)), _STD move(_ULast),
                _Pass_fn(_Pred), _Pass_fn(_Proj));
            return _First;
        }

        template <bidirectional_range _Rng, class _Pr = ranges::less, class _Pj = identity>
            requires sortable<iterator_t<_Rng>, _Pr, _Pj>
        borrowed_iterator_t<_Rng> operator()(
            _Rng&& _Range, iterator_t<_Rng> _Mid, _Pr _Pred = {}, _Pj _Proj = {}) const {
            auto _First = _RANGES begin(_Range);
            auto _Last = _RANGES end(_Range);

            _Adl_verify_range(_First, _Mid);
            _Adl_verify_range(_Mid, _Last);

            auto _UFirst = _Unwrap_range_iter<_Rng>(_STD move(_First));
            auto _ULast = _Get_final_iterator_unwrapped<iterator_t<_Rng>>(_UFirst, _STD move(_Last));
            _Seek_wrapped(_First, _ULast);

            _Inplace_merge_common(_STD move(_UFirst), _Unwrap_range_iter<_Rng>(_STD move(_Mid)), _STD move(_ULast),
                _Pass_fn(_Pred), _Pass_fn(_Proj));
            return _First;
        }

    private:
        template <class _It, class _Pr, class _Pj>
        static void _Inplace_merge_common(_It _First, _It _Mid, _It _Last, _Pr _Pred, _Pj _Proj) {
            _STL_INTERNAL_STATIC_ASSERT(bidirectional_iterator<_It>);
            _STL_INTERNAL_STATIC_ASSERT(sortable<_It, _Pr, _Pj>);

            if (_First == _Mid || _Mid == _Last) {
                return;
            }
#if _ITERATOR_DEBUG_LEVEL == 2
            _STL_VERIFY(_RANGES _Is_sorted_until_unchecked(_First, _Mid, _Pred, _Proj) == _Mid,
                "ranges::inplace_merge requires the range [first, middle) to be sorted");
            _STL_VERIFY(_RANGES _Is_sorted_until_unchecked(_Mid, _Last, _Pred, _Proj) == _Last,
                "ranges::inplace_merge requires the range [middle, last) to be sorted");
#endif // _ITERATOR_DEBUG_LEVEL == 2

            // Find first element in [_First, _Mid) that is greater than *_Mid
            while (!_STD invoke(_Pred, _STD invoke(_Proj, *_Mid), _STD invoke(_Proj, *_First))) {
                if (++_First == _Mid) {
                    return;
                }
            }

            // Fast early return if there is only one element to be moved
            if (_Mid == --_Last) {
                // rotate only element remaining in right partition to the beginning, without allocating
                _RANGES _Rotate_one_right(_STD move(_First), _STD move(_Mid), _STD move(++_Last));
                return;
            }

            // Find last element in [_Mid, _Last) that is less than *--_Mid
            const auto _Highest = _RANGES prev(_Mid);
            while (!_STD invoke(_Pred, _STD invoke(_Proj, *_Last), _STD invoke(_Proj, *_Highest))) {
                if (_Mid == --_Last) {
                    // rotate only element remaining in right partition to the beginning, without allocating
                    _RANGES _Rotate_one_right(_STD move(_First), _STD move(_Mid), _STD move(++_Last));
                    return;
                }
            }
            ++_Last;

            const iter_difference_t<_It> _Count1 = _RANGES distance(_First, _Mid);
            if (_Count1 == 1) { // rotate only element remaining in left partition to the end, without allocating
                _RANGES _Rotate_one_left(_STD move(_First), _STD move(_Mid), _STD move(_Last));
                return;
            }

            const iter_difference_t<_It> _Count2 = _RANGES distance(_Mid, _Last);
            _Optimistic_temporary_buffer<iter_value_t<_It>> _Temp_buf{ (_STD min)(_Count1, _Count2) };
            if (_Count1 <= _Count2 && _Count1 <= _Temp_buf._Capacity) {
                _RANGES _Inplace_merge_buffer_left(_STD move(_First), _STD move(_Mid), _STD move(_Last),
                    _Temp_buf._Data, _Temp_buf._Capacity, _Pred, _Proj);
            }
            else if (_Count2 <= _Temp_buf._Capacity) {
                _RANGES _Inplace_merge_buffer_right(_STD move(_First), _STD move(_Mid), _STD move(_Last),
                    _Temp_buf._Data, _Temp_buf._Capacity, _Pred, _Proj);
            }
            else {
                _RANGES _Buffered_inplace_merge_divide_and_conquer(_STD move(_First), _STD move(_Mid), _STD move(_Last),
                    _Count1, _Count2, _Temp_buf._Data, _Temp_buf._Capacity, _Pred, _Proj);
            }
        }
    };

    _EXPORT_STD inline constexpr _Inplace_merge_fn inplace_merge{ _Not_quite_object::_Construct_tag{} };
} // namespace ranges
#endif // __cpp_lib_concepts

template <class _BidIt, class _Pr>
_CONSTEXPR20 _BidIt _Insertion_sort_unchecked(const _BidIt _First, const _BidIt _Last, _Pr _Pred) {
    // insertion sort [_First, _Last)
    if (_First != _Last) {
        for (_BidIt _Mid = _First; ++_Mid != _Last;) { // order next element
            _BidIt _Hole = _Mid;
            _Iter_value_t<_BidIt> _Val = _STD move(*_Mid);

            if (_DEBUG_LT_PRED(_Pred, _Val, *_First)) { // found new earliest element, move to front
                _Move_backward_unchecked(_First, _Mid, ++_Hole);
                *_First = _STD move(_Val);
            }
            else { // look for insertion point after first
                for (_BidIt _Prev = _Hole; _DEBUG_LT_PRED(_Pred, _Val, *--_Prev); _Hole = _Prev) {
                    *_Hole = _STD move(*_Prev); // move hole down
                }

                *_Hole = _STD move(_Val); // insert element in hole
            }
        }
    }

    return _Last;
}

template <class _RanIt, class _Pr>
_CONSTEXPR20 void _Med3_unchecked(_RanIt _First, _RanIt _Mid, _RanIt _Last, _Pr _Pred) {
    // sort median of three elements to middle
    if (_DEBUG_LT_PRED(_Pred, *_Mid, *_First)) {
        swap(*_Mid, *_First); // intentional ADL
    }

    if (_DEBUG_LT_PRED(_Pred, *_Last, *_Mid)) { // swap middle and last, then test first again
        swap(*_Last, *_Mid); // intentional ADL

        if (_DEBUG_LT_PRED(_Pred, *_Mid, *_First)) {
            swap(*_Mid, *_First); // intentional ADL
        }
    }
}

template <class _RanIt, class _Pr>
_CONSTEXPR20 void _Guess_median_unchecked(_RanIt _First, _RanIt _Mid, _RanIt _Last, _Pr _Pred) {
    // sort median element to middle
    using _Diff = _Iter_diff_t<_RanIt>;
    const _Diff _Count = _Last - _First;
    if (40 < _Count) { // Tukey's ninther
        const _Diff _Step = (_Count + 1) >> 3; // +1 can't overflow because range was made inclusive in caller
        const _Diff _Two_step = _Step << 1; // note: intentionally discards low-order bit
        _Med3_unchecked(_First, _First + _Step, _First + _Two_step, _Pred);
        _Med3_unchecked(_Mid - _Step, _Mid, _Mid + _Step, _Pred);
        _Med3_unchecked(_Last - _Two_step, _Last - _Step, _Last, _Pred);
        _Med3_unchecked(_First + _Step, _Mid, _Last - _Step, _Pred);
    }
    else {
        _Med3_unchecked(_First, _Mid, _Last, _Pred);
    }
}

template <class _RanIt, class _Pr>
_CONSTEXPR20 pair<_RanIt, _RanIt> _Partition_by_median_guess_unchecked(_RanIt _First, _RanIt _Last, _Pr _Pred) {
    // partition [_First, _Last)
    _RanIt _Mid = _First + ((_Last - _First) >> 1); // shift for codegen
    _Guess_median_unchecked(_First, _Mid, _Prev_iter(_Last), _Pred);
    _RanIt _Pfirst = _Mid;
    _RanIt _Plast = _Next_iter(_Pfirst);

    while (_First < _Pfirst && !_DEBUG_LT_PRED(_Pred, *_Prev_iter(_Pfirst), *_Pfirst)
        && !_Pred(*_Pfirst, *_Prev_iter(_Pfirst))) {
        --_Pfirst;
    }

    while (_Plast < _Last && !_DEBUG_LT_PRED(_Pred, *_Plast, *_Pfirst) && !_Pred(*_Pfirst, *_Plast)) {
        ++_Plast;
    }

    _RanIt _Gfirst = _Plast;
    _RanIt _Glast = _Pfirst;

    for (;;) { // partition
        for (; _Gfirst < _Last; ++_Gfirst) {
            if (_DEBUG_LT_PRED(_Pred, *_Pfirst, *_Gfirst)) {
                continue;
            }
            else if (_Pred(*_Gfirst, *_Pfirst)) {
                break;
            }
            else if (_Plast != _Gfirst) {
                swap(*_Plast, *_Gfirst); // intentional ADL
                ++_Plast;
            }
            else {
                ++_Plast;
            }
        }

        for (; _First < _Glast; --_Glast) {
            const auto _Glast_prev = _Prev_iter(_Glast);
            if (_DEBUG_LT_PRED(_Pred, *_Glast_prev, *_Pfirst)) {
                continue;
            }
            else if (_Pred(*_Pfirst, *_Glast_prev)) {
                break;
            }
            else if (--_Pfirst != _Glast_prev) {
                swap(*_Pfirst, *_Glast_prev); // intentional ADL
            }
        }

        if (_Glast == _First && _Gfirst == _Last) {
            return pair<_RanIt, _RanIt>(_Pfirst, _Plast);
        }

        if (_Glast == _First) { // no room at bottom, rotate pivot upward
            if (_Plast != _Gfirst) {
                swap(*_Pfirst, *_Plast); // intentional ADL
            }

            ++_Plast;
            swap(*_Pfirst, *_Gfirst); // intentional ADL
            ++_Pfirst;
            ++_Gfirst;
        }
        else if (_Gfirst == _Last) { // no room at top, rotate pivot downward
            if (--_Glast != --_Pfirst) {
                swap(*_Glast, *_Pfirst); // intentional ADL
            }

            swap(*_Pfirst, *--_Plast); // intentional ADL
        }
        else {
            swap(*_Gfirst, *--_Glast); // intentional ADL
            ++_Gfirst;
        }
    }
}

template <class _RanIt, class _Pr>
_CONSTEXPR20 void _Sort_unchecked(_RanIt _First, _RanIt _Last, _Iter_diff_t<_RanIt> _Ideal, _Pr _Pred) {
    // order [_First, _Last)
    for (;;) {
        if (_Last - _First <= _ISORT_MAX) { // small
            _Insertion_sort_unchecked(_First, _Last, _Pred);
            return;
        }

        if (_Ideal <= 0) { // heap sort if too many divisions
            _Make_heap_unchecked(_First, _Last, _Pred);
            _Sort_heap_unchecked(_First, _Last, _Pred);
            return;
        }

        // divide and conquer by quicksort
        auto _Mid = _Partition_by_median_guess_unchecked(_First, _Last, _Pred);

        _Ideal = (_Ideal >> 1) + (_Ideal >> 2); // allow 1.5 log2(N) divisions

        if (_Mid.first - _First < _Last - _Mid.second) { // loop on second half
            _Sort_unchecked(_First, _Mid.first, _Ideal, _Pred);
            _First = _Mid.second;
        }
        else { // loop on first half
            _Sort_unchecked(_Mid.second, _Last, _Ideal, _Pred);
            _Last = _Mid.first;
        }
    }
}

_EXPORT_STD template <class _RanIt, class _Pr>
_CONSTEXPR20 void sort(const _RanIt _First, const _RanIt _Last, _Pr _Pred) { // order [_First, _Last)
    _Adl_verify_range(_First, _Last);
    const auto _UFirst = _Get_unwrapped(_First);
    const auto _ULast = _Get_unwrapped(_Last);
    _Sort_unchecked(_UFirst, _ULast, _ULast - _UFirst, _Pass_fn(_Pred));
}

_EXPORT_STD template <class _RanIt>
_CONSTEXPR20 void sort(const _RanIt _First, const _RanIt _Last) { // order [_First, _Last)
    _STD sort(_First, _Last, less<>{});
}

#if _HAS_CXX17
_EXPORT_STD template <class _ExPo, class _RanIt, class _Pr, _Enable_if_execution_policy_t<_ExPo> = 0>
void sort(_ExPo&& _Exec, _RanIt _First, _RanIt _Last, _Pr _Pred) noexcept; // terminates

_EXPORT_STD template <class _ExPo, class _RanIt, _Enable_if_execution_policy_t<_ExPo> = 0>
void sort(_ExPo&& _Exec, const _RanIt _First, const _RanIt _Last) noexcept /* terminates */ {
    // order [_First, _Last)
    _STD sort(_STD forward<_ExPo>(_Exec), _First, _Last, less{});
}
#endif // _HAS_CXX17

#ifdef __cpp_lib_concepts
namespace ranges {
    template <bidirectional_iterator _It, class _Pr, class _Pj>
        requires sortable<_It, _Pr, _Pj>
    constexpr _It _Insertion_sort_common(const _It _First, const _It _Last, _Pr _Pred, _Pj _Proj) {
        // insertion sort [_First, _Last)

        if (_First == _Last) { // empty range is sorted
            return _Last;
        }

        for (auto _Mid = _First; ++_Mid != _Last;) { // order next element
            iter_value_t<_It> _Val = _RANGES iter_move(_Mid);
            auto _Hole = _Mid;

            for (auto _Prev = _Hole;;) {
                --_Prev;
                if (!_STD invoke(_Pred, _STD invoke(_Proj, _Val), _STD invoke(_Proj, *_Prev))) {
                    break;
                }
                *_Hole = _RANGES iter_move(_Prev); // move hole down
                if (--_Hole == _First) {
                    break;
                }
            }

            *_Hole = _STD move(_Val); // insert element in hole
        }
        return _Last;
    }

    template <random_access_iterator _It, class _Pr, class _Pj>
        requires sortable<_It, _Pr, _Pj>
    constexpr void _Med3_common(_It _First, _It _Mid, _It _Last, _Pr _Pred, _Pj _Proj) {
        // sort median of three elements to middle
        if (_STD invoke(_Pred, _STD invoke(_Proj, *_Mid), _STD invoke(_Proj, *_First))) {
            _RANGES iter_swap(_Mid, _First);
        }

        if (!_STD invoke(_Pred, _STD invoke(_Proj, *_Last), _STD invoke(_Proj, *_Mid))) {
            return;
        }

        // swap middle and last, then test first again
        _RANGES iter_swap(_Last, _Mid);

        if (_STD invoke(_Pred, _STD invoke(_Proj, *_Mid), _STD invoke(_Proj, *_First))) {
            _RANGES iter_swap(_Mid, _First);
        }
    }

    template <random_access_iterator _It, class _Pr, class _Pj>
        requires sortable<_It, _Pr, _Pj>
    constexpr void _Guess_median_common(_It _First, _It _Mid, _It _Last, _Pr _Pred, _Pj _Proj) {
        // sort median element to middle
        using _Diff = iter_difference_t<_It>;
        const _Diff _Count = _Last - _First;
        if (_Count > 40) { // Tukey's ninther
            const _Diff _Step = (_Count + 1) >> 3; // +1 can't overflow because range was made inclusive in caller
            const _Diff _Two_step = _Step << 1; // note: intentionally discards low-order bit
            _Med3_common(_First, _First + _Step, _First + _Two_step, _Pred, _Proj);
            _Med3_common(_Mid - _Step, _Mid, _Mid + _Step, _Pred, _Proj);
            _Med3_common(_Last - _Two_step, _Last - _Step, _Last, _Pred, _Proj);
            _Med3_common(_First + _Step, _Mid, _Last - _Step, _Pred, _Proj);
        }
        else {
            _Med3_common(_First, _Mid, _Last, _Pred, _Proj);
        }
    }

    template <random_access_iterator _It, class _Pr, class _Pj>
        requires sortable<_It, _Pr, _Pj>
    _NODISCARD constexpr subrange<_It> _Partition_by_median_guess_common(_It _First, _It _Last, _Pr _Pred, _Pj _Proj) {
        // Choose a pivot, partition [_First, _Last) into elements less than pivot, elements equal to pivot, and
        // elements greater than pivot; return the equal partition as a subrange.

        _It _Mid = _First + ((_Last - _First) >> 1); // shift for codegen
        _Guess_median_common(_First, _Mid, _RANGES prev(_Last), _Pred, _Proj);
        _It _Pfirst = _Mid;
        _It _Plast = _RANGES next(_Pfirst);

        while (_First < _Pfirst
            && !_STD invoke(_Pred, _STD invoke(_Proj, *_RANGES prev(_Pfirst)), _STD invoke(_Proj, *_Pfirst))
            && !_STD invoke(_Pred, _STD invoke(_Proj, *_Pfirst), _STD invoke(_Proj, *_RANGES prev(_Pfirst)))) {
            --_Pfirst;
        }

        while (_Plast < _Last && !_STD invoke(_Pred, _STD invoke(_Proj, *_Plast), _STD invoke(_Proj, *_Pfirst))
            && !_STD invoke(_Pred, _STD invoke(_Proj, *_Pfirst), _STD invoke(_Proj, *_Plast))) {
            ++_Plast;
        }

        _It _Gfirst = _Plast;
        _It _Glast = _Pfirst;

        for (;;) { // partition
            for (; _Gfirst < _Last; ++_Gfirst) {
                if (_STD invoke(_Pred, _STD invoke(_Proj, *_Pfirst), _STD invoke(_Proj, *_Gfirst))) {
                    continue;
                }
                else if (_STD invoke(_Pred, _STD invoke(_Proj, *_Gfirst), _STD invoke(_Proj, *_Pfirst))) {
                    break;
                }
                else if (_Plast != _Gfirst) {
                    _RANGES iter_swap(_Plast, _Gfirst);
                    ++_Plast;
                }
                else {
                    ++_Plast;
                }
            }

            for (; _First < _Glast; --_Glast) {
                if (_STD invoke(_Pred, _STD invoke(_Proj, *_RANGES prev(_Glast)), _STD invoke(_Proj, *_Pfirst))) {
                    continue;
                }
                else if (_STD invoke(
                    _Pred, _STD invoke(_Proj, *_Pfirst), _STD invoke(_Proj, *_RANGES prev(_Glast)))) {
                    break;
                }
                else if (--_Pfirst != _RANGES prev(_Glast)) {
                    _RANGES iter_swap(_Pfirst, _RANGES prev(_Glast));
                }
            }

            if (_Glast == _First && _Gfirst == _Last) {
                return { _STD move(_Pfirst), _STD move(_Plast) };
            }

            if (_Glast == _First) { // no room at bottom, rotate pivot upward
                if (_Plast != _Gfirst) {
                    _RANGES iter_swap(_Pfirst, _Plast);
                }

                ++_Plast;
                _RANGES iter_swap(_Pfirst, _Gfirst);
                ++_Pfirst;
                ++_Gfirst;
            }
            else if (_Gfirst == _Last) { // no room at top, rotate pivot downward
                if (--_Glast != --_Pfirst) {
                    _RANGES iter_swap(_Glast, _Pfirst);
                }

                _RANGES iter_swap(_Pfirst, --_Plast);
            }
            else {
                _RANGES iter_swap(_Gfirst, --_Glast);
                ++_Gfirst;
            }
        }
    }

    class _Sort_fn : private _Not_quite_object {
    public:
        using _Not_quite_object::_Not_quite_object;

        template <random_access_iterator _It, sentinel_for<_It> _Se, class _Pr = ranges::less, class _Pj = identity>
            requires sortable<_It, _Pr, _Pj>
        constexpr _It operator()(_It _First, _Se _Last, _Pr _Pred = {}, _Pj _Proj = {}) const {
            _Adl_verify_range(_First, _Last);
            auto _UFirst = _Unwrap_iter<_Se>(_STD move(_First));
            auto _ULast = _Get_final_iterator_unwrapped<_It>(_UFirst, _STD move(_Last));
            _Seek_wrapped(_First, _ULast);
            const auto _Count = _ULast - _UFirst;
            _Sort_common(_STD move(_UFirst), _STD move(_ULast), _Count, _Pass_fn(_Pred), _Pass_fn(_Proj));
            return _First;
        }

        template <random_access_range _Rng, class _Pr = ranges::less, class _Pj = identity>
            requires sortable<iterator_t<_Rng>, _Pr, _Pj>
        constexpr borrowed_iterator_t<_Rng> operator()(_Rng&& _Range, _Pr _Pred = {}, _Pj _Proj = {}) const {
            auto _UFirst = _Ubegin(_Range);
            auto _ULast = _Get_final_iterator_unwrapped(_Range);
            const auto _Count = _ULast - _UFirst;
            _Sort_common(_STD move(_UFirst), _ULast, _Count, _Pass_fn(_Pred), _Pass_fn(_Proj));
            return _Rewrap_iterator(_Range, _STD move(_ULast));
        }

    private:
        template <class _It, class _Pr, class _Pj>
        static constexpr void _Sort_common(_It _First, _It _Last, iter_difference_t<_It> _Ideal, _Pr _Pred, _Pj _Proj) {
            // sort [_First, _Last) with respect to _Pred and _Proj
            _STL_INTERNAL_STATIC_ASSERT(random_access_iterator<_It>);
            _STL_INTERNAL_STATIC_ASSERT(sortable<_It, _Pr, _Pj>);

            for (;;) {
                if (_Last - _First <= _ISORT_MAX) { // small
                    _RANGES _Insertion_sort_common(_STD move(_First), _STD move(_Last), _Pred, _Proj);
                    return;
                }

                if (_Ideal <= 0) { // heap sort if too many divisions
                    _Make_heap_common(_First, _Last, _Pred, _Proj);
                    _Sort_heap_common(_STD move(_First), _STD move(_Last), _Pred, _Proj);
                    return;
                }

                // divide and conquer by quicksort
#if defined(__clang__) || defined(__EDG__) // TRANSITION, DevCom-1559808
                auto [_Mid_first, _Mid_last] = _Partition_by_median_guess_common(_First, _Last, _Pred, _Proj);
#else // ^^^ no workaround / workaround vvv
                auto _Mid = _Partition_by_median_guess_common(_First, _Last, _Pred, _Proj);
                auto _Mid_first = _Mid.begin();
                auto _Mid_last = _Mid.end();
#endif // TRANSITION, DevCom-1559808

                _Ideal = (_Ideal >> 1) + (_Ideal >> 2); // allow 1.5 log2(N) divisions

                if (_Mid_first - _First < _Last - _Mid_last) { // loop on second half
                    _Sort_common(_First, _STD move(_Mid_first), _Ideal, _Pred, _Proj);
                    _First = _STD move(_Mid_last);
                }
                else { // loop on first half
                    _Sort_common(_STD move(_Mid_last), _Last, _Ideal, _Pred, _Proj);
                    _Last = _STD move(_Mid_first);
                }
            }
        }
    };

    _EXPORT_STD inline constexpr _Sort_fn sort{ _Not_quite_object::_Construct_tag{} };
} // namespace ranges
#endif // __cpp_lib_concepts

template <class _FwdIt, class _Ty, class _Pr>
_Ty* _Uninitialized_merge_move(_FwdIt _First, const _FwdIt _Mid, const _FwdIt _Last, _Ty* const _Dest, _Pr _Pred) {
    // move merging ranges to uninitialized storage
    // pre: _First != _Mid && _Mid != _Last
    _Uninitialized_backout<_Ty*> _Backout{ _Dest };
    _FwdIt _Next = _Mid;
    for (;;) {
        if (_DEBUG_LT_PRED(_Pred, *_Next, *_First)) {
            _Backout._Emplace_back(_STD move(*_Next));
            ++_Next;

            if (_Next == _Last) {
                _Backout._Last = _STD _Uninitialized_move_unchecked(_First, _Mid, _Backout._Last);
                return _Backout._Release();
            }
        }
        else {
            _Backout._Emplace_back(_STD move(*_First));
            ++_First;

            if (_First == _Mid) {
                _Backout._Last = _STD _Uninitialized_move_unchecked(_Next, _Last, _Backout._Last);
                return _Backout._Release();
            }
        }
    }
}

template <class _InIt, class _OutIt, class _Pr>
_OutIt _Merge_move(_InIt _First, const _InIt _Mid, const _InIt _Last, _OutIt _Dest, _Pr _Pred) {
    // move merging adjacent ranges [_First, _Mid) and [_Mid, _Last) to _Dest
    // pre: _First != _Mid && _Mid != _Last
    _InIt _Next = _Mid;
    for (;;) {
        if (_DEBUG_LT_PRED(_Pred, *_Next, *_First)) {
            *_Dest = _STD move(*_Next);
            ++_Dest;
            ++_Next;

            if (_Next == _Last) {
                return _STD _Move_unchecked(_First, _Mid, _Dest);
            }
        }
        else {
            *_Dest = _STD move(*_First);
            ++_Dest;
            ++_First;

            if (_First == _Mid) {
                return _STD _Move_unchecked(_Next, _Last, _Dest);
            }
        }
    }
}

template <class _BidIt, class _Ty, class _Pr>
void _Uninitialized_chunked_merge_unchecked2(
    _BidIt _First, const _BidIt _Last, _Ty* _Dest, _Iter_diff_t<_BidIt> _Count, _Pr _Pred) {
    // move to uninitialized merging adjacent chunks of distance _Isort_max<_BidIt>
    // pre: _Count == distance(_First, _Last)
    // pre: _Chunk > 0
    _Uninitialized_backout<_Ty*> _Backout{ _Dest };
    while (_Count > _Isort_max<_BidIt>) {
        _Count -= _Isort_max<_BidIt>;
        const _BidIt _Mid1 = _STD next(_First, _Isort_max<_BidIt>);
        const auto _Chunk2 = (_STD min)(_Isort_max<_BidIt>, _Count);
        _Count -= _Chunk2;
        const _BidIt _Mid2 = _STD next(_Mid1, _Chunk2);
        _Backout._Last = _Uninitialized_merge_move(_First, _Mid1, _Mid2, _Backout._Last, _Pred);
        _First = _Mid2;
    }

    _STD _Uninitialized_move_unchecked(_First, _Last, _Backout._Last); // copy partial last chunk
    _Backout._Release();
}

template <class _BidIt, class _OutIt, class _Pr>
void _Chunked_merge_unchecked(_BidIt _First, const _BidIt _Last, _OutIt _Dest, const _Iter_diff_t<_BidIt> _Chunk,
    _Iter_diff_t<_BidIt> _Count, _Pr _Pred) {
    // move merging adjacent chunks of distance _Chunk
    // pre: _Count == distance(_First, _Last)
    // pre: _Chunk > 0
    while (_Chunk < _Count) {
        _Count -= _Chunk;
        const _BidIt _Mid1 = _STD next(_First, _Chunk);
        const auto _Chunk2 = (_STD min)(_Chunk, _Count);
        _Count -= _Chunk2;
        const _BidIt _Mid2 = _STD next(_Mid1, _Chunk2);
        _Dest = _Merge_move(_First, _Mid1, _Mid2, _Dest, _Pred);
        _First = _Mid2;
    }

    _STD _Move_unchecked(_First, _Last, _Dest); // copy partial last chunk
}

template <class _BidIt, class _Pr>
void _Insertion_sort_isort_max_chunks(_BidIt _First, const _BidIt _Last, _Iter_diff_t<_BidIt> _Count, _Pr _Pred) {
    // insertion sort every chunk of distance _Isort_max<_BidIt> in [_First, _Last)
    // pre: _Count == distance(_First, _Last)
    for (; _Isort_max<_BidIt> < _Count; _Count -= _Isort_max<_BidIt>) { // sort chunks
        _First = _Insertion_sort_unchecked(_First, _STD next(_First, _Isort_max<_BidIt>), _Pred);
    }

    _Insertion_sort_unchecked(_First, _Last, _Pred); // sort partial last chunk
}

template <class _BidIt, class _Pr>
void _Buffered_merge_sort_unchecked(const _BidIt _First, const _BidIt _Last, const _Iter_diff_t<_BidIt> _Count,
    _Iter_value_t<_BidIt>* const _Temp_ptr, _Pr _Pred) {
    // sort using temp buffer for merges
    // pre: _Last - _First == _Count
    // pre: _Count <= capacity of buffer at _Temp_ptr; also allows safe narrowing to ptrdiff_t
    _Insertion_sort_isort_max_chunks(_First, _Last, _Count, _Pred);
    // merge adjacent pairs of chunks to and from temp buffer
    if (_Count <= _Isort_max<_BidIt>) {
        return;
    }

    // do the first merge, constructing elements in the temporary buffer
    _Uninitialized_chunked_merge_unchecked2(_First, _Last, _Temp_ptr, _Count, _Pred);
    _Uninitialized_backout<_Iter_value_t<_BidIt>*> _Backout{ _Temp_ptr, _Temp_ptr + _Count };
    auto _Chunk = _Isort_max<_BidIt>;
    for (;;) {
        // unconditionally merge elements back into the source buffer
        _Chunk <<= 1;
        _Chunked_merge_unchecked(_Temp_ptr, _Temp_ptr + _Count, _First, static_cast<ptrdiff_t>(_Chunk),
            static_cast<ptrdiff_t>(_Count), _Pred);
        _Chunk <<= 1;
        if (_Count <= _Chunk) { // if the input would be a single chunk, it's already sorted and we're done
            return;
        }

        // more merges necessary; merge to temporary buffer
        _Chunked_merge_unchecked(_First, _Last, _Temp_ptr, _Chunk, _Count, _Pred);
    }
}

template <class _BidIt, class _Pr>
void _Stable_sort_unchecked(const _BidIt _First, const _BidIt _Last, const _Iter_diff_t<_BidIt> _Count,
    _Iter_value_t<_BidIt>* const _Temp_ptr, const ptrdiff_t _Capacity, _Pr _Pred) {
    // sort preserving order of equivalents
    using _Diff = _Iter_diff_t<_BidIt>;
    if (_Count <= _ISORT_MAX) {
        _Insertion_sort_unchecked(_First, _Last, _Pred); // small
    }
    else { // sort halves and merge
        const auto _Half_count = static_cast<_Diff>(_Count >> 1); // shift for codegen
        const auto _Half_count_ceil = static_cast<_Diff>(_Count - _Half_count);
        const _BidIt _Mid = _STD next(_First, _Half_count_ceil);
        if (_Half_count_ceil <= _Capacity) { // temp buffer big enough, sort each half using buffer
            _Buffered_merge_sort_unchecked(_First, _Mid, _Half_count_ceil, _Temp_ptr, _Pred);
            _Buffered_merge_sort_unchecked(_Mid, _Last, _Half_count, _Temp_ptr, _Pred);
        }
        else { // temp buffer not big enough, divide and conquer
            _Stable_sort_unchecked(_First, _Mid, _Half_count_ceil, _Temp_ptr, _Capacity, _Pred);
            _Stable_sort_unchecked(_Mid, _Last, _Half_count, _Temp_ptr, _Capacity, _Pred);
        }

        _Buffered_inplace_merge_unchecked(
            _First, _Mid, _Last, _Half_count_ceil, _Half_count, _Temp_ptr, _Capacity, _Pred); // merge halves
    }
}

_EXPORT_STD template <class _BidIt, class _Pr>
void stable_sort(const _BidIt _First, const _BidIt _Last, _Pr _Pred) {
    // sort preserving order of equivalents
    _Adl_verify_range(_First, _Last);
    const auto _UFirst = _Get_unwrapped(_First);
    const auto _ULast = _Get_unwrapped(_Last);
    const auto _Count = _STD distance(_UFirst, _ULast);
    if (_Count <= _ISORT_MAX) {
        _Insertion_sort_unchecked(_UFirst, _ULast, _Pass_fn(_Pred));
        return;
    }

    _Optimistic_temporary_buffer<_Iter_value_t<_BidIt>> _Temp_buf{ _Count - _Count / 2 };
    _Stable_sort_unchecked(_UFirst, _ULast, _Count, _Temp_buf._Data, _Temp_buf._Capacity, _Pass_fn(_Pred));
}

#if _HAS_CXX17
_EXPORT_STD template <class _ExPo, class _BidIt, class _Pr, _Enable_if_execution_policy_t<_ExPo> = 0>
void stable_sort(_ExPo&& _Exec, _BidIt _First, _BidIt _Last, _Pr _Pred) noexcept; // terminates
#endif // _HAS_CXX17

_EXPORT_STD template <class _BidIt>
void stable_sort(const _BidIt _First, const _BidIt _Last) { // sort preserving order of equivalents
    _STD stable_sort(_First, _Last, less<>{});
}

#if _HAS_CXX17
_EXPORT_STD template <class _ExPo, class _BidIt, _Enable_if_execution_policy_t<_ExPo> = 0>
void stable_sort(_ExPo&& _Exec, _BidIt _First, _BidIt _Last) noexcept /* terminates */ {
    // sort preserving order of equivalents
    _STD stable_sort(_STD forward<_ExPo>(_Exec), _First, _Last, less{});
}
#endif // _HAS_CXX17

#ifdef __cpp_lib_concepts
namespace ranges {
    class _Stable_sort_fn : private _Not_quite_object {
    public:
        using _Not_quite_object::_Not_quite_object;

        template <random_access_iterator _It, sentinel_for<_It> _Se, class _Pr = ranges::less, class _Pj = identity>
            requires sortable<_It, _Pr, _Pj>
        _It operator()(_It _First, _Se _Last, _Pr _Pred = {}, _Pj _Proj = {}) const {
            _Adl_verify_range(_First, _Last);
            auto _UFirst = _Unwrap_iter<_Se>(_STD move(_First));
            auto _ULast = _Get_final_iterator_unwrapped<_It>(_UFirst, _STD move(_Last));
            _Seek_wrapped(_First, _ULast);

            const auto _Count = _ULast - _UFirst;
            _Stable_sort_common(_STD move(_UFirst), _STD move(_ULast), _Count, _Pass_fn(_Pred), _Pass_fn(_Proj));
            return _First;
        }

        template <random_access_range _Rng, class _Pr = ranges::less, class _Pj = identity>
            requires sortable<iterator_t<_Rng>, _Pr, _Pj>
        borrowed_iterator_t<_Rng> operator()(_Rng&& _Range, _Pr _Pred = {}, _Pj _Proj = {}) const {
            auto _UFirst = _Ubegin(_Range);
            auto _ULast = _Get_final_iterator_unwrapped(_Range);

            const auto _Count = _ULast - _UFirst;
            _Stable_sort_common(_STD move(_UFirst), _ULast, _Count, _Pass_fn(_Pred), _Pass_fn(_Proj));
            return _Rewrap_iterator(_Range, _STD move(_ULast));
        }

    private:
        template <class _It, class _Pr, class _Pj>
        static void _Stable_sort_common(
            _It _First, _It _Last, const iter_difference_t<_It> _Count, _Pr _Pred, _Pj _Proj) {
            // sort [_First, _Last) with respect to _Pred and _Proj
            _STL_INTERNAL_STATIC_ASSERT(random_access_iterator<_It>);
            _STL_INTERNAL_STATIC_ASSERT(sortable<_It, _Pr, _Pj>);
            _STL_INTERNAL_CHECK(_RANGES distance(_First, _Last) == _Count);

            if (_Count <= _Isort_max<_It>) {
                _RANGES _Insertion_sort_common(_STD move(_First), _STD move(_Last), _Pred, _Proj);
                return;
            }

            _Optimistic_temporary_buffer<_Iter_value_t<_It>> _Temp_buf{ _Count - _Count / 2 };
            _Stable_sort_common_buffered(
                _STD move(_First), _STD move(_Last), _Count, _Temp_buf._Data, _Temp_buf._Capacity, _Pred, _Proj);
        }

        template <class _It, class _Pr, class _Pj>
        static void _Stable_sort_common_buffered(_It _First, _It _Last, const iter_difference_t<_It> _Count,
            iter_value_t<_It>* const _Temp_ptr, const ptrdiff_t _Capacity, _Pr _Pred, _Pj _Proj) {
            // sort [_First, _Last) with respect to _Pred and _Proj
            _STL_INTERNAL_STATIC_ASSERT(random_access_iterator<_It>);
            _STL_INTERNAL_STATIC_ASSERT(sortable<_It, _Pr, _Pj>);
            _STL_INTERNAL_CHECK(_RANGES distance(_First, _Last) == _Count);
            // Pre: _Temp_ptr points to empty storage for _Capacity objects

            if (_Count <= _Isort_max<_It>) {
                _RANGES _Insertion_sort_common(_STD move(_First), _STD move(_Last), _Pred, _Proj);
            }
            else { // sort halves and merge
                const iter_difference_t<_It> _Half_count = _Count >> 1; // shift for codegen
                const iter_difference_t<_It> _Half_count_ceil = _Count - _Half_count;
                const _It _Mid = _First + _Half_count_ceil;
                if (_Half_count_ceil <= _Capacity) { // temp buffer big enough, sort each half using buffer
                    _Buffered_merge_sort_common(_First, _Mid, _Half_count_ceil, _Temp_ptr, _Pred, _Proj);
                    _Buffered_merge_sort_common(_Mid, _Last, _Half_count, _Temp_ptr, _Pred, _Proj);
                }
                else { // temp buffer not big enough, divide and conquer
                    _Stable_sort_common_buffered(_First, _Mid, _Half_count_ceil, _Temp_ptr, _Capacity, _Pred, _Proj);
                    _Stable_sort_common_buffered(_Mid, _Last, _Half_count, _Temp_ptr, _Capacity, _Pred, _Proj);
                }
                // merge halves
                _RANGES _Buffered_inplace_merge_common(_STD move(_First), _STD move(_Mid), _STD move(_Last),
                    _Half_count_ceil, _Half_count, _Temp_ptr, _Capacity, _Pred, _Proj);
            }
        }

        template <class _It, class _Pr, class _Pj>
        static void _Buffered_merge_sort_common(const _It _First, const _It _Last, const iter_difference_t<_It> _Count,
            iter_value_t<_It>* const _Temp_ptr, _Pr _Pred, _Pj _Proj) {
            // sort using temp buffer for merges
            // pre: _Count <= capacity of buffer at _Temp_ptr; also allows safe narrowing to ptrdiff_t
            _STL_INTERNAL_STATIC_ASSERT(random_access_iterator<_It>);
            _STL_INTERNAL_STATIC_ASSERT(sortable<_It, _Pr, _Pj>);
            _STL_INTERNAL_CHECK(_Last - _First == _Count);

            _Insertion_sort_isort_max_chunks(_First, _Last, _Count, _Pred, _Proj);
            // merge adjacent pairs of chunks to and from temp buffer
            if (_Count <= _Isort_max<_It>) {
                return;
            }

            // do the first merge, constructing elements in the temporary buffer
            _Uninitialized_chunked_merge_common(_First, _Last, _Temp_ptr, _Count, _Pred, _Proj);
            _Uninitialized_backout<iter_value_t<_It>*> _Backout{ _Temp_ptr, _Temp_ptr + _Count };
            iter_difference_t<_It> _Chunk_size = _Isort_max<_It>;
            for (;;) {
                // unconditionally merge elements back into the source buffer
                _Chunk_size <<= 1;
                _Chunked_merge_common(_Temp_ptr, _Temp_ptr + _Count, _First, _Chunk_size, _Count, _Pred, _Proj);
                _Chunk_size <<= 1;
                if (_Count <= _Chunk_size) { // if the input would be a single chunk, it's already sorted and we're done
                    return;
                }

                // more merges necessary; merge to temporary buffer
                _Chunked_merge_common(_First, _Last, _Temp_ptr, _Chunk_size, _Count, _Pred, _Proj);
            }
        }

        template <class _It, class _Pr, class _Pj>
        static void _Insertion_sort_isort_max_chunks(
            _It _First, _It _Last, iter_difference_t<_It> _Count, _Pr _Pred, _Pj _Proj) {
            // insertion sort every chunk of distance _Isort_max<_It> in [_First, _Last)
            _STL_INTERNAL_STATIC_ASSERT(random_access_iterator<_It>);
            _STL_INTERNAL_STATIC_ASSERT(sortable<_It, _Pr, _Pj>);
            _STL_INTERNAL_CHECK(_RANGES distance(_First, _Last) == _Count);

            for (; _Isort_max<_It> < _Count; _Count -= _Isort_max<_It>) { // sort chunks
                _First = _RANGES _Insertion_sort_common(_First, _First + _Isort_max<_It>, _Pred, _Proj);
            }

            // sort partial last chunk
            _RANGES _Insertion_sort_common(_STD move(_First), _STD move(_Last), _Pred, _Proj);
        }

        template <class _It, class _Pr, class _Pj>
        static void _Uninitialized_chunked_merge_common(_It _First, const _It _Last, iter_value_t<_It>* const _Dest,
            iter_difference_t<_It> _Count, _Pr _Pred, _Pj _Proj) {
            // move to uninitialized merging adjacent chunks of distance _Isort_max<_It>
            _STL_INTERNAL_STATIC_ASSERT(random_access_iterator<_It>);
            _STL_INTERNAL_STATIC_ASSERT(sortable<_It, _Pr, _Pj>);
            _STL_INTERNAL_STATIC_ASSERT(constructible_from<iter_value_t<_It>, iter_rvalue_reference_t<_It>>);
            _STL_INTERNAL_CHECK(_RANGES distance(_First, _Last) == _Count);

            _Uninitialized_backout<iter_value_t<_It>*> _Backout{ _Dest };
            const auto _Backout_end = _Dest + _Count;
            while (_Isort_max<_It> < _Count) {
                _Count -= _Isort_max<_It>;
                const auto _Chunk2 = (_STD min)(_Isort_max<_It>, _Count);
                _Count -= _Chunk2;

                auto _Mid1 = _First + _Isort_max<_It>;
                auto _Last1 = _Mid1 + _Chunk2;
                auto _Last2 = _Backout._Last + _Isort_max<_It> +_Chunk2;
                _Backout._Last = _Uninitialized_merge_move(
                    _STD move(_First), _STD move(_Mid1), _Last1, _Backout._Last, _Last2, _Pred, _Proj);
                _First = _STD move(_Last1);
            }

            // move partial last chunk
            _RANGES _Uninitialized_move_unchecked(_STD move(_First), _STD move(_Last), _Backout._Last, _Backout_end);
            _Backout._Release();
        }

        template <class _It, class _Pr, class _Pj>
        _NODISCARD static iter_value_t<_It>* _Uninitialized_merge_move(_It _First, _It _Mid, _It _Last,
            iter_value_t<_It>* const _Dest, iter_value_t<_It>* const _Dest_last, _Pr _Pred, _Pj _Proj) {
            // move merging ranges to uninitialized storage
            _STL_INTERNAL_STATIC_ASSERT(sortable<_It, _Pr, _Pj>);
            _STL_INTERNAL_STATIC_ASSERT(constructible_from<iter_value_t<_It>, iter_rvalue_reference_t<_It>>);
            _STL_INTERNAL_CHECK(_First != _Mid);
            _STL_INTERNAL_CHECK(_Mid != _Last);
            _STL_INTERNAL_CHECK(_RANGES distance(_First, _Last) <= _RANGES distance(_Dest, _Dest_last));

            _Uninitialized_backout<iter_value_t<_It>*> _Backout{ _Dest };
            _It _Next = _Mid;
            for (;;) {
                if (_STD invoke(_Pred, _STD invoke(_Proj, *_Next), _STD invoke(_Proj, *_First))) {
                    _Backout._Emplace_back(_RANGES iter_move(_Next));
                    ++_Next;

                    if (_Next == _Last) {
                        _Backout._Last = _RANGES _Uninitialized_move_unchecked(
                            _STD move(_First), _STD move(_Mid), _Backout._Last, _Dest_last)
                            .out;
                        return _Backout._Release();
                    }
                }
                else {
                    _Backout._Emplace_back(_RANGES iter_move(_First));
                    ++_First;

                    if (_First == _Mid) {
                        _Backout._Last = _RANGES _Uninitialized_move_unchecked(
                            _STD move(_Next), _STD move(_Last), _Backout._Last, _Dest_last)
                            .out;
                        return _Backout._Release();
                    }
                }
            }
        }

        template <class _InIt, class _OutIt, class _Pr, class _Pj>
        _NODISCARD static _OutIt _Merge_move_common(
            _InIt _First, _InIt _Mid, _InIt _Last, _OutIt _Dest, _Pr _Pred, _Pj _Proj) {
            // move merging adjacent ranges [_First, _Mid) and [_Mid, _Last) to _Dest
            _STL_INTERNAL_STATIC_ASSERT(sortable<_InIt, _Pr, _Pj>);
            _STL_INTERNAL_STATIC_ASSERT(indirectly_movable<_InIt, _OutIt>);
            _STL_INTERNAL_CHECK(_First != _Mid);
            _STL_INTERNAL_CHECK(_Mid != _Last);

            _InIt _Next = _Mid;
            for (;;) {
                if (_STD invoke(_Pred, _STD invoke(_Proj, *_Next), _STD invoke(_Proj, *_First))) {
                    *_Dest = _RANGES iter_move(_Next);
                    ++_Dest;
                    ++_Next;

                    if (_Next == _Last) {
                        return _RANGES _Move_unchecked(_STD move(_First), _STD move(_Mid), _STD move(_Dest)).out;
                    }
                }
                else {
                    *_Dest = _RANGES iter_move(_First);
                    ++_Dest;
                    ++_First;

                    if (_First == _Mid) {
                        return _RANGES _Move_unchecked(_STD move(_Next), _STD move(_Last), _STD move(_Dest)).out;
                    }
                }
            }
        }

        template <class _It1, class _It2, class _Pr, class _Pj>
        static void _Chunked_merge_common(_It1 _First, const _It1 _Last, _It2 _Dest,
            const iter_difference_t<_It1> _Chunk_size, iter_difference_t<_It1> _Count, _Pr _Pred, _Pj _Proj) {
            // move merging adjacent chunks of distance _Chunk_size
            _STL_INTERNAL_STATIC_ASSERT(random_access_iterator<_It1>);
            _STL_INTERNAL_STATIC_ASSERT(sortable<_It1, _Pr, _Pj>);
            _STL_INTERNAL_STATIC_ASSERT(indirectly_movable<_It1, _It2>);
            _STL_INTERNAL_CHECK(_Last - _First == _Count);
            _STL_INTERNAL_CHECK(_Chunk_size > 0);

            while (_Chunk_size < _Count) {
                _Count -= _Chunk_size;
                const auto _Right_chunk_size = (_STD min)(_Chunk_size, _Count);
                _Count -= _Right_chunk_size;

                auto _Mid1 = _First + _Chunk_size;
                auto _Last1 = _Mid1 + _Right_chunk_size;
                _Dest = _Merge_move_common(_STD move(_First), _STD move(_Mid1), _Last1, _Dest, _Pred, _Proj);
                _First = _STD move(_Last1);
            }

            // copy partial last chunk
            _RANGES _Move_unchecked(_STD move(_First), _STD move(_Last), _STD move(_Dest));
        }
    };

    _EXPORT_STD inline constexpr _Stable_sort_fn stable_sort{ _Not_quite_object::_Construct_tag{} };
} // namespace ranges
#endif // __cpp_lib_concepts

_EXPORT_STD template <class _RanIt, class _Pr>
_CONSTEXPR20 void partial_sort(_RanIt _First, _RanIt _Mid, _RanIt _Last, _Pr _Pred) {
    // order [_First, _Last) up to _Mid
    _Adl_verify_range(_First, _Mid);
    _Adl_verify_range(_Mid, _Last);
    auto _UFirst = _Get_unwrapped(_First);
    const auto _UMid = _Get_unwrapped(_Mid);
    const auto _ULast = _Get_unwrapped(_Last);

    if (_UFirst == _UMid) {
        return; // nothing to do, avoid violating _Pop_heap_hole_unchecked preconditions
    }

    _Make_heap_unchecked(_UFirst, _UMid, _Pass_fn(_Pred));
    for (auto _UNext = _UMid; _UNext < _ULast; ++_UNext) {
        if (_DEBUG_LT_PRED(_Pred, *_UNext, *_UFirst)) { // replace top with new largest
            _Iter_value_t<_RanIt> _Val = _STD move(*_UNext);
            _STD _Pop_heap_hole_unchecked(_UFirst, _UMid, _UNext, _STD move(_Val), _Pass_fn(_Pred));
        }
    }

    _Sort_heap_unchecked(_UFirst, _UMid, _Pass_fn(_Pred));
}

_EXPORT_STD template <class _RanIt>
_CONSTEXPR20 void partial_sort(_RanIt _First, _RanIt _Mid, _RanIt _Last) {
    // order [_First, _Last) up to _Mid
    _STD partial_sort(_First, _Mid, _Last, less<>{});
}

#if _HAS_CXX17
_EXPORT_STD template <class _ExPo, class _RanIt, class _Pr, _Enable_if_execution_policy_t<_ExPo> = 0>
void partial_sort(_ExPo&&, _RanIt _First, _RanIt _Mid, _RanIt _Last, _Pr _Pred) noexcept /* terminates */ {
    // order [_First, _Last) up to _Mid
    // parallelism suspected to be infeasible
    return _STD partial_sort(_First, _Mid, _Last, _Pass_fn(_Pred));
}

_EXPORT_STD template <class _ExPo, class _RanIt, _Enable_if_execution_policy_t<_ExPo> = 0>
void partial_sort(_ExPo&&, _RanIt _First, _RanIt _Mid, _RanIt _Last) noexcept /* terminates */ {
    // order [_First, _Last) up to _Mid
    // parallelism suspected to be infeasible
    return _STD partial_sort(_First, _Mid, _Last);
}

#ifdef __cpp_lib_concepts
namespace ranges {
    class _Partial_sort_fn : private _Not_quite_object {
    public:
        using _Not_quite_object::_Not_quite_object;

        template <random_access_iterator _It, sentinel_for<_It> _Se, class _Pr = ranges::less, class _Pj = identity>
            requires sortable<_It, _Pr, _Pj>
        constexpr _It operator()(_It _First, _It _Mid, _Se _Last, _Pr _Pred = {}, _Pj _Proj = {}) const {
            _Adl_verify_range(_First, _Mid);
            _Adl_verify_range(_Mid, _Last);

            if constexpr (is_same_v<_It, _Se>) {
                _Partial_sort_common(_Unwrap_iter<_Se>(_STD move(_First)), _Unwrap_iter<_Se>(_STD move(_Mid)),
                    _Unwrap_sent<_It>(_Last), _Pass_fn(_Pred), _Pass_fn(_Proj));
                return _Last;
            }
            else {
                auto _UMid = _Unwrap_iter<_Se>(_STD move(_Mid));
                auto _ULast = _Get_final_iterator_unwrapped<_It>(_UMid, _STD move(_Last));
                _Seek_wrapped(_Mid, _ULast);
                _Partial_sort_common(_Unwrap_iter<_Se>(_STD move(_First)), _STD move(_UMid), _STD move(_ULast),
                    _Pass_fn(_Pred), _Pass_fn(_Proj));
                return _Mid;
            }
        }

        template <random_access_range _Rng, class _Pr = ranges::less, class _Pj = identity>
            requires sortable<iterator_t<_Rng>, _Pr, _Pj>
        constexpr borrowed_iterator_t<_Rng> operator()(
            _Rng&& _Range, iterator_t<_Rng> _Mid, _Pr _Pred = {}, _Pj _Proj = {}) const {
            _Adl_verify_range(_RANGES begin(_Range), _Mid);
            _Adl_verify_range(_Mid, _RANGES end(_Range));

            if constexpr (common_range<_Rng>) {
                _Partial_sort_common(_Ubegin(_Range), _Unwrap_range_iter<_Rng>(_STD move(_Mid)), _Uend(_Range),
                    _Pass_fn(_Pred), _Pass_fn(_Proj));
                return _RANGES end(_Range);
            }
            else {
                auto _UMid = _Unwrap_range_iter<_Rng>(_STD move(_Mid));
                auto _ULast = _Get_final_iterator_unwrapped(_Range, _UMid);
                _Seek_wrapped(_Mid, _ULast);
                _Partial_sort_common(
                    _Ubegin(_Range), _STD move(_UMid), _STD move(_ULast), _Pass_fn(_Pred), _Pass_fn(_Proj));
                return _Mid;
            }
        }

    private:
        template <class _It, class _Pr, class _Pj>
        static constexpr void _Partial_sort_common(_It _First, _It _Mid, const _It _Last, _Pr _Pred, _Pj _Proj) {
            _STL_INTERNAL_STATIC_ASSERT(random_access_iterator<_It>);
            _STL_INTERNAL_STATIC_ASSERT(sortable<_It, _Pr, _Pj>);

            if (_First == _Mid) {
                return; // nothing to do
            }

            _Make_heap_common(_First, _Mid, _Pred, _Proj);
            for (auto _Next = _Mid; _Next != _Last; ++_Next) {
                if (_STD invoke(_Pred, _STD invoke(_Proj, *_Next), _STD invoke(_Proj, *_First))) {
                    // replace top with new largest
                    iter_value_t<_It> _Val = _RANGES iter_move(_Next);
                    _RANGES _Pop_heap_hole_unchecked(_First, _Mid, _Next, _STD move(_Val), _Pred, _Proj, _Proj);
                }
            }

            _Sort_heap_common(_STD move(_First), _STD move(_Mid), _Pred, _Proj);
        }
    };

    _EXPORT_STD inline constexpr _Partial_sort_fn partial_sort{ _Not_quite_object::_Construct_tag{} };
} // namespace ranges
#endif // __cpp_lib_concepts
#endif // _HAS_CXX17

_EXPORT_STD template <class _InIt, class _RanIt, class _Pr>
_CONSTEXPR20 _RanIt partial_sort_copy(_InIt _First1, _InIt _Last1, _RanIt _First2, _RanIt _Last2, _Pr _Pred) {
    // copy [_First1, _Last1) into [_First2, _Last2)
    _Adl_verify_range(_First1, _Last1);
    _Adl_verify_range(_First2, _Last2);
    auto _UFirst1 = _Get_unwrapped(_First1);
    const auto _ULast1 = _Get_unwrapped(_Last1);
    auto _UFirst2 = _Get_unwrapped(_First2);
    const auto _ULast2 = _Get_unwrapped(_Last2);
    auto _UMid2 = _UFirst2;
    if (_UFirst1 != _ULast1 && _UFirst2 != _ULast2) {
        for (; _UFirst1 != _ULast1 && _UMid2 != _ULast2; ++_UFirst1, (void) ++_UMid2) {
            *_UMid2 = *_UFirst1; // copy min(_ULast1 - _UFirst1, _ULast2 - _UFirst2)
        }

        _Make_heap_unchecked(_UFirst2, _UMid2, _Pass_fn(_Pred));
        for (; _UFirst1 != _ULast1; ++_UFirst1) {
            if (_DEBUG_LT_PRED(_Pred, *_UFirst1, *_UFirst2)) {
                // replace top with new largest:
                using _Diff = _Iter_diff_t<_RanIt>;
                _STD _Pop_heap_hole_by_index(
                    _UFirst2, static_cast<_Diff>(0), static_cast<_Diff>(_UMid2 - _UFirst2), *_UFirst1, _Pass_fn(_Pred));
            }
        }

        _Sort_heap_unchecked(_UFirst2, _UMid2, _Pass_fn(_Pred));
    }

    _Seek_wrapped(_First2, _UMid2);
    return _First2;
}

_EXPORT_STD template <class _InIt, class _RanIt>
_CONSTEXPR20 _RanIt partial_sort_copy(_InIt _First1, _InIt _Last1, _RanIt _First2, _RanIt _Last2) {
    // copy [_First1, _Last1) into [_First2, _Last2)
    return _STD partial_sort_copy(_First1, _Last1, _First2, _Last2, less<>{});
}

#if _HAS_CXX17
_EXPORT_STD template <class _ExPo, class _FwdIt, class _RanIt, class _Pr, _Enable_if_execution_policy_t<_ExPo> = 0>
_RanIt partial_sort_copy(_ExPo&&, _FwdIt _First1, _FwdIt _Last1, _RanIt _First2, _RanIt _Last2, _Pr _Pred) noexcept
/* terminates */ {
    // copy [_First1, _Last1) into [_First2, _Last2)
    // parallelism suspected to be infeasible
    _REQUIRE_PARALLEL_ITERATOR(_FwdIt);
    _REQUIRE_CPP17_MUTABLE_ITERATOR(_RanIt);
    return _STD partial_sort_copy(_First1, _Last1, _First2, _Last2, _Pass_fn(_Pred));
}

_EXPORT_STD template <class _ExPo, class _FwdIt, class _RanIt, _Enable_if_execution_policy_t<_ExPo> = 0>
_RanIt partial_sort_copy(_ExPo&&, _FwdIt _First1, _FwdIt _Last1, _RanIt _First2, _RanIt _Last2) noexcept
/* terminates */ {
    // copy [_First1, _Last1) into [_First2, _Last2)
    // parallelism suspected to be infeasible
    _REQUIRE_PARALLEL_ITERATOR(_FwdIt);
    _REQUIRE_CPP17_MUTABLE_ITERATOR(_RanIt);
    return _STD partial_sort_copy(_First1, _Last1, _First2, _Last2);
}

#ifdef __cpp_lib_concepts
namespace ranges {
    _EXPORT_STD template <class _In, class _Out>
        using partial_sort_copy_result = in_out_result<_In, _Out>;

    class _Partial_sort_copy_fn : private _Not_quite_object {
    public:
        using _Not_quite_object::_Not_quite_object;

        template <input_iterator _It1, sentinel_for<_It1> _Se1, random_access_iterator _It2, sentinel_for<_It2> _Se2,
            class _Pr = ranges::less, class _Pj1 = identity, class _Pj2 = identity>
            requires indirectly_copyable<_It1, _It2>&& sortable<_It2, _Pr, _Pj2>
        && indirect_strict_weak_order<_Pr, projected<_It1, _Pj1>, projected<_It2, _Pj2>>
            constexpr partial_sort_copy_result<_It1, _It2> operator()(_It1 _First1, _Se1 _Last1, _It2 _First2, _Se2 _Last2,
                _Pr _Pred = {}, _Pj1 _Proj1 = {}, _Pj2 _Proj2 = {}) const {
            _Adl_verify_range(_First1, _Last1);
            _Adl_verify_range(_First2, _Last2);

            auto _UResult = _Partial_sort_copy_unchecked(_Unwrap_iter<_Se1>(_STD move(_First1)),
                _Unwrap_sent<_It1>(_STD move(_Last1)), _Unwrap_iter<_Se2>(_STD move(_First2)),
                _Unwrap_sent<_It2>(_STD move(_Last2)), _Pass_fn(_Pred), _Pass_fn(_Proj1), _Pass_fn(_Proj2));

            _Seek_wrapped(_First1, _STD move(_UResult.in));
            _Seek_wrapped(_First2, _STD move(_UResult.out));
            return { _STD move(_First1), _STD move(_First2) };
        }

        template <input_range _Rng1, random_access_range _Rng2, class _Pr = ranges::less, class _Pj1 = identity,
            class _Pj2 = identity>
            requires indirectly_copyable<iterator_t<_Rng1>, iterator_t<_Rng2>>&& sortable<iterator_t<_Rng2>, _Pr, _Pj2>
        && indirect_strict_weak_order<_Pr, projected<iterator_t<_Rng1>, _Pj1>,
            projected<iterator_t<_Rng2>, _Pj2>>
            constexpr partial_sort_copy_result<borrowed_iterator_t<_Rng1>, borrowed_iterator_t<_Rng2>> operator()(
                _Rng1&& _Range1, _Rng2&& _Range2, _Pr _Pred = {}, _Pj1 _Proj1 = {}, _Pj2 _Proj2 = {}) const {
            auto _First = _RANGES begin(_Range1);
            auto _UResult = _Partial_sort_copy_unchecked(_Unwrap_range_iter<_Rng1>(_STD move(_First)), _Uend(_Range1),
                _Ubegin(_Range2), _Uend(_Range2), _Pass_fn(_Pred), _Pass_fn(_Proj1), _Pass_fn(_Proj2));

            _Seek_wrapped(_First, _STD move(_UResult.in));
            return { _STD move(_First), _Rewrap_iterator(_Range2, _STD move(_UResult.out)) };
        }

    private:
        template <class _It1, class _Se1, class _It2, class _Se2, class _Pr, class _Pj1, class _Pj2>
        _NODISCARD static constexpr partial_sort_copy_result<_It1, _It2> _Partial_sort_copy_unchecked(
            _It1 _First1, _Se1 _Last1, _It2 _First2, const _Se2 _Last2, _Pr _Pred, _Pj1 _Proj1, _Pj2 _Proj2) {
            _STL_INTERNAL_STATIC_ASSERT(input_iterator<_It1>);
            _STL_INTERNAL_STATIC_ASSERT(sentinel_for<_Se1, _It1>);
            _STL_INTERNAL_STATIC_ASSERT(random_access_iterator<_It2>);
            _STL_INTERNAL_STATIC_ASSERT(sentinel_for<_Se2, _It2>);
            _STL_INTERNAL_STATIC_ASSERT(indirectly_copyable<_It1, _It2>);
            _STL_INTERNAL_STATIC_ASSERT(sortable<_It2, _Pr, _Pj2>);
            _STL_INTERNAL_STATIC_ASSERT(indirect_strict_weak_order<_Pr, projected<_It1, _Pj1>, projected<_It2, _Pj2>>);

            if (_First1 == _Last1 || _First2 == _Last2) {
                _RANGES advance(_First1, _STD move(_Last1));
                return { _STD move(_First1), _STD move(_First2) };
            }

            // copy N = min(distance(_First1, _Last1), distance(_First2, _Last2)) elements
            auto _Mid2 = _First2;
            do {
                *_Mid2 = *_First1;
                ++_First1;
                ++_Mid2;
            } while (_First1 != _Last1 && _Mid2 != _Last2);

            _Make_heap_common(_First2, _Mid2, _Pred, _Proj2); // Make a heap
            for (; _First1 != _Last1; ++_First1) { // Scan the remaining elements...
                // ... for values less than the largest in the heap ...
                if (_STD invoke(_Pred, _STD invoke(_Proj1, *_First1), _STD invoke(_Proj2, *_First2))) {
                    // ... to replace the largest, after which we restore the heap invariants.
                    using _Diff = iter_difference_t<_It2>;
                    _RANGES _Pop_heap_hole_by_index(_First2, static_cast<_Diff>(0), static_cast<_Diff>(_Mid2 - _First2),
                        *_First1, _Pred, _Proj2, _Proj1);
                }
            }

            // the heap contains the N smallest elements; sort them
            _Sort_heap_common(_STD move(_First2), _Mid2, _Pred, _Proj2);
            return { _STD move(_First1), _STD move(_Mid2) };
        }
    };

    _EXPORT_STD inline constexpr _Partial_sort_copy_fn partial_sort_copy{ _Not_quite_object::_Construct_tag{} };
} // namespace ranges
#endif // __cpp_lib_concepts
#endif // _HAS_CXX17

_EXPORT_STD template <class _RanIt, class _Pr>
_CONSTEXPR20 void nth_element(_RanIt _First, _RanIt _Nth, _RanIt _Last, _Pr _Pred) {
    // order Nth element
    _Adl_verify_range(_First, _Nth);
    _Adl_verify_range(_Nth, _Last);
    auto _UFirst = _Get_unwrapped(_First);
    const auto _UNth = _Get_unwrapped(_Nth);
    auto _ULast = _Get_unwrapped(_Last);
    if (_UNth == _ULast) {
        return; // nothing to do
    }

    while (_ISORT_MAX < _ULast - _UFirst) { // divide and conquer, ordering partition containing Nth
        auto _UMid = _Partition_by_median_guess_unchecked(_UFirst, _ULast, _Pass_fn(_Pred));

        if (_UMid.second <= _UNth) {
            _UFirst = _UMid.second;
        }
        else if (_UMid.first <= _UNth) {
            return; // _Nth is in the subrange of elements equal to the pivot; done
        }
        else {
            _ULast = _UMid.first;
        }
    }

    _Insertion_sort_unchecked(_UFirst, _ULast, _Pass_fn(_Pred)); // sort any remainder
}

_EXPORT_STD template <class _RanIt>
_CONSTEXPR20 void nth_element(_RanIt _First, _RanIt _Nth, _RanIt _Last) { // order Nth element
    _STD nth_element(_First, _Nth, _Last, less<>{});
}

#if _HAS_CXX17
_EXPORT_STD template <class _ExPo, class _RanIt, class _Pr, _Enable_if_execution_policy_t<_ExPo> = 0>
void nth_element(_ExPo&&, _RanIt _First, _RanIt _Nth, _RanIt _Last, _Pr _Pred) noexcept /* terminates */ {
    // order Nth element
    // not parallelized at present, parallelism expected to be feasible in a future release
    _STD nth_element(_First, _Nth, _Last, _Pass_fn(_Pred));
}

_EXPORT_STD template <class _ExPo, class _RanIt, _Enable_if_execution_policy_t<_ExPo> = 0>
void nth_element(_ExPo&&, _RanIt _First, _RanIt _Nth, _RanIt _Last) noexcept /* terminates */ {
    // order Nth element
    // not parallelized at present, parallelism expected to be feasible in a future release
    _STD nth_element(_First, _Nth, _Last);
}

#ifdef __cpp_lib_concepts
namespace ranges {
    class _Nth_element_fn : private _Not_quite_object {
    public:
        using _Not_quite_object::_Not_quite_object;

        template <random_access_iterator _It, sentinel_for<_It> _Se, class _Pr = ranges::less, class _Pj = identity>
            requires sortable<_It, _Pr, _Pj>
        constexpr _It operator()(_It _First, _It _Nth, _Se _Last, _Pr _Pred = {}, _Pj _Proj = {}) const {
            _Adl_verify_range(_First, _Nth);
            _Adl_verify_range(_Nth, _Last);
            auto _UNth = _Unwrap_iter<_Se>(_Nth);
            auto _UFinal = _Get_final_iterator_unwrapped<_It>(_UNth, _STD move(_Last));
            _Seek_wrapped(_Nth, _UFinal);

            _Nth_element_common(_Unwrap_iter<_Se>(_STD move(_First)), _STD move(_UNth), _STD move(_UFinal),
                _Pass_fn(_Pred), _Pass_fn(_Proj));
            return _Nth;
        }

        template <random_access_range _Rng, class _Pr = ranges::less, class _Pj = identity>
            requires sortable<iterator_t<_Rng>, _Pr, _Pj>
        constexpr borrowed_iterator_t<_Rng> operator()(
            _Rng&& _Range, iterator_t<_Rng> _Nth, _Pr _Pred = {}, _Pj _Proj = {}) const {
            _Adl_verify_range(_RANGES begin(_Range), _Nth);
            _Adl_verify_range(_Nth, _RANGES end(_Range));
            auto _UNth = _Unwrap_range_iter<_Rng>(_Nth);
            auto _UFinal = [&] {
                if constexpr (common_range<_Rng>) {
                    return _Uend(_Range);
                }
                else if constexpr (sized_range<_Rng>) {
                    return _RANGES next(_Ubegin(_Range), _RANGES distance(_Range));
                }
                else {
                    return _RANGES next(_UNth, _Uend(_Range));
                }
                }();
                _Seek_wrapped(_Nth, _UFinal);

                _Nth_element_common(
                    _Ubegin(_Range), _STD move(_UNth), _STD move(_UFinal), _Pass_fn(_Pred), _Pass_fn(_Proj));
                return _Nth;
        }

    private:
        template <class _It, class _Pr, class _Pj>
        static constexpr void _Nth_element_common(_It _First, _It _Nth, _It _Last, _Pr _Pred, _Pj _Proj) {
            _STL_INTERNAL_STATIC_ASSERT(random_access_iterator<_It>);
            _STL_INTERNAL_STATIC_ASSERT(sortable<_It, _Pr, _Pj>);

            if (_Nth == _Last) {
                return; // nothing to do
            }

            while (_ISORT_MAX < _Last - _First) { // divide and conquer, ordering partition containing Nth
                subrange<_It> _Mid = _Partition_by_median_guess_common(_First, _Last, _Pred, _Proj);

                if (_Mid.end() <= _Nth) {
                    _First = _Mid.end();
                }
                else if (_Mid.begin() <= _Nth) {
                    return; // _Nth is in the subrange of elements equal to the pivot; done
                }
                else {
                    _Last = _Mid.begin();
                }
            }

            // sort any remainder
            _RANGES _Insertion_sort_common(_STD move(_First), _STD move(_Last), _Pred, _Proj);
        }
    };

    _EXPORT_STD inline constexpr _Nth_element_fn nth_element{ _Not_quite_object::_Construct_tag{} };
} // namespace ranges
#endif // __cpp_lib_concepts
#endif // _HAS_CXX17

_EXPORT_STD template <class _InIt1, class _InIt2, class _Pr>
_NODISCARD _CONSTEXPR20 bool includes(_InIt1 _First1, _InIt1 _Last1, _InIt2 _First2, _InIt2 _Last2, _Pr _Pred) {
    // test if every element in sorted [_First2, _Last2) is in sorted [_First1, _Last1)
    _Adl_verify_range(_First1, _Last1);
    _Adl_verify_range(_First2, _Last2);
    auto _UFirst1 = _Get_unwrapped(_First1);
    const auto _ULast1 = _Get_unwrapped(_Last1);
    auto _UFirst2 = _Get_unwrapped(_First2);
    const auto _ULast2 = _Get_unwrapped(_Last2);
    _DEBUG_ORDER_SET_UNWRAPPED(_InIt2, _UFirst1, _ULast1, _Pred);
    _DEBUG_ORDER_SET_UNWRAPPED(_InIt1, _UFirst2, _ULast2, _Pred);
    for (; _UFirst1 != _ULast1 && _UFirst2 != _ULast2; ++_UFirst1) {
        if (_DEBUG_LT_PRED(_Pred, *_UFirst2, *_UFirst1)) {
            return false;
        }

        if (!_Pred(*_UFirst1, *_UFirst2)) {
            ++_UFirst2;
        }
    }

    return _UFirst2 == _ULast2;
}

_EXPORT_STD template <class _InIt1, class _InIt2>
_NODISCARD _CONSTEXPR20 bool includes(_InIt1 _First1, _InIt1 _Last1, _InIt2 _First2, _InIt2 _Last2) {
    // test if every element in sorted [_First2, _Last2) is in sorted [_First1, _Last1)
    return _STD includes(_First1, _Last1, _First2, _Last2, less<>{});
}

#if _HAS_CXX17
_EXPORT_STD template <class _ExPo, class _FwdIt1, class _FwdIt2, class _Pr, _Enable_if_execution_policy_t<_ExPo> = 0>
_NODISCARD bool includes(_ExPo&&, _FwdIt1 _First1, _FwdIt1 _Last1, _FwdIt2 _First2, _FwdIt2 _Last2, _Pr _Pred) noexcept
/* terminates */ {
    // test if every element in sorted [_First2, _Last2) is in sorted [_First1, _Last1)
    // not parallelized at present, parallelism expected to be feasible in a future release
    _REQUIRE_PARALLEL_ITERATOR(_FwdIt1);
    _REQUIRE_PARALLEL_ITERATOR(_FwdIt2);
    return _STD includes(_First1, _Last1, _First2, _Last2, _Pass_fn(_Pred));
}

_EXPORT_STD template <class _ExPo, class _FwdIt1, class _FwdIt2, _Enable_if_execution_policy_t<_ExPo> = 0>
_NODISCARD bool includes(_ExPo&&, _FwdIt1 _First1, _FwdIt1 _Last1, _FwdIt2 _First2, _FwdIt2 _Last2) noexcept
/* terminates */ {
    // test if every element in sorted [_First2, _Last2) is in sorted [_First1, _Last1)
    // not parallelized at present, parallelism expected to be feasible in a future release
    _REQUIRE_PARALLEL_ITERATOR(_FwdIt1);
    _REQUIRE_PARALLEL_ITERATOR(_FwdIt2);
    return _STD includes(_First1, _Last1, _First2, _Last2);
}

#ifdef __cpp_lib_concepts
namespace ranges {
    class _Includes_fn : private _Not_quite_object {
    public:
        using _Not_quite_object::_Not_quite_object;

        template <input_iterator _It1, sentinel_for<_It1> _Se1, input_iterator _It2, sentinel_for<_It2> _Se2,
            class _Pj1 = identity, class _Pj2 = identity,
            indirect_strict_weak_order<projected<_It1, _Pj1>, projected<_It2, _Pj2>> _Pr = ranges::less>
        _NODISCARD constexpr bool operator()(_It1 _First1, _Se1 _Last1, _It2 _First2, _Se2 _Last2, _Pr _Pred = {},
            _Pj1 _Proj1 = {}, _Pj2 _Proj2 = {}) const {
            _Adl_verify_range(_First1, _Last1);
            _Adl_verify_range(_First2, _Last2);
            return _Includes_unchecked(_Unwrap_iter<_Se1>(_STD move(_First1)), _Unwrap_sent<_It1>(_STD move(_Last1)),
                _Unwrap_iter<_Se2>(_STD move(_First2)), _Unwrap_sent<_It2>(_STD move(_Last2)), _Pass_fn(_Pred),
                _Pass_fn(_Proj1), _Pass_fn(_Proj2));
        }

        template <input_range _Rng1, input_range _Rng2, class _Pj1 = identity, class _Pj2 = identity,
            indirect_strict_weak_order<projected<iterator_t<_Rng1>, _Pj1>, projected<iterator_t<_Rng2>, _Pj2>> _Pr =
            ranges::less>
        _NODISCARD constexpr bool operator()(
            _Rng1&& _Range1, _Rng2&& _Range2, _Pr _Pred = {}, _Pj1 _Proj1 = {}, _Pj2 _Proj2 = {}) const {
            return _Includes_unchecked(_Ubegin(_Range1), _Uend(_Range1), _Ubegin(_Range2), _Uend(_Range2),
                _Pass_fn(_Pred), _Pass_fn(_Proj1), _Pass_fn(_Proj2));
        }

    private:
        template <class _It1, class _Se1, class _It2, class _Se2, class _Pr, class _Pj1, class _Pj2>
        _NODISCARD static constexpr bool _Includes_unchecked(
            _It1 _First1, const _Se1 _Last1, _It2 _First2, const _Se2 _Last2, _Pr _Pred, _Pj1 _Proj1, _Pj2 _Proj2) {
            _STL_INTERNAL_STATIC_ASSERT(input_iterator<_It1>);
            _STL_INTERNAL_STATIC_ASSERT(sentinel_for<_Se1, _It1>);
            _STL_INTERNAL_STATIC_ASSERT(input_iterator<_It2>);
            _STL_INTERNAL_STATIC_ASSERT(sentinel_for<_Se2, _It2>);
            _STL_INTERNAL_STATIC_ASSERT(indirect_strict_weak_order<_Pr, projected<_It1, _Pj1>, projected<_It2, _Pj2>>);

            if (_First2 == _Last2) {
                return true;
            }
            else if (_First1 == _Last1) {
                return false;
            }

            for (;;) {
                if (_STD invoke(_Pred, _STD invoke(_Proj1, *_First1), _STD invoke(_Proj2, *_First2))) {
                    ++_First1;
                    if (_First1 == _Last1) {
                        return false;
                    }

                    continue;
                }

                if (_STD invoke(_Pred, _STD invoke(_Proj2, *_First2), _STD invoke(_Proj1, *_First1))) {
                    return false;
                }

                ++_First1;
                ++_First2;
                if (_First2 == _Last2) {
                    return true;
                }
                else if (_First1 == _Last1) {
                    return false;
                }
            }
        }
    };

    _EXPORT_STD inline constexpr _Includes_fn includes{ _Not_quite_object::_Construct_tag{} };
} // namespace ranges
#endif // __cpp_lib_concepts
#endif // _HAS_CXX17

_EXPORT_STD template <class _InIt1, class _InIt2, class _OutIt, class _Pr>
_CONSTEXPR20 _OutIt set_union(_InIt1 _First1, _InIt1 _Last1, _InIt2 _First2, _InIt2 _Last2, _OutIt _Dest, _Pr _Pred) {
    // OR sets [_First1, _Last1) and [_First2, _Last2)
    _Adl_verify_range(_First1, _Last1);
    _Adl_verify_range(_First2, _Last2);
    auto _UFirst1 = _Get_unwrapped(_First1);
    const auto _ULast1 = _Get_unwrapped(_Last1);
    auto _UFirst2 = _Get_unwrapped(_First2);
    const auto _ULast2 = _Get_unwrapped(_Last2);
    _DEBUG_ORDER_SET_UNWRAPPED(_InIt2, _UFirst1, _ULast1, _Pred);
    _DEBUG_ORDER_SET_UNWRAPPED(_InIt1, _UFirst2, _ULast2, _Pred);
    auto _UDest = _Get_unwrapped_unverified(_Dest);
    for (; _UFirst1 != _ULast1 && _UFirst2 != _ULast2; ++_UDest) {
        if (_DEBUG_LT_PRED(_Pred, *_UFirst1, *_UFirst2)) { // copy first
            *_UDest = *_UFirst1;
            ++_UFirst1;
        }
        else if (_Pred(*_UFirst2, *_UFirst1)) { // copy second
            *_UDest = *_UFirst2;
            ++_UFirst2;
        }
        else { // advance both
            *_UDest = *_UFirst1;
            ++_UFirst1;
            ++_UFirst2;
        }
    }

    _UDest = _STD _Copy_unchecked(_UFirst1, _ULast1, _UDest);
    _Seek_wrapped(_Dest, _STD _Copy_unchecked(_UFirst2, _ULast2, _UDest));
    return _Dest;
}

_EXPORT_STD template <class _InIt1, class _InIt2, class _OutIt>
_CONSTEXPR20 _OutIt set_union(_InIt1 _First1, _InIt1 _Last1, _InIt2 _First2, _InIt2 _Last2, _OutIt _Dest) {
    // OR sets [_First1, _Last1) and [_First2, _Last2)
    return _STD set_union(_First1, _Last1, _First2, _Last2, _Dest, less<>{});
}

#if _HAS_CXX17
_EXPORT_STD template <class _ExPo, class _FwdIt1, class _FwdIt2, class _FwdIt3, class _Pr,
    _Enable_if_execution_policy_t<_ExPo> = 0>
    _FwdIt3 set_union(_ExPo&&, _FwdIt1 _First1, _FwdIt1 _Last1, _FwdIt2 _First2, _FwdIt2 _Last2, _FwdIt3 _Dest,
        _Pr _Pred) noexcept /* terminates */ {
    // OR sets [_First1, _Last1) and [_First2, _Last2)
    // not parallelized at present, parallelism expected to be feasible in a future release
    _REQUIRE_PARALLEL_ITERATOR(_FwdIt1);
    _REQUIRE_PARALLEL_ITERATOR(_FwdIt2);
    _REQUIRE_CPP17_MUTABLE_ITERATOR(_FwdIt3);
    return _STD set_union(_First1, _Last1, _First2, _Last2, _Dest, _Pass_fn(_Pred));
}

_EXPORT_STD template <class _ExPo, class _FwdIt1, class _FwdIt2, class _FwdIt3,
    _Enable_if_execution_policy_t<_ExPo> = 0>
    _FwdIt3 set_union(_ExPo&&, _FwdIt1 _First1, _FwdIt1 _Last1, _FwdIt2 _First2, _FwdIt2 _Last2, _FwdIt3 _Dest) noexcept
    /* terminates */ {
    // OR sets [_First1, _Last1) and [_First2, _Last2)
    // not parallelized at present, parallelism expected to be feasible in a future release
    _REQUIRE_PARALLEL_ITERATOR(_FwdIt1);
    _REQUIRE_PARALLEL_ITERATOR(_FwdIt2);
    _REQUIRE_CPP17_MUTABLE_ITERATOR(_FwdIt3);
    return _STD set_union(_First1, _Last1, _First2, _Last2, _Dest);
}

#ifdef __cpp_lib_concepts
namespace ranges {
    _EXPORT_STD template <class _In1, class _In2, class _Out>
        using set_union_result = in_in_out_result<_In1, _In2, _Out>;

    class _Set_union_fn : private _Not_quite_object {
    public:
        using _Not_quite_object::_Not_quite_object;

        template <input_iterator _It1, sentinel_for<_It1> _Se1, input_iterator _It2, sentinel_for<_It2> _Se2,
            weakly_incrementable _Out, class _Pr = ranges::less, class _Pj1 = identity, class _Pj2 = identity>
            requires mergeable<_It1, _It2, _Out, _Pr, _Pj1, _Pj2>
        constexpr set_union_result<_It1, _It2, _Out> operator()(_It1 _First1, _Se1 _Last1, _It2 _First2, _Se2 _Last2,
            _Out _Result, _Pr _Pred = {}, _Pj1 _Proj1 = {}, _Pj2 _Proj2 = {}) const {
            _Adl_verify_range(_First1, _Last1);
            _Adl_verify_range(_First2, _Last2);
            auto _UResult =
                _Set_union_unchecked(_Unwrap_iter<_Se1>(_STD move(_First1)), _Unwrap_sent<_It1>(_STD move(_Last1)),
                    _Unwrap_iter<_Se2>(_STD move(_First2)), _Unwrap_sent<_It2>(_STD move(_Last2)),
                    _Get_unwrapped_unverified(_STD move(_Result)), _Pass_fn(_Pred), _Pass_fn(_Proj1), _Pass_fn(_Proj2));
            _Seek_wrapped(_First1, _STD move(_UResult.in1));
            _Seek_wrapped(_First2, _STD move(_UResult.in2));
            _Seek_wrapped(_Result, _STD move(_UResult.out));
            return { _STD move(_First1), _STD move(_First2), _STD move(_Result) };
        }

        template <input_range _Rng1, input_range _Rng2, weakly_incrementable _Out, class _Pr = ranges::less,
            class _Pj1 = identity, class _Pj2 = identity>
            requires mergeable<iterator_t<_Rng1>, iterator_t<_Rng2>, _Out, _Pr, _Pj1, _Pj2>
        constexpr set_union_result<borrowed_iterator_t<_Rng1>, borrowed_iterator_t<_Rng2>, _Out> operator()(
            _Rng1&& _Range1, _Rng2&& _Range2, _Out _Result, _Pr _Pred = {}, _Pj1 _Proj1 = {}, _Pj2 _Proj2 = {}) const {
            auto _First1 = _RANGES begin(_Range1);
            auto _First2 = _RANGES begin(_Range2);
            auto _UResult = _Set_union_unchecked(_Unwrap_range_iter<_Rng1>(_STD move(_First1)), _Uend(_Range1),
                _Unwrap_range_iter<_Rng2>(_STD move(_First2)), _Uend(_Range2),
                _Get_unwrapped_unverified(_STD move(_Result)), _Pass_fn(_Pred), _Pass_fn(_Proj1), _Pass_fn(_Proj2));
            _Seek_wrapped(_First1, _STD move(_UResult.in1));
            _Seek_wrapped(_First2, _STD move(_UResult.in2));
            _Seek_wrapped(_Result, _STD move(_UResult.out));
            return { _STD move(_First1), _STD move(_First2), _STD move(_Result) };
        }

    private:
        template <class _It1, class _Se1, class _It2, class _Se2, class _Out, class _Pr, class _Pj1, class _Pj2>
        _NODISCARD static constexpr set_union_result<_It1, _It2, _Out> _Set_union_unchecked(_It1 _First1,
            const _Se1 _Last1, _It2 _First2, const _Se2 _Last2, _Out _Result, _Pr _Pred, _Pj1 _Proj1, _Pj2 _Proj2) {
            _STL_INTERNAL_STATIC_ASSERT(input_iterator<_It1>);
            _STL_INTERNAL_STATIC_ASSERT(sentinel_for<_Se1, _It1>);
            _STL_INTERNAL_STATIC_ASSERT(input_iterator<_It2>);
            _STL_INTERNAL_STATIC_ASSERT(sentinel_for<_Se2, _It2>);
            _STL_INTERNAL_STATIC_ASSERT(weakly_incrementable<_Out>);
            _STL_INTERNAL_STATIC_ASSERT(mergeable<_It1, _It2, _Out, _Pr, _Pj1, _Pj2>);

            for (; _First1 != _Last1 && _First2 != _Last2; ++_Result) {
                if (_STD invoke(_Pred, _STD invoke(_Proj2, *_First2), _STD invoke(_Proj1, *_First1))) {
                    *_Result = *_First2;
                    ++_First2;
                }
                else {
                    *_Result = *_First1;
                    if (!_STD invoke(_Pred, _STD invoke(_Proj1, *_First1), _STD invoke(_Proj2, *_First2))) {
                        ++_First2;
                    }
                    ++_First1;
                }
            }

            auto _UResult1 = _RANGES _Copy_unchecked(_STD move(_First1), _STD move(_Last1), _STD move(_Result));
            auto _UResult2 = _RANGES _Copy_unchecked(_STD move(_First2), _STD move(_Last2), _STD move(_UResult1.out));
            return { _STD move(_UResult1.in), _STD move(_UResult2.in), _STD move(_UResult2.out) };
        }
    };

    _EXPORT_STD inline constexpr _Set_union_fn set_union{ _Not_quite_object::_Construct_tag{} };
} // namespace ranges
#endif // __cpp_lib_concepts
#endif // _HAS_CXX17

_EXPORT_STD template <class _InIt1, class _InIt2, class _OutIt, class _Pr>
_CONSTEXPR20 _OutIt set_intersection(
    _InIt1 _First1, _InIt1 _Last1, _InIt2 _First2, _InIt2 _Last2, _OutIt _Dest, _Pr _Pred) {
    // AND sets [_First1, _Last1) and [_First2, _Last2)
    _Adl_verify_range(_First1, _Last1);
    _Adl_verify_range(_First2, _Last2);
    auto _UFirst1 = _Get_unwrapped(_First1);
    const auto _ULast1 = _Get_unwrapped(_Last1);
    auto _UFirst2 = _Get_unwrapped(_First2);
    const auto _ULast2 = _Get_unwrapped(_Last2);
    _DEBUG_ORDER_SET_UNWRAPPED(_InIt2, _UFirst1, _ULast1, _Pred);
    _DEBUG_ORDER_SET_UNWRAPPED(_InIt1, _UFirst2, _ULast2, _Pred);
    auto _UDest = _Get_unwrapped_unverified(_Dest);
    while (_UFirst1 != _ULast1 && _UFirst2 != _ULast2) {
        if (_DEBUG_LT_PRED(_Pred, *_UFirst1, *_UFirst2)) {
            ++_UFirst1;
        }
        else if (_Pred(*_UFirst2, *_UFirst1)) {
            ++_UFirst2;
        }
        else {
            *_UDest = *_UFirst1;
            ++_UDest;
            ++_UFirst1;
            ++_UFirst2;
        }
    }

    _Seek_wrapped(_Dest, _UDest);
    return _Dest;
}

_EXPORT_STD template <class _InIt1, class _InIt2, class _OutIt>
_CONSTEXPR20 _OutIt set_intersection(_InIt1 _First1, _InIt1 _Last1, _InIt2 _First2, _InIt2 _Last2, _OutIt _Dest) {
    // AND sets [_First1, _Last1) and [_First2, _Last2)
    return _STD set_intersection(_First1, _Last1, _First2, _Last2, _Dest, less<>{});
}

#if _HAS_CXX17
_EXPORT_STD template <class _ExPo, class _FwdIt1, class _FwdIt2, class _FwdIt3, class _Pr,
    _Enable_if_execution_policy_t<_ExPo> = 0>
    _FwdIt3 set_intersection(_ExPo&&, _FwdIt1 _First1, _FwdIt1 _Last1, _FwdIt2 _First2, _FwdIt2 _Last2, _FwdIt3 _Dest,
        _Pr _Pred) noexcept; // terminates

_EXPORT_STD template <class _ExPo, class _FwdIt1, class _FwdIt2, class _FwdIt3,
    _Enable_if_execution_policy_t<_ExPo> = 0>
    _FwdIt3 set_intersection(_ExPo&& _Exec, _FwdIt1 _First1, _FwdIt1 _Last1, _FwdIt2 _First2, _FwdIt2 _Last2,
        _FwdIt3 _Dest) noexcept /* terminates */ {
    // AND sets [_First1, _Last1) and [_First2, _Last2)
    return _STD set_intersection(_STD forward<_ExPo>(_Exec), _First1, _Last1, _First2, _Last2, _Dest, less{});
}

#ifdef __cpp_lib_concepts
namespace ranges {
    _EXPORT_STD template <class _In1, class _In2, class _Out>
        using set_intersection_result = in_in_out_result<_In1, _In2, _Out>;

    class _Set_intersection_fn : private _Not_quite_object {
    public:
        using _Not_quite_object::_Not_quite_object;

        template <input_iterator _It1, sentinel_for<_It1> _Se1, input_iterator _It2, sentinel_for<_It2> _Se2,
            weakly_incrementable _Out, class _Pr = ranges::less, class _Pj1 = identity, class _Pj2 = identity>
            requires mergeable<_It1, _It2, _Out, _Pr, _Pj1, _Pj2>
        constexpr set_intersection_result<_It1, _It2, _Out> operator()(_It1 _First1, _Se1 _Last1, _It2 _First2,
            _Se2 _Last2, _Out _Result, _Pr _Pred = {}, _Pj1 _Proj1 = {}, _Pj2 _Proj2 = {}) const {
            _Adl_verify_range(_First1, _Last1);
            _Adl_verify_range(_First2, _Last2);
            auto _UResult = _Set_intersection_unchecked(_Unwrap_iter<_Se1>(_STD move(_First1)),
                _Unwrap_sent<_It1>(_STD move(_Last1)), _Unwrap_iter<_Se2>(_STD move(_First2)),
                _Unwrap_sent<_It2>(_STD move(_Last2)), _Get_unwrapped_unverified(_STD move(_Result)), _Pass_fn(_Pred),
                _Pass_fn(_Proj1), _Pass_fn(_Proj2));
            _Seek_wrapped(_First1, _STD move(_UResult.in1));
            _Seek_wrapped(_First2, _STD move(_UResult.in2));
            _Seek_wrapped(_Result, _STD move(_UResult.out));
            return { _STD move(_First1), _STD move(_First2), _STD move(_Result) };
        }

        template <input_range _Rng1, input_range _Rng2, weakly_incrementable _Out, class _Pr = ranges::less,
            class _Pj1 = identity, class _Pj2 = identity>
            requires mergeable<iterator_t<_Rng1>, iterator_t<_Rng2>, _Out, _Pr, _Pj1, _Pj2>
        constexpr set_intersection_result<borrowed_iterator_t<_Rng1>, borrowed_iterator_t<_Rng2>, _Out> operator()(
            _Rng1&& _Range1, _Rng2&& _Range2, _Out _Result, _Pr _Pred = {}, _Pj1 _Proj1 = {}, _Pj2 _Proj2 = {}) const {
            auto _First1 = _RANGES begin(_Range1);
            auto _First2 = _RANGES begin(_Range2);
            auto _UResult = _Set_intersection_unchecked(_Unwrap_range_iter<_Rng1>(_STD move(_First1)), _Uend(_Range1),
                _Unwrap_range_iter<_Rng2>(_STD move(_First2)), _Uend(_Range2),
                _Get_unwrapped_unverified(_STD move(_Result)), _Pass_fn(_Pred), _Pass_fn(_Proj1), _Pass_fn(_Proj2));
            _Seek_wrapped(_First1, _STD move(_UResult.in1));
            _Seek_wrapped(_First2, _STD move(_UResult.in2));
            _Seek_wrapped(_Result, _STD move(_UResult.out));
            return { _STD move(_First1), _STD move(_First2), _STD move(_Result) };
        }

    private:
        template <class _It1, class _Se1, class _It2, class _Se2, class _Out, class _Pr, class _Pj1, class _Pj2>
        _NODISCARD static constexpr set_intersection_result<_It1, _It2, _Out> _Set_intersection_unchecked(_It1 _First1,
            const _Se1 _Last1, _It2 _First2, const _Se2 _Last2, _Out _Result, _Pr _Pred, _Pj1 _Proj1, _Pj2 _Proj2) {
            _STL_INTERNAL_STATIC_ASSERT(input_iterator<_It1>);
            _STL_INTERNAL_STATIC_ASSERT(sentinel_for<_Se1, _It1>);
            _STL_INTERNAL_STATIC_ASSERT(input_iterator<_It2>);
            _STL_INTERNAL_STATIC_ASSERT(sentinel_for<_Se2, _It2>);
            _STL_INTERNAL_STATIC_ASSERT(weakly_incrementable<_Out>);
            _STL_INTERNAL_STATIC_ASSERT(mergeable<_It1, _It2, _Out, _Pr, _Pj1, _Pj2>);

            for (;;) {
                if (_First1 == _Last1) {
                    _RANGES advance(_First2, _Last2);
                    break;
                }
                else if (_First2 == _Last2) {
                    _RANGES advance(_First1, _Last1);
                    break;
                }

                if (_STD invoke(_Pred, _STD invoke(_Proj1, *_First1), _STD invoke(_Proj2, *_First2))) {
                    ++_First1;
                }
                else if (_STD invoke(_Pred, _STD invoke(_Proj2, *_First2), _STD invoke(_Proj1, *_First1))) {
                    ++_First2;
                }
                else {
                    *_Result = *_First1;
                    ++_Result;
                    ++_First1;
                    ++_First2;
                }
            }

            return { _STD move(_First1), _STD move(_First2), _STD move(_Result) };
        }
    };

    _EXPORT_STD inline constexpr _Set_intersection_fn set_intersection{ _Not_quite_object::_Construct_tag{} };
} // namespace ranges
#endif // __cpp_lib_concepts
#endif // _HAS_CXX17

_EXPORT_STD template <class _InIt1, class _InIt2, class _OutIt, class _Pr>
_CONSTEXPR20 _OutIt set_difference(
    _InIt1 _First1, _InIt1 _Last1, _InIt2 _First2, _InIt2 _Last2, _OutIt _Dest, _Pr _Pred) {
    // take set [_First2, _Last2) from [_First1, _Last1)
    _Adl_verify_range(_First1, _Last1);
    _Adl_verify_range(_First2, _Last2);
    auto _UFirst1 = _Get_unwrapped(_First1);
    const auto _ULast1 = _Get_unwrapped(_Last1);
    auto _UFirst2 = _Get_unwrapped(_First2);
    const auto _ULast2 = _Get_unwrapped(_Last2);
    _DEBUG_ORDER_SET_UNWRAPPED(_InIt2, _UFirst1, _ULast1, _Pred);
    _DEBUG_ORDER_SET_UNWRAPPED(_InIt1, _UFirst2, _ULast2, _Pred);
    auto _UDest = _Get_unwrapped_unverified(_Dest);
    while (_UFirst1 != _ULast1 && _UFirst2 != _ULast2) {
        if (_DEBUG_LT_PRED(_Pred, *_UFirst1, *_UFirst2)) { // copy first
            *_UDest = *_UFirst1;
            ++_UDest;
            ++_UFirst1;
        }
        else {
            if (!_Pred(*_UFirst2, *_UFirst1)) {
                ++_UFirst1;
            }

            ++_UFirst2;
        }
    }

    _Seek_wrapped(_Dest, _STD _Copy_unchecked(_UFirst1, _ULast1, _UDest));
    return _Dest;
}

_EXPORT_STD template <class _InIt1, class _InIt2, class _OutIt>
_CONSTEXPR20 _OutIt set_difference(_InIt1 _First1, _InIt1 _Last1, _InIt2 _First2, _InIt2 _Last2, _OutIt _Dest) {
    // take set [_First2, _Last2) from [_First1, _Last1)
    return _STD set_difference(_First1, _Last1, _First2, _Last2, _Dest, less<>{});
}

#if _HAS_CXX17
_EXPORT_STD template <class _ExPo, class _FwdIt1, class _FwdIt2, class _FwdIt3, class _Pr,
    _Enable_if_execution_policy_t<_ExPo> = 0>
    _FwdIt3 set_difference(_ExPo&&, _FwdIt1 _First1, _FwdIt1 _Last1, _FwdIt2 _First2, _FwdIt2 _Last2, _FwdIt3 _Dest,
        _Pr _Pred) noexcept; // terminates

_EXPORT_STD template <class _ExPo, class _FwdIt1, class _FwdIt2, class _FwdIt3,
    _Enable_if_execution_policy_t<_ExPo> = 0>
    _FwdIt3 set_difference(_ExPo&& _Exec, _FwdIt1 _First1, _FwdIt1 _Last1, _FwdIt2 _First2, _FwdIt2 _Last2,
        _FwdIt3 _Dest) noexcept /* terminates */ {
    // take set [_First2, _Last2) from [_First1, _Last1)
    return _STD set_difference(_STD forward<_ExPo>(_Exec), _First1, _Last1, _First2, _Last2, _Dest, less{});
}

#ifdef __cpp_lib_concepts
namespace ranges {
    _EXPORT_STD template <class _In, class _Out>
        using set_difference_result = in_out_result<_In, _Out>;

    class _Set_difference_fn : private _Not_quite_object {
    public:
        using _Not_quite_object::_Not_quite_object;

        template <input_iterator _It1, sentinel_for<_It1> _Se1, input_iterator _It2, sentinel_for<_It2> _Se2,
            weakly_incrementable _Out, class _Pr = ranges::less, class _Pj1 = identity, class _Pj2 = identity>
            requires mergeable<_It1, _It2, _Out, _Pr, _Pj1, _Pj2>
        constexpr set_difference_result<_It1, _Out> operator()(_It1 _First1, _Se1 _Last1, _It2 _First2, _Se2 _Last2,
            _Out _Result, _Pr _Pred = {}, _Pj1 _Proj1 = {}, _Pj2 _Proj2 = {}) const {
            _Adl_verify_range(_First1, _Last1);
            _Adl_verify_range(_First2, _Last2);
            auto _UResult =
                _Set_difference_unchecked(_Unwrap_iter<_Se1>(_STD move(_First1)), _Unwrap_sent<_It1>(_STD move(_Last1)),
                    _Unwrap_iter<_Se2>(_STD move(_First2)), _Unwrap_sent<_It2>(_STD move(_Last2)),
                    _Get_unwrapped_unverified(_STD move(_Result)), _Pass_fn(_Pred), _Pass_fn(_Proj1), _Pass_fn(_Proj2));
            _Seek_wrapped(_First1, _STD move(_UResult.in));
            _Seek_wrapped(_Result, _STD move(_UResult.out));
            return { _STD move(_First1), _STD move(_Result) };
        }

        template <input_range _Rng1, input_range _Rng2, weakly_incrementable _Out, class _Pr = ranges::less,
            class _Pj1 = identity, class _Pj2 = identity>
            requires mergeable<iterator_t<_Rng1>, iterator_t<_Rng2>, _Out, _Pr, _Pj1, _Pj2>
        constexpr set_difference_result<borrowed_iterator_t<_Rng1>, _Out> operator()(
            _Rng1&& _Range1, _Rng2&& _Range2, _Out _Result, _Pr _Pred = {}, _Pj1 _Proj1 = {}, _Pj2 _Proj2 = {}) const {
            auto _First1 = _RANGES begin(_Range1);
            auto _UResult = _Set_difference_unchecked(_Unwrap_range_iter<_Rng1>(_STD move(_First1)), _Uend(_Range1),
                _Ubegin(_Range2), _Uend(_Range2), _Get_unwrapped_unverified(_STD move(_Result)), _Pass_fn(_Pred),
                _Pass_fn(_Proj1), _Pass_fn(_Proj2));
            _Seek_wrapped(_First1, _STD move(_UResult.in));
            _Seek_wrapped(_Result, _STD move(_UResult.out));
            return { _STD move(_First1), _STD move(_Result) };
        }

    private:
        template <class _It1, class _Se1, class _It2, class _Se2, class _Out, class _Pr, class _Pj1, class _Pj2>
        _NODISCARD static constexpr set_difference_result<_It1, _Out> _Set_difference_unchecked(_It1 _First1,
            const _Se1 _Last1, _It2 _First2, const _Se2 _Last2, _Out _Result, _Pr _Pred, _Pj1 _Proj1, _Pj2 _Proj2) {
            _STL_INTERNAL_STATIC_ASSERT(input_iterator<_It1>);
            _STL_INTERNAL_STATIC_ASSERT(sentinel_for<_Se1, _It1>);
            _STL_INTERNAL_STATIC_ASSERT(input_iterator<_It2>);
            _STL_INTERNAL_STATIC_ASSERT(sentinel_for<_Se2, _It2>);
            _STL_INTERNAL_STATIC_ASSERT(weakly_incrementable<_Out>);
            _STL_INTERNAL_STATIC_ASSERT(mergeable<_It1, _It2, _Out, _Pr, _Pj1, _Pj2>);

            for (;;) {
                if (_First1 == _Last1) {
                    return { _STD move(_First1), _STD move(_Result) };
                }

                if (_First2 == _Last2) {
                    return _RANGES _Copy_unchecked(_STD move(_First1), _STD move(_Last1), _STD move(_Result));
                }

                if (_STD invoke(_Pred, _STD invoke(_Proj1, *_First1), _STD invoke(_Proj2, *_First2))) {
                    *_Result = *_First1;
                    ++_Result;
                    ++_First1;
                }
                else {
                    if (!_STD invoke(_Pred, _STD invoke(_Proj2, *_First2), _STD invoke(_Proj1, *_First1))) {
                        ++_First1;
                    }

                    ++_First2;
                }
            }
        }
    };

    _EXPORT_STD inline constexpr _Set_difference_fn set_difference{ _Not_quite_object::_Construct_tag{} };
} // namespace ranges
#endif // __cpp_lib_concepts
#endif // _HAS_CXX17

_EXPORT_STD template <class _InIt1, class _InIt2, class _OutIt, class _Pr>
_CONSTEXPR20 _OutIt set_symmetric_difference(
    _InIt1 _First1, _InIt1 _Last1, _InIt2 _First2, _InIt2 _Last2, _OutIt _Dest, _Pr _Pred) {
    // XOR sets [_First1, _Last1) and [_First2, _Last2)
    _Adl_verify_range(_First1, _Last1);
    _Adl_verify_range(_First2, _Last2);
    auto _UFirst1 = _Get_unwrapped(_First1);
    const auto _ULast1 = _Get_unwrapped(_Last1);
    auto _UFirst2 = _Get_unwrapped(_First2);
    const auto _ULast2 = _Get_unwrapped(_Last2);
    _DEBUG_ORDER_SET_UNWRAPPED(_InIt2, _UFirst1, _ULast1, _Pred);
    _DEBUG_ORDER_SET_UNWRAPPED(_InIt1, _UFirst2, _ULast2, _Pred);
    auto _UDest = _Get_unwrapped_unverified(_Dest);
    while (_UFirst1 != _ULast1 && _UFirst2 != _ULast2) {
        if (_DEBUG_LT_PRED(_Pred, *_UFirst1, *_UFirst2)) { // copy first
            *_UDest = *_UFirst1;
            ++_UDest;
            ++_UFirst1;
        }
        else if (_Pred(*_UFirst2, *_UFirst1)) { // copy second
            *_UDest = *_UFirst2;
            ++_UDest;
            ++_UFirst2;
        }
        else { // advance both
            ++_UFirst1;
            ++_UFirst2;
        }
    }

    _UDest = _STD _Copy_unchecked(_UFirst1, _ULast1, _UDest);
    _Seek_wrapped(_Dest, _STD _Copy_unchecked(_UFirst2, _ULast2, _UDest));
    return _Dest;
}

_EXPORT_STD template <class _InIt1, class _InIt2, class _OutIt>
_CONSTEXPR20 _OutIt set_symmetric_difference(
    _InIt1 _First1, _InIt1 _Last1, _InIt2 _First2, _InIt2 _Last2, _OutIt _Dest) {
    // XOR sets [_First1, _Last1) and [_First2, _Last2)
    return _STD set_symmetric_difference(_First1, _Last1, _First2, _Last2, _Dest, less<>{});
}

#if _HAS_CXX17
_EXPORT_STD template <class _ExPo, class _FwdIt1, class _FwdIt2, class _FwdIt3, class _Pr,
    _Enable_if_execution_policy_t<_ExPo> = 0>
    _FwdIt3 set_symmetric_difference(_ExPo&&, _FwdIt1 _First1, _FwdIt1 _Last1, _FwdIt2 _First2, _FwdIt2 _Last2,
        _FwdIt3 _Dest, _Pr _Pred) noexcept /* terminates */ {
    // XOR sets [_First1, _Last1) and [_First2, _Last2)
    // not parallelized at present, parallelism expected to be feasible in a future release
    _REQUIRE_PARALLEL_ITERATOR(_FwdIt1);
    _REQUIRE_PARALLEL_ITERATOR(_FwdIt2);
    _REQUIRE_CPP17_MUTABLE_ITERATOR(_FwdIt3);
    return _STD set_symmetric_difference(_First1, _Last1, _First2, _Last2, _Dest, _Pass_fn(_Pred));
}

_EXPORT_STD template <class _ExPo, class _FwdIt1, class _FwdIt2, class _FwdIt3,
    _Enable_if_execution_policy_t<_ExPo> = 0>
    _FwdIt3 set_symmetric_difference(_ExPo&&, _FwdIt1 _First1, _FwdIt1 _Last1, _FwdIt2 _First2, _FwdIt2 _Last2,
        _FwdIt3 _Dest) noexcept /* terminates */ {
    // XOR sets [_First1, _Last1) and [_First2, _Last2)
    // not parallelized at present, parallelism expected to be feasible in a future release
    _REQUIRE_PARALLEL_ITERATOR(_FwdIt1);
    _REQUIRE_PARALLEL_ITERATOR(_FwdIt2);
    _REQUIRE_CPP17_MUTABLE_ITERATOR(_FwdIt3);
    return _STD set_symmetric_difference(_First1, _Last1, _First2, _Last2, _Dest);
}

#ifdef __cpp_lib_concepts
namespace ranges {
    _EXPORT_STD template <class _In1, class _In2, class _Out>
        using set_symmetric_difference_result = in_in_out_result<_In1, _In2, _Out>;

    class _Set_symmetric_difference_fn : private _Not_quite_object {
    public:
        using _Not_quite_object::_Not_quite_object;

        template <input_iterator _It1, sentinel_for<_It1> _Se1, input_iterator _It2, sentinel_for<_It2> _Se2,
            weakly_incrementable _Out, class _Pr = ranges::less, class _Pj1 = identity, class _Pj2 = identity>
            requires mergeable<_It1, _It2, _Out, _Pr, _Pj1, _Pj2>
        constexpr set_symmetric_difference_result<_It1, _It2, _Out> operator()(_It1 _First1, _Se1 _Last1, _It2 _First2,
            _Se2 _Last2, _Out _Result, _Pr _Pred = {}, _Pj1 _Proj1 = {}, _Pj2 _Proj2 = {}) const {
            _Adl_verify_range(_First1, _Last1);
            _Adl_verify_range(_First2, _Last2);
            auto _UResult = _Set_symmetric_difference_unchecked(_Unwrap_iter<_Se1>(_STD move(_First1)),
                _Unwrap_sent<_It1>(_STD move(_Last1)), _Unwrap_iter<_Se2>(_STD move(_First2)),
                _Unwrap_sent<_It2>(_STD move(_Last2)), _Get_unwrapped_unverified(_STD move(_Result)), _Pass_fn(_Pred),
                _Pass_fn(_Proj1), _Pass_fn(_Proj2));
            _Seek_wrapped(_First1, _STD move(_UResult.in1));
            _Seek_wrapped(_First2, _STD move(_UResult.in2));
            _Seek_wrapped(_Result, _STD move(_UResult.out));
            return { _STD move(_First1), _STD move(_First2), _STD move(_Result) };
        }

        template <input_range _Rng1, input_range _Rng2, weakly_incrementable _Out, class _Pr = ranges::less,
            class _Pj1 = identity, class _Pj2 = identity>
            requires mergeable<iterator_t<_Rng1>, iterator_t<_Rng2>, _Out, _Pr, _Pj1, _Pj2>
        constexpr set_symmetric_difference_result<borrowed_iterator_t<_Rng1>, borrowed_iterator_t<_Rng2>, _Out>
            operator()(_Rng1&& _Range1, _Rng2&& _Range2, _Out _Result, _Pr _Pred = {}, _Pj1 _Proj1 = {},
                _Pj2 _Proj2 = {}) const {
            auto _First1 = _RANGES begin(_Range1);
            auto _First2 = _RANGES begin(_Range2);
            auto _UResult = _Set_symmetric_difference_unchecked(_Unwrap_range_iter<_Rng1>(_STD move(_First1)),
                _Uend(_Range1), _Unwrap_range_iter<_Rng2>(_STD move(_First2)), _Uend(_Range2),
                _Get_unwrapped_unverified(_STD move(_Result)), _Pass_fn(_Pred), _Pass_fn(_Proj1), _Pass_fn(_Proj2));
            _Seek_wrapped(_First1, _STD move(_UResult.in1));
            _Seek_wrapped(_First2, _STD move(_UResult.in2));
            _Seek_wrapped(_Result, _STD move(_UResult.out));
            return { _STD move(_First1), _STD move(_First2), _STD move(_Result) };
        }

    private:
        template <class _It1, class _Se1, class _It2, class _Se2, class _Out, class _Pr, class _Pj1, class _Pj2>
        _NODISCARD static constexpr set_symmetric_difference_result<_It1, _It2, _Out>
            _Set_symmetric_difference_unchecked(_It1 _First1, const _Se1 _Last1, _It2 _First2, const _Se2 _Last2,
                _Out _Result, _Pr _Pred, _Pj1 _Proj1, _Pj2 _Proj2) {
            _STL_INTERNAL_STATIC_ASSERT(input_iterator<_It1>);
            _STL_INTERNAL_STATIC_ASSERT(sentinel_for<_Se1, _It1>);
            _STL_INTERNAL_STATIC_ASSERT(input_iterator<_It2>);
            _STL_INTERNAL_STATIC_ASSERT(sentinel_for<_Se2, _It2>);
            _STL_INTERNAL_STATIC_ASSERT(weakly_incrementable<_Out>);
            _STL_INTERNAL_STATIC_ASSERT(mergeable<_It1, _It2, _Out, _Pr, _Pj1, _Pj2>);

            for (;;) {
                if (_First1 == _Last1) {
                    auto _UResult = _RANGES _Copy_unchecked(_STD move(_First2), _STD move(_Last2), _STD move(_Result));
                    return { _STD move(_First1), _STD move(_UResult.in), _STD move(_UResult.out) };
                }

                if (_First2 == _Last2) {
                    auto _UResult = _RANGES _Copy_unchecked(_STD move(_First1), _STD move(_Last1), _STD move(_Result));
                    return { _STD move(_UResult.in), _STD move(_First2), _STD move(_UResult.out) };
                }

                if (_STD invoke(_Pred, _STD invoke(_Proj1, *_First1), _STD invoke(_Proj2, *_First2))) {
                    *_Result = *_First1;
                    ++_Result;
                    ++_First1;
                }
                else if (_STD invoke(_Pred, _STD invoke(_Proj2, *_First2), _STD invoke(_Proj1, *_First1))) {
                    *_Result = *_First2;
                    ++_Result;
                    ++_First2;
                }
                else {
                    ++_First1;
                    ++_First2;
                }
            }
        }
    };

    _EXPORT_STD inline constexpr _Set_symmetric_difference_fn set_symmetric_difference{
        _Not_quite_object::_Construct_tag{} };
} // namespace ranges
#endif // __cpp_lib_concepts
#endif // _HAS_CXX17

template <class _FwdIt, class _Pr>
constexpr pair<_FwdIt, _FwdIt> _Minmax_element_unchecked(_FwdIt _First, _FwdIt _Last, _Pr _Pred) {
#if _USE_STD_VECTOR_ALGORITHMS
    if constexpr (_Is_min_max_optimization_safe<_FwdIt, _Pr>) {
        if (!_Is_constant_evaluated()) {
            const auto _First_ptr = _To_address(_First);
            const auto _Result = __std_minmax_element(_First_ptr, _To_address(_Last));
            if constexpr (is_pointer_v<_FwdIt>) {
                return _Result;
            }
            else {
                return { _First + (_Result.first - _First_ptr), _First + (_Result.second - _First_ptr) };
            }
        }
    }
#endif // _USE_STD_VECTOR_ALGORITHMS

    // find smallest and largest elements
    pair<_FwdIt, _FwdIt> _Found(_First, _First);

    if (_First != _Last) {
        while (++_First != _Last) { // process one or two elements
            _FwdIt _Next = _First;
            if (++_Next == _Last) { // process last element
                if (_DEBUG_LT_PRED(_Pred, *_First, *_Found.first)) {
                    _Found.first = _First;
                }
                else if (!_DEBUG_LT_PRED(_Pred, *_First, *_Found.second)) {
                    _Found.second = _First;
                }
            }
            else { // process next two elements
                if (_DEBUG_LT_PRED(_Pred, *_Next, *_First)) { // test _Next for new smallest
                    if (_DEBUG_LT_PRED(_Pred, *_Next, *_Found.first)) {
                        _Found.first = _Next;
                    }

                    if (!_DEBUG_LT_PRED(_Pred, *_First, *_Found.second)) {
                        _Found.second = _First;
                    }
                }
                else { // test _First for new smallest
                    if (_DEBUG_LT_PRED(_Pred, *_First, *_Found.first)) {
                        _Found.first = _First;
                    }

                    if (!_DEBUG_LT_PRED(_Pred, *_Next, *_Found.second)) {
                        _Found.second = _Next;
                    }
                }
                _First = _Next;
            }
        }
    }

    return _Found;
}

_EXPORT_STD template <class _FwdIt, class _Pr>
_NODISCARD constexpr pair<_FwdIt, _FwdIt> minmax_element(_FwdIt _First, _FwdIt _Last, _Pr _Pred) {
    // find smallest and largest elements
    _Adl_verify_range(_First, _Last);
    const auto _Result = _STD _Minmax_element_unchecked(_Get_unwrapped(_First), _Get_unwrapped(_Last), _Pass_fn(_Pred));
    _Seek_wrapped(_Last, _Result.second);
    _Seek_wrapped(_First, _Result.first);
    return { _First, _Last };
}

_EXPORT_STD template <class _FwdIt>
_NODISCARD constexpr pair<_FwdIt, _FwdIt> minmax_element(_FwdIt _First, _FwdIt _Last) {
    // find smallest and largest elements
    return _STD minmax_element(_First, _Last, less<>{});
}

#if _HAS_CXX17
_EXPORT_STD template <class _ExPo, class _FwdIt, class _Pr, _Enable_if_execution_policy_t<_ExPo> = 0>
_NODISCARD pair<_FwdIt, _FwdIt> minmax_element(_ExPo&&, _FwdIt _First, _FwdIt _Last, _Pr _Pred) noexcept
/* terminates */ {
    // find smallest and largest elements
    // not parallelized at present, parallelism expected to be feasible in a future release
    return _STD minmax_element(_First, _Last, _Pass_fn(_Pred));
}

_EXPORT_STD template <class _ExPo, class _FwdIt, _Enable_if_execution_policy_t<_ExPo> = 0>
_NODISCARD pair<_FwdIt, _FwdIt> minmax_element(_ExPo&&, _FwdIt _First, _FwdIt _Last) noexcept /* terminates */ {
    // find smallest and largest elements
    // not parallelized at present, parallelism expected to be feasible in a future release
    return _STD minmax_element(_First, _Last);
}

#ifdef __cpp_lib_concepts
namespace ranges {
    _EXPORT_STD template <class _Ty>
        using minmax_element_result = min_max_result<_Ty>;

    class _Minmax_element_fn : private _Not_quite_object {
    public:
        using _Not_quite_object::_Not_quite_object;

        template <forward_iterator _It, sentinel_for<_It> _Se, class _Pj = identity,
            indirect_strict_weak_order<projected<_It, _Pj>> _Pr = ranges::less>
        _NODISCARD constexpr minmax_element_result<_It> operator()(
            _It _First, _Se _Last, _Pr _Pred = {}, _Pj _Proj = {}) const {
            _STD _Adl_verify_range(_First, _Last);
            auto _UResult = _Minmax_element_fwd_unchecked(_RANGES _Unwrap_iter<_Se>(_STD move(_First)),
                _RANGES _Unwrap_sent<_It>(_STD move(_Last)), _STD _Pass_fn(_Pred), _STD _Pass_fn(_Proj));
            _STD _Seek_wrapped(_First, _STD move(_UResult.min));
            auto _Second = _First;
            _STD _Seek_wrapped(_Second, _STD move(_UResult.max));
            return { _STD move(_First), _STD move(_Second) };
        }

        template <forward_range _Rng, class _Pj = identity,
            indirect_strict_weak_order<projected<iterator_t<_Rng>, _Pj>> _Pr = ranges::less>
        _NODISCARD constexpr minmax_element_result<borrowed_iterator_t<_Rng>> operator()(
            _Rng&& _Range, _Pr _Pred = {}, _Pj _Proj = {}) const {
            auto _UResult = _Minmax_element_fwd_unchecked(
                _RANGES _Ubegin(_Range), _RANGES _Uend(_Range), _STD _Pass_fn(_Pred), _STD _Pass_fn(_Proj));
            return {
                _Rewrap_iterator(_Range, _STD move(_UResult.min)), _Rewrap_iterator(_Range, _STD move(_UResult.max)) };
        }

    private:
        template <class _It, class _Se, class _Pr, class _Pj>
        _NODISCARD static constexpr min_max_result<_It> _Minmax_element_fwd_unchecked(
            _It _First, const _Se _Last, _Pr _Pred, _Pj _Proj) {
            _STL_INTERNAL_STATIC_ASSERT(forward_iterator<_It>);
            _STL_INTERNAL_STATIC_ASSERT(sentinel_for<_Se, _It>);
            _STL_INTERNAL_STATIC_ASSERT(indirect_strict_weak_order<_Pr, projected<_It, _Pj>>);

#if _USE_STD_VECTOR_ALGORITHMS
            if constexpr (is_same_v<_Pj, identity> && _Is_min_max_optimization_safe<_It, _Pr>
                && sized_sentinel_for<_Se, _It>) {
                if (!_STD is_constant_evaluated()) {
                    const auto _First_ptr = _STD to_address(_First);
                    const auto _Last_ptr = _First_ptr + (_Last - _First);
                    const auto _Result = _CSTD __std_minmax_element(_First_ptr, _Last_ptr);
                    if constexpr (is_pointer_v<_It>) {
                        return { _Result.first, _Result.second };
                    }
                    else {
                        return { _First + (_Result.first - _First_ptr), _First + (_Result.second - _First_ptr) };
                    }
                }
            }
#endif // _USE_STD_VECTOR_ALGORITHMS

            min_max_result<_It> _Found{ _First, _First };

            if (_First == _Last) {
                return _Found;
            }

            while (++_First != _Last) { // process one or two elements
                _It _Prev = _First;
                if (++_First == _Last) { // process last element
                    if (_STD invoke(_Pred, _STD invoke(_Proj, *_Prev), _STD invoke(_Proj, *_Found.min))) {
                        _Found.min = _Prev;
                    }
                    else if (!_STD invoke(_Pred, _STD invoke(_Proj, *_Prev), _STD invoke(_Proj, *_Found.max))) {
                        _Found.max = _Prev;
                    }

                    break;
                }

                // process next two elements
                if (_STD invoke(_Pred, _STD invoke(_Proj, *_First), _STD invoke(_Proj, *_Prev))) {
                    // test _First for new smallest
                    if (_STD invoke(_Pred, _STD invoke(_Proj, *_First), _STD invoke(_Proj, *_Found.min))) {
                        _Found.min = _First;
                    }

                    if (!_STD invoke(_Pred, _STD invoke(_Proj, *_Prev), _STD invoke(_Proj, *_Found.max))) {
                        _Found.max = _Prev;
                    }
                }
                else { // test _Prev for new smallest
                    if (_STD invoke(_Pred, _STD invoke(_Proj, *_Prev), _STD invoke(_Proj, *_Found.min))) {
                        _Found.min = _Prev;
                    }

                    if (!_STD invoke(_Pred, _STD invoke(_Proj, *_First), _STD invoke(_Proj, *_Found.max))) {
                        _Found.max = _First;
                    }
                }
            }

            return _Found;
        }
    };

    _EXPORT_STD inline constexpr _Minmax_element_fn minmax_element{ _Not_quite_object::_Construct_tag{} };
} // namespace ranges
#endif // __cpp_lib_concepts
#endif // _HAS_CXX17

_EXPORT_STD template <class _Ty, class _Pr>
_NODISCARD constexpr pair<const _Ty&, const _Ty&> minmax(const _Ty& _Left, const _Ty& _Right, _Pr _Pred) noexcept(
    noexcept(_DEBUG_LT_PRED(_Pred, _Right, _Left))) /* strengthened */ {
    // return pair(leftmost/smaller, rightmost/larger) of _Left and _Right
    if (_DEBUG_LT_PRED(_Pred, _Right, _Left)) {
        return { _Right, _Left };
    }

    return { _Left, _Right };
}

_EXPORT_STD template <class _Ty, class _Pr>
_NODISCARD constexpr pair<_Ty, _Ty> minmax(initializer_list<_Ty> _Ilist, _Pr _Pred) {
    // return {leftmost/smallest, rightmost/largest}
    pair<const _Ty*, const _Ty*> _Res = _STD _Minmax_element_unchecked(_Ilist.begin(), _Ilist.end(), _Pass_fn(_Pred));
    return pair<_Ty, _Ty>(*_Res.first, *_Res.second);
}

_EXPORT_STD template <class _Ty>
_NODISCARD constexpr pair<const _Ty&, const _Ty&> minmax(const _Ty& _Left, const _Ty& _Right) noexcept(
    noexcept(_Right < _Left)) /* strengthened */ {
    // return pair(leftmost/smaller, rightmost/larger) of _Left and _Right
    if (_Right < _Left) {
        _STL_ASSERT(!(_Left < _Right), "invalid comparator");
        return { _Right, _Left };
    }

    return { _Left, _Right };
}

_EXPORT_STD template <class _Ty>
_NODISCARD constexpr pair<_Ty, _Ty> minmax(initializer_list<_Ty> _Ilist) {
    // return {leftmost/smallest, rightmost/largest}
    return _STD minmax(_Ilist, less<>{});
}

#ifdef __cpp_lib_concepts
namespace ranges {
    _EXPORT_STD template <class _Ty>
        using minmax_result = min_max_result<_Ty>;

    class _Minmax_fn : private _Not_quite_object {
    public:
        using _Not_quite_object::_Not_quite_object;

        template <class _Ty, class _Pj = identity,
            indirect_strict_weak_order<projected<const _Ty*, _Pj>> _Pr = ranges::less>
        _NODISCARD constexpr minmax_result<const _Ty&> operator()(
            const _Ty& _Left, const _Ty& _Right, _Pr _Pred = {}, _Pj _Proj = {}) const {
            if (_STD invoke(_Pred, _STD invoke(_Proj, _Right), _STD invoke(_Proj, _Left))) {
                return { _Right, _Left };
            }
            else {
                return { _Left, _Right };
            }
        }

        template <copyable _Ty, class _Pj = identity,
            indirect_strict_weak_order<projected<const _Ty*, _Pj>> _Pr = ranges::less>
        _NODISCARD constexpr minmax_result<_Ty> operator()(
            initializer_list<_Ty> _Range, _Pr _Pred = {}, _Pj _Proj = {}) const {
            const auto _First = _Range.begin();
            const auto _Last = _Range.end();
            _STL_ASSERT(_First != _Last,
                "An initializer_list passed to std::ranges::minmax must not be empty. (N4950 [alg.min.max]/21)");
            return _Minmax_fwd_unchecked(_First, _Last, _STD _Pass_fn(_Pred), _STD _Pass_fn(_Proj));
        }

        template <input_range _Rng, class _Pj = identity,
            indirect_strict_weak_order<projected<iterator_t<_Rng>, _Pj>> _Pr = ranges::less>
            requires indirectly_copyable_storable<iterator_t<_Rng>, range_value_t<_Rng>*>
        _NODISCARD constexpr minmax_result<range_value_t<_Rng>> operator()(
            _Rng&& _Range, _Pr _Pred = {}, _Pj _Proj = {}) const {
            auto _UFirst = _RANGES _Ubegin(_Range);
            auto _ULast = _RANGES _Uend(_Range);
            _STL_ASSERT(
                _UFirst != _ULast, "A range passed to std::ranges::minmax must not be empty. (N4950 [alg.min.max]/21)");
            if constexpr (forward_range<_Rng> && _Prefer_iterator_copies<iterator_t<_Rng>>) {
                return _Minmax_fwd_unchecked(
                    _STD move(_UFirst), _STD move(_ULast), _STD _Pass_fn(_Pred), _STD _Pass_fn(_Proj));
            }
            else {
                using _Vty = range_value_t<_Rng>;
                // This initialization is correct, similar to the N4950 [dcl.init.aggr]/6 example
                minmax_result<_Vty> _Found = { static_cast<_Vty>(*_UFirst), _Found.min };
                if (_UFirst == _ULast) {
                    return _Found;
                }

                while (++_UFirst != _ULast) { // process one or two elements
                    auto _Prev = *_UFirst;
                    if (++_UFirst == _ULast) { // process last element
                        if (_STD invoke(_Pred, _STD invoke(_Proj, _Prev), _STD invoke(_Proj, _Found.min))) {
                            _Found.min = _STD move(_Prev);
                        }
                        else if (!_STD invoke(_Pred, _STD invoke(_Proj, _Prev), _STD invoke(_Proj, _Found.max))) {
                            _Found.max = _STD move(_Prev);
                        }

                        break;
                    }

                    // process next two elements
                    if (_STD invoke(_Pred, _STD invoke(_Proj, *_UFirst), _STD invoke(_Proj, _Prev))) {
                        // test _UFirst for new smallest
                        if (_STD invoke(_Pred, _STD invoke(_Proj, *_UFirst), _STD invoke(_Proj, _Found.min))) {
                            _Found.min = *_UFirst;
                        }

                        if (!_STD invoke(_Pred, _STD invoke(_Proj, _Prev), _STD invoke(_Proj, _Found.max))) {
                            _Found.max = _STD move(_Prev);
                        }
                    }
                    else { // test _Prev for new smallest
                        if (_STD invoke(_Pred, _STD invoke(_Proj, _Prev), _STD invoke(_Proj, _Found.min))) {
                            _Found.min = _STD move(_Prev);
                        }

                        if (!_STD invoke(_Pred, _STD invoke(_Proj, *_UFirst), _STD invoke(_Proj, _Found.max))) {
                            _Found.max = *_UFirst;
                        }
                    }
                }

                return _Found;
            }
        }

    private:
        template <class _It, class _Se, class _Pr, class _Pj>
        _NODISCARD static constexpr minmax_result<iter_value_t<_It>> _Minmax_fwd_unchecked(
            _It _First, const _Se _Last, _Pr _Pred, _Pj _Proj) {
            _STL_INTERNAL_STATIC_ASSERT(forward_iterator<_It>);
            _STL_INTERNAL_STATIC_ASSERT(sentinel_for<_Se, _It>);
            _STL_INTERNAL_STATIC_ASSERT(indirect_strict_weak_order<_Pr, projected<_It, _Pj>>);

            _STL_INTERNAL_CHECK(_First != _Last);

            using _Vty = iter_value_t<_It>;
#if _USE_STD_VECTOR_ALGORITHMS
            if constexpr (is_same_v<_Pj, identity> && _Is_min_max_optimization_safe<_It, _Pr>
                && sized_sentinel_for<_Se, _It>) {
                if (!_STD is_constant_evaluated()) {
                    const auto _First_ptr = _STD to_address(_First);
                    const auto _Last_ptr = _First_ptr + (_Last - _First);
                    const auto _Result = _CSTD __std_minmax_element(_First_ptr, _Last_ptr);
                    return { static_cast<_Vty>(*_Result.first), static_cast<_Vty>(*_Result.second) };
                }
            }
#endif // _USE_STD_VECTOR_ALGORITHMS

            auto _Found_min = _First;
            if (++_First == _Last) {
                // This initialization is correct, similar to the N4950 [dcl.init.aggr]/6 example
                minmax_result<_Vty> _Result = { static_cast<_Vty>(*_Found_min), _Result.min };
                return _Result;
            }

            auto _Found_max = _First;
            if (_STD invoke(_Pred, _STD invoke(_Proj, *_Found_max), _STD invoke(_Proj, *_Found_min))) {
                _RANGES swap(_Found_min, _Found_max);
            }

            while (++_First != _Last) { // process one or two elements
                _It _Prev = _First;
                if (++_First == _Last) { // process last element
                    if (_STD invoke(_Pred, _STD invoke(_Proj, *_Prev), _STD invoke(_Proj, *_Found_min))) {
                        _Found_min = _Prev;
                    }
                    else if (!_STD invoke(_Pred, _STD invoke(_Proj, *_Prev), _STD invoke(_Proj, *_Found_max))) {
                        _Found_max = _Prev;
                    }

                    break;
                }

                // process two elements
                if (_STD invoke(_Pred, _STD invoke(_Proj, *_First), _STD invoke(_Proj, *_Prev))) {
                    // test _First for new smallest
                    if (_STD invoke(_Pred, _STD invoke(_Proj, *_First), _STD invoke(_Proj, *_Found_min))) {
                        _Found_min = _First;
                    }

                    if (!_STD invoke(_Pred, _STD invoke(_Proj, *_Prev), _STD invoke(_Proj, *_Found_max))) {
                        _Found_max = _Prev;
                    }
                }
                else { // test _Prev for new smallest
                    if (_STD invoke(_Pred, _STD invoke(_Proj, *_Prev), _STD invoke(_Proj, *_Found_min))) {
                        _Found_min = _Prev;
                    }

                    if (!_STD invoke(_Pred, _STD invoke(_Proj, *_First), _STD invoke(_Proj, *_Found_max))) {
                        _Found_max = _First;
                    }
                }
            }

            return { static_cast<_Vty>(*_Found_min), static_cast<_Vty>(*_Found_max) };
        }
    };

    _EXPORT_STD inline constexpr _Minmax_fn minmax{ _Not_quite_object::_Construct_tag{} };
} // namespace ranges
#endif // __cpp_lib_concepts

_EXPORT_STD template <class _BidIt, class _Pr>
_CONSTEXPR20 bool next_permutation(_BidIt _First, _BidIt _Last, _Pr _Pred) {
    // permute and test for pure ascending
    _Adl_verify_range(_First, _Last);
    auto _UFirst = _Get_unwrapped(_First);
    const auto _ULast = _Get_unwrapped(_Last);
    auto _UNext = _ULast;
    if (_UFirst == _ULast || _UFirst == --_UNext) {
        return false;
    }

    for (;;) { // find rightmost element smaller than successor
        auto _UNext1 = _UNext;
        if (_DEBUG_LT_PRED(_Pred, *--_UNext, *_UNext1)) { // swap with rightmost element that's smaller, flip suffix
            auto _UMid = _ULast;
            do {
                --_UMid;
            } while (!_DEBUG_LT_PRED(_Pred, *_UNext, *_UMid));

            swap(*_UNext, *_UMid); // intentional ADL
            _STD reverse(_UNext1, _ULast);
            return true;
        }

        if (_UNext == _UFirst) { // pure descending, flip all
            _STD reverse(_UFirst, _ULast);
            return false;
        }
    }
}

_EXPORT_STD template <class _BidIt>
_CONSTEXPR20 bool next_permutation(_BidIt _First, _BidIt _Last) {
    // permute and test for pure ascending
    return _STD next_permutation(_First, _Last, less<>{});
}

#ifdef __cpp_lib_concepts
namespace ranges {
    _EXPORT_STD template <class _In>
        using next_permutation_result = in_found_result<_In>;

    class _Next_permutation_fn : private _Not_quite_object {
    public:
        using _Not_quite_object::_Not_quite_object;

        template <bidirectional_iterator _It, sentinel_for<_It> _Se, class _Pr = ranges::less, class _Pj = identity>
            requires sortable<_It, _Pr, _Pj>
        constexpr next_permutation_result<_It> operator()(_It _First, _Se _Last, _Pr _Pred = {}, _Pj _Proj = {}) const {
            _Adl_verify_range(_First, _Last);
            auto _UFirst = _Unwrap_iter<_Se>(_STD move(_First));
            auto _ULast = _Get_final_iterator_unwrapped<_It>(_UFirst, _STD move(_Last));
            _Seek_wrapped(_First, _ULast);
            const bool _Found =
                _Next_permutation_common(_STD move(_UFirst), _STD move(_ULast), _Pass_fn(_Pred), _Pass_fn(_Proj));
            return { _STD move(_First), _Found };
        }

        template <bidirectional_range _Rng, class _Pr = ranges::less, class _Pj = identity>
            requires sortable<iterator_t<_Rng>, _Pr, _Pj>
        constexpr next_permutation_result<borrowed_iterator_t<_Rng>> operator()(
            _Rng&& _Range, _Pr _Pred = {}, _Pj _Proj = {}) const {
            auto _ULast = _Get_final_iterator_unwrapped(_Range);
            const bool _Found = _Next_permutation_common(_Ubegin(_Range), _ULast, _Pass_fn(_Pred), _Pass_fn(_Proj));
            return { _Rewrap_iterator(_Range, _STD move(_ULast)), _Found };
        }

    private:
        template <class _It, class _Pr, class _Pj>
        _NODISCARD static constexpr bool _Next_permutation_common(_It _First, _It _Last, _Pr _Pred, _Pj _Proj) {
            _STL_INTERNAL_STATIC_ASSERT(bidirectional_iterator<_It>);
            _STL_INTERNAL_STATIC_ASSERT(sortable<_It, _Pr, _Pj>);

            auto _Next = _Last;
            if (_First == _Last || _First == --_Next) {
                return false;
            }

            for (;;) { // find rightmost element smaller than successor
                auto _Next1 = _Next;
                if (_STD invoke(_Pred, _STD invoke(_Proj, *--_Next), _STD invoke(_Proj, *_Next1))) {
                    // swap with rightmost element that's smaller, flip suffix
                    auto _Mid = _Last;
                    do {
                        --_Mid;
                    } while (!_STD invoke(_Pred, _STD invoke(_Proj, *_Next), _STD invoke(_Proj, *_Mid)));

                    _RANGES iter_swap(_Next, _Mid);
                    _Reverse_common(_STD move(_Next1), _STD move(_Last));
                    return true;
                }

                if (_Next == _First) { // pure descending, flip all
                    _Reverse_common(_STD move(_First), _STD move(_Last));
                    return false;
                }
            }
        }
    };

    _EXPORT_STD inline constexpr _Next_permutation_fn next_permutation{ _Not_quite_object::_Construct_tag{} };
} // namespace ranges
#endif // __cpp_lib_concepts

_EXPORT_STD template <class _BidIt, class _Pr>
_CONSTEXPR20 bool prev_permutation(_BidIt _First, _BidIt _Last, _Pr _Pred) {
    // reverse permute and test for pure descending
    _Adl_verify_range(_First, _Last);
    auto _UFirst = _Get_unwrapped(_First);
    const auto _ULast = _Get_unwrapped(_Last);
    auto _UNext = _ULast;
    if (_UFirst == _ULast || _UFirst == --_UNext) {
        return false;
    }

    for (;;) { // find rightmost element not smaller than successor
        auto _UNext1 = _UNext;
        if (_DEBUG_LT_PRED(_Pred, *_UNext1, *--_UNext)) { // swap with rightmost element that's not smaller, flip suffix
            auto _UMid = _ULast;
            do {
                --_UMid;
            } while (!_DEBUG_LT_PRED(_Pred, *_UMid, *_UNext));

            swap(*_UNext, *_UMid); // intentional ADL
            _STD reverse(_UNext1, _ULast);
            return true;
        }

        if (_UNext == _UFirst) { // pure ascending, flip all
            _STD reverse(_UFirst, _ULast);
            return false;
        }
    }
}

_EXPORT_STD template <class _BidIt>
_CONSTEXPR20 bool prev_permutation(_BidIt _First, _BidIt _Last) {
    // reverse permute and test for pure descending
    return _STD prev_permutation(_First, _Last, less<>{});
}

#ifdef __cpp_lib_concepts
namespace ranges {
    _EXPORT_STD template <class _In>
        using prev_permutation_result = in_found_result<_In>;

    class _Prev_permutation_fn : private _Not_quite_object {
    public:
        using _Not_quite_object::_Not_quite_object;

        template <bidirectional_iterator _It, sentinel_for<_It> _Se, class _Pr = ranges::less, class _Pj = identity>
            requires sortable<_It, _Pr, _Pj>
        constexpr prev_permutation_result<_It> operator()(_It _First, _Se _Last, _Pr _Pred = {}, _Pj _Proj = {}) const {
            _Adl_verify_range(_First, _Last);
            auto _UFirst = _Unwrap_iter<_Se>(_STD move(_First));
            auto _ULast = _Get_final_iterator_unwrapped<_It>(_UFirst, _STD move(_Last));
            _Seek_wrapped(_First, _ULast);
            const bool _Found =
                _Prev_permutation_common(_STD move(_UFirst), _STD move(_ULast), _Pass_fn(_Pred), _Pass_fn(_Proj));
            return { _STD move(_First), _Found };
        }

        template <bidirectional_range _Rng, class _Pr = ranges::less, class _Pj = identity>
            requires sortable<iterator_t<_Rng>, _Pr, _Pj>
        constexpr prev_permutation_result<borrowed_iterator_t<_Rng>> operator()(
            _Rng&& _Range, _Pr _Pred = {}, _Pj _Proj = {}) const {
            auto _ULast = _Get_final_iterator_unwrapped(_Range);
            const bool _Found = _Prev_permutation_common(_Ubegin(_Range), _ULast, _Pass_fn(_Pred), _Pass_fn(_Proj));
            return { _Rewrap_iterator(_Range, _STD move(_ULast)), _Found };
        }

    private:
        template <class _It, class _Pr, class _Pj>
        _NODISCARD static constexpr bool _Prev_permutation_common(_It _First, _It _Last, _Pr _Pred, _Pj _Proj) {
            _STL_INTERNAL_STATIC_ASSERT(bidirectional_iterator<_It>);
            _STL_INTERNAL_STATIC_ASSERT(sortable<_It, _Pr, _Pj>);

            auto _Next = _Last;
            if (_First == _Last || _First == --_Next) {
                return false;
            }

            for (;;) { // find rightmost element not smaller than successor
                auto _Next1 = _Next;
                if (_STD invoke(_Pred, _STD invoke(_Proj, *_Next1), _STD invoke(_Proj, *--_Next))) {
                    // swap with rightmost element that's not smaller, flip suffix
                    auto _Mid = _Last;
                    do {
                        --_Mid;
                    } while (!_STD invoke(_Pred, _STD invoke(_Proj, *_Mid), _STD invoke(_Proj, *_Next)));

                    _RANGES iter_swap(_Next, _Mid);
                    _Reverse_common(_STD move(_Next1), _STD move(_Last));
                    return true;
                }

                if (_Next == _First) { // pure ascending, flip all
                    _Reverse_common(_STD move(_First), _STD move(_Last));
                    return false;
                }
            }
        }
    };

    _EXPORT_STD inline constexpr _Prev_permutation_fn prev_permutation{ _Not_quite_object::_Construct_tag{} };
} // namespace ranges
#endif // __cpp_lib_concepts

_EXPORT_STD template <class _FwdIt, class _Pr>
_NODISCARD _CONSTEXPR20 _FwdIt is_sorted_until(const _FwdIt _First, _FwdIt _Last, _Pr _Pred) {
    // find extent of range that is ordered by predicate
    _Adl_verify_range(_First, _Last);
    auto _UFirst = _Get_unwrapped(_First);
    auto _ULast = _Get_unwrapped(_Last);
    if (_UFirst != _ULast) {
        for (auto _UNext = _UFirst; ++_UNext != _ULast; ++_UFirst) {
            if (_DEBUG_LT_PRED(_Pred, *_UNext, *_UFirst)) {
                _ULast = _UNext;
                break;
            }
        }
    }

    _Seek_wrapped(_Last, _ULast);
    return _Last;
}

_EXPORT_STD template <class _FwdIt, class _Pr>
_NODISCARD _CONSTEXPR20 bool is_sorted(_FwdIt _First, _FwdIt _Last, _Pr _Pred) {
    // test if range is ordered by predicate
    _Adl_verify_range(_First, _Last);
    const auto _UFirst = _Get_unwrapped(_First);
    const auto _ULast = _Get_unwrapped(_Last);
    return _STD is_sorted_until(_UFirst, _ULast, _Pass_fn(_Pred)) == _ULast;
}

_EXPORT_STD template <class _FwdIt>
_NODISCARD _CONSTEXPR20 _FwdIt is_sorted_until(_FwdIt _First, _FwdIt _Last) {
    // find extent of range that is ordered by operator<
    return _STD is_sorted_until(_First, _Last, less<>{});
}

_EXPORT_STD template <class _FwdIt>
_NODISCARD _CONSTEXPR20 bool is_sorted(_FwdIt _First, _FwdIt _Last) { // test if range is ordered by operator<
    return _STD is_sorted(_First, _Last, less<>{});
}

#if _HAS_CXX17
_EXPORT_STD template <class _ExPo, class _FwdIt, class _Pr, _Enable_if_execution_policy_t<_ExPo> = 0>
_NODISCARD _FwdIt is_sorted_until(_ExPo&&, _FwdIt _First, _FwdIt _Last, _Pr _Pred) noexcept; // terminates

_EXPORT_STD template <class _ExPo, class _FwdIt, class _Pr, _Enable_if_execution_policy_t<_ExPo> = 0>
_NODISCARD bool is_sorted(_ExPo&& _Exec, _FwdIt _First, _FwdIt _Last, _Pr _Pred) noexcept /* terminates */ {
    // test if range is ordered by predicate
    return _STD is_sorted_until(_STD forward<_ExPo>(_Exec), _First, _Last, _Pass_fn(_Pred)) == _Last;
}

_EXPORT_STD template <class _ExPo, class _FwdIt, _Enable_if_execution_policy_t<_ExPo> = 0>
_NODISCARD _FwdIt is_sorted_until(_ExPo&& _Exec, _FwdIt _First, _FwdIt _Last) noexcept /* terminates */ {
    // find extent of range that is ordered by operator<
    return _STD is_sorted_until(_STD forward<_ExPo>(_Exec), _First, _Last, less{});
}

_EXPORT_STD template <class _ExPo, class _FwdIt, _Enable_if_execution_policy_t<_ExPo> = 0>
_NODISCARD bool is_sorted(_ExPo&& _Exec, _FwdIt _First, _FwdIt _Last) noexcept /* terminates */ {
    // test if range is ordered by operator<
    return _STD is_sorted(_STD forward<_ExPo>(_Exec), _First, _Last, less{});
}

#ifdef __cpp_lib_concepts
namespace ranges {
    class _Is_sorted_fn : private _Not_quite_object {
    public:
        using _Not_quite_object::_Not_quite_object;

        template <forward_iterator _It, sentinel_for<_It> _Se, class _Pj = identity,
            indirect_strict_weak_order<projected<_It, _Pj>> _Pr = ranges::less>
        _NODISCARD constexpr bool operator()(_It _First, _Se _Last, _Pr _Pred = {}, _Pj _Proj = {}) const {
            _Adl_verify_range(_First, _Last);
            const auto _ULast = _Unwrap_sent<_It>(_STD move(_Last));
            const auto _UFirst = _RANGES _Is_sorted_until_unchecked(
                _Unwrap_iter<_Se>(_STD move(_First)), _ULast, _Pass_fn(_Pred), _Pass_fn(_Proj));
            return _UFirst == _ULast;
        }

        template <forward_range _Rng, class _Pj = identity,
            indirect_strict_weak_order<projected<iterator_t<_Rng>, _Pj>> _Pr = ranges::less>
        _NODISCARD constexpr bool operator()(_Rng&& _Range, _Pr _Pred = {}, _Pj _Proj = {}) const {
            const auto _ULast = _Uend(_Range);
            const auto _UFirst =
                _RANGES _Is_sorted_until_unchecked(_Ubegin(_Range), _ULast, _Pass_fn(_Pred), _Pass_fn(_Proj));
            return _UFirst == _ULast;
        }
    };

    _EXPORT_STD inline constexpr _Is_sorted_fn is_sorted{ _Not_quite_object::_Construct_tag{} };

    class _Is_sorted_until_fn : private _Not_quite_object {
    public:
        using _Not_quite_object::_Not_quite_object;

        template <forward_iterator _It, sentinel_for<_It> _Se, class _Pj = identity,
            indirect_strict_weak_order<projected<_It, _Pj>> _Pr = ranges::less>
        _NODISCARD constexpr _It operator()(_It _First, _Se _Last, _Pr _Pred = {}, _Pj _Proj = {}) const {
            _Adl_verify_range(_First, _Last);
            auto _UFirst = _RANGES _Is_sorted_until_unchecked(_Unwrap_iter<_Se>(_STD move(_First)),
                _Unwrap_sent<_It>(_STD move(_Last)), _Pass_fn(_Pred), _Pass_fn(_Proj));
            _Seek_wrapped(_First, _STD move(_UFirst));
            return _First;
        }

        template <forward_range _Rng, class _Pj = identity,
            indirect_strict_weak_order<projected<iterator_t<_Rng>, _Pj>> _Pr = ranges::less>
        _NODISCARD constexpr borrowed_iterator_t<_Rng> operator()(_Rng&& _Range, _Pr _Pred = {}, _Pj _Proj = {}) const {
            auto _UResult =
                _RANGES _Is_sorted_until_unchecked(_Ubegin(_Range), _Uend(_Range), _Pass_fn(_Pred), _Pass_fn(_Proj));
            return _Rewrap_iterator(_Range, _STD move(_UResult));
        }
    };

    _EXPORT_STD inline constexpr _Is_sorted_until_fn is_sorted_until{ _Not_quite_object::_Construct_tag{} };
} // namespace ranges
#endif // __cpp_lib_concepts

_EXPORT_STD template <class _Ty, class _Pr>
_NODISCARD constexpr const _Ty& clamp(const _Ty& _Val, const _Ty& _Min_val, const _Ty& _Max_val, _Pr _Pred) {
    // returns _Val constrained to [_Min_val, _Max_val]
#if _ITERATOR_DEBUG_LEVEL == 2
    if (_DEBUG_LT_PRED(_Pred, _Max_val, _Min_val)) {
        _STL_REPORT_ERROR("invalid bounds arguments passed to std::clamp");
        return _Val;
    }
#endif // _ITERATOR_DEBUG_LEVEL == 2

    if constexpr (conjunction_v<is_scalar<_Ty>, disjunction<is_same<_Pr, less<>>, is_same<_Pr, less<_Ty>>>>) {
        // Avoid early return to promote branchless code generation; see GH-2334
        const _Ty& _Tmp = _DEBUG_LT_PRED(_Pred, _Val, _Min_val) ? _Min_val : _Val;
        return _DEBUG_LT_PRED(_Pred, _Max_val, _Val) ? _Max_val : _Tmp;
    }
    else {

        if (_DEBUG_LT_PRED(_Pred, _Max_val, _Val)) {
            return _Max_val;
        }

        if (_DEBUG_LT_PRED(_Pred, _Val, _Min_val)) {
            return _Min_val;
        }

        return _Val;
    }
}

_EXPORT_STD template <class _Ty>
_NODISCARD constexpr const _Ty& clamp(const _Ty& _Val, const _Ty& _Min_val, const _Ty& _Max_val) {
    // returns _Val constrained to [_Min_val, _Max_val]
    return _STD clamp(_Val, _Min_val, _Max_val, less{});
}

#ifdef __cpp_lib_concepts
namespace ranges {
    class _Clamp_fn : private _Not_quite_object {
    public:
        using _Not_quite_object::_Not_quite_object;

        template <class _Ty, class _Pj = identity,
            indirect_strict_weak_order<projected<const _Ty*, _Pj>> _Pr = ranges::less>
        _NODISCARD constexpr const _Ty& operator()(
            const _Ty& _Val, const _Ty& _Lo, const _Ty& _Hi, _Pr _Pred = {}, _Pj _Proj = {}) const {
            _STL_ASSERT(!_STD invoke(_Pred, _STD invoke(_Proj, _Hi), _STD invoke(_Proj, _Lo)),
                "The lower bound cannot be greater than the upper bound in a call to std::ranges::clamp "
                "(N4950 [alg.clamp]/2).");

            auto&& _Temp = _STD invoke(_Proj, _Val);
            if (_STD invoke(_Pred, _STD forward<decltype(_Temp)>(_Temp), _STD invoke(_Proj, _Lo))) {
                return _Lo;
            }
            // The double forward is safe because regular_invocable requires that the invocation of the predicate not
            // modify _Temp in a manner observable to equality-preserving expressions.
            if (_STD invoke(_Pred, _STD invoke(_Proj, _Hi), _STD forward<decltype(_Temp)>(_Temp))) {
                return _Hi;
            }

            return _Val;
        }
    };

    _EXPORT_STD inline constexpr _Clamp_fn clamp{ _Not_quite_object::_Construct_tag{} };

    class _Lexicographical_compare_fn : private _Not_quite_object {
    public:
        using _Not_quite_object::_Not_quite_object;

        template <input_iterator _It1, sentinel_for<_It1> _Se1, input_iterator _It2, sentinel_for<_It2> _Se2,
            class _Pj1 = identity, class _Pj2 = identity,
            indirect_strict_weak_order<projected<_It1, _Pj1>, projected<_It2, _Pj2>> _Pr = ranges::less>
        _NODISCARD constexpr bool operator()(_It1 _First1, _Se1 _Last1, _It2 _First2, _Se2 _Last2, _Pr _Pred = {},
            _Pj1 _Proj1 = {}, _Pj2 _Proj2 = {}) const {
            _Adl_verify_range(_First1, _Last1);
            _Adl_verify_range(_First2, _Last2);

            return _Lexicographical_compare_unchecked(_Unwrap_iter<_Se1>(_STD move(_First1)),
                _Unwrap_sent<_It1>(_STD move(_Last1)), _Unwrap_iter<_Se2>(_STD move(_First2)),
                _Unwrap_sent<_It2>(_STD move(_Last2)), _Pass_fn(_Pred), _Pass_fn(_Proj1), _Pass_fn(_Proj2));
        }

        template <input_range _Rng1, input_range _Rng2, class _Pj1 = identity, class _Pj2 = identity,
            indirect_strict_weak_order<projected<iterator_t<_Rng1>, _Pj1>, projected<iterator_t<_Rng2>, _Pj2>> _Pr =
            ranges::less>
        _NODISCARD constexpr bool operator()(
            _Rng1&& _Range1, _Rng2&& _Range2, _Pr _Pred = {}, _Pj1 _Proj1 = {}, _Pj2 _Proj2 = {}) const {
            return _Lexicographical_compare_unchecked(_Ubegin(_Range1), _Uend(_Range1), _Ubegin(_Range2),
                _Uend(_Range2), _Pass_fn(_Pred), _Pass_fn(_Proj1), _Pass_fn(_Proj2));
        }

    private:
        template <class _It1, class _Se1, class _It2, class _Se2, class _Pr, class _Pj1, class _Pj2>
        _NODISCARD static constexpr bool _Lexicographical_compare_unchecked(
            _It1 _First1, _Se1 _Last1, _It2 _First2, _Se2 _Last2, _Pr _Pred, _Pj1 _Proj1, _Pj2 _Proj2) {
            _STL_INTERNAL_STATIC_ASSERT(input_iterator<_It1>);
            _STL_INTERNAL_STATIC_ASSERT(sentinel_for<_Se1, _It1>);
            _STL_INTERNAL_STATIC_ASSERT(input_iterator<_It2>);
            _STL_INTERNAL_STATIC_ASSERT(sentinel_for<_Se2, _It2>);
            _STL_INTERNAL_STATIC_ASSERT(indirect_strict_weak_order<_Pr, projected<_It1, _Pj1>, projected<_It2, _Pj2>>);

            using _Memcmp_classification_pred = _Lex_compare_memcmp_classify<_It1, _It2, _Pr>;
            constexpr bool _Is_sized1 = sized_sentinel_for<_Se1, _It1>;
            constexpr bool _Is_sized2 = sized_sentinel_for<_Se2, _It2>;
            if constexpr (!is_void_v<_Memcmp_classification_pred> && _Sized_or_unreachable_sentinel_for<_Se1, _It1> //
                && _Sized_or_unreachable_sentinel_for<_Se2, _It2> //
                && same_as<_Pj1, identity> && same_as<_Pj2, identity>) {
                if (!_STD is_constant_evaluated()) {
                    size_t _Num1;
                    if constexpr (_Is_sized1) {
                        _Num1 = static_cast<size_t>(_Last1 - _First1);
                    }
                    else {
                        _Num1 = SIZE_MAX;
                    }

                    size_t _Num2;
                    if constexpr (_Is_sized2) {
                        _Num2 = static_cast<size_t>(_Last2 - _First2);
                    }
                    else {
                        _Num2 = SIZE_MAX;
                    }

                    const int _Ans = _Memcmp_count(_First1, _First2, (_STD min)(_Num1, _Num2));
                    return _Memcmp_classification_pred{}(_Ans, 0) || (_Ans == 0 && _Num1 < _Num2);
                }
            }

            for (;; ++_First1, (void) ++_First2) {
                if (_First2 == _Last2) {
                    return false;
                }
                else if (_First1 == _Last1) {
                    return true;
                }
                else if (_STD invoke(_Pred, _STD invoke(_Proj1, *_First1), _STD invoke(_Proj2, *_First2))) {
                    return true;
                }
                else if (_STD invoke(_Pred, _STD invoke(_Proj2, *_First2), _STD invoke(_Proj1, *_First1))) {
                    return false;
                }
            }
        }
    };

    _EXPORT_STD inline constexpr _Lexicographical_compare_fn lexicographical_compare{
        _Not_quite_object::_Construct_tag{} };
} // namespace ranges
#endif // __cpp_lib_concepts
#endif // _HAS_CXX17

_STD_END
#pragma pop_macro("new")
_STL_RESTORE_CLANG_WARNINGS
#pragma warning(pop)
#pragma pack(pop)
#endif // _STL_COMPILER_PREPROCESSOR
#endif // _ALGORITHM_
