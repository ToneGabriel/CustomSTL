// iterator standard header

// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once
#ifndef _ITERATOR_
#define _ITERATOR_
#include <yvals_core.h>
#if _STL_COMPILER_PREPROCESSOR
#include <iosfwd>
#include <xutility>

#pragma pack(push, _CRT_PACKING)
#pragma warning(push, _STL_WARNING_LEVEL)
#pragma warning(disable : _STL_DISABLED_WARNINGS)
_STL_DISABLE_CLANG_WARNINGS
#pragma push_macro("new")
#undef new

_STD_BEGIN
_EXPORT_STD template <class _Container>
class back_insert_iterator { // wrap pushes to back of container as output iterator
public:
    using iterator_category = output_iterator_tag;
    using value_type = void;
    using pointer = void;
    using reference = void;

    using container_type = _Container;

#ifdef __cpp_lib_concepts
    using difference_type = ptrdiff_t;
#else
    using difference_type = void;
#endif // __cpp_lib_concepts

    _CONSTEXPR20 explicit back_insert_iterator(_Container& _Cont) noexcept /* strengthened */
        : container(_STD addressof(_Cont)) {}

    _CONSTEXPR20 back_insert_iterator& operator=(const typename _Container::value_type& _Val) {
        container->push_back(_Val);
        return *this;
    }

    _CONSTEXPR20 back_insert_iterator& operator=(typename _Container::value_type&& _Val) {
        container->push_back(_STD move(_Val));
        return *this;
    }

    _NODISCARD _CONSTEXPR20 back_insert_iterator& operator*() noexcept /* strengthened */ {
        return *this;
    }

    _CONSTEXPR20 back_insert_iterator& operator++() noexcept /* strengthened */ {
        return *this;
    }

    _CONSTEXPR20 back_insert_iterator operator++(int) noexcept /* strengthened */ {
        return *this;
    }

protected:
    _Container* container;
};

_EXPORT_STD template <class _Container>
_NODISCARD _CONSTEXPR20 back_insert_iterator<_Container> back_inserter(_Container& _Cont) noexcept /* strengthened */ {
    // return a back_insert_iterator
    return back_insert_iterator<_Container>(_Cont);
}

_EXPORT_STD template <class _Container>
class front_insert_iterator { // wrap pushes to front of container as output iterator
public:
    using iterator_category = output_iterator_tag;
    using value_type = void;
    using pointer = void;
    using reference = void;

    using container_type = _Container;

#ifdef __cpp_lib_concepts
    using difference_type = ptrdiff_t;
#else
    using difference_type = void;
#endif // __cpp_lib_concepts

    _CONSTEXPR20 explicit front_insert_iterator(_Container& _Cont) : container(_STD addressof(_Cont)) {}

    _CONSTEXPR20 front_insert_iterator& operator=(const typename _Container::value_type& _Val) {
        // push value into container
        container->push_front(_Val);
        return *this;
    }

    _CONSTEXPR20 front_insert_iterator& operator=(typename _Container::value_type&& _Val) { // push value into container
        container->push_front(_STD move(_Val));
        return *this;
    }

    _NODISCARD _CONSTEXPR20 front_insert_iterator& operator*() { // pretend to return designated value
        return *this;
    }

    _CONSTEXPR20 front_insert_iterator& operator++() { // pretend to preincrement
        return *this;
    }

    _CONSTEXPR20 front_insert_iterator operator++(int) { // pretend to postincrement
        return *this;
    }

protected:
    _Container* container;
};

_EXPORT_STD template <class _Container>
_NODISCARD _CONSTEXPR20 front_insert_iterator<_Container> front_inserter(_Container& _Cont) {
    return front_insert_iterator<_Container>(_Cont);
}

_EXPORT_STD template <class _Container>
class insert_iterator { // wrap inserts into container as output iterator
public:
    using iterator_category = output_iterator_tag;
    using value_type = void;
    using pointer = void;
    using reference = void;

    using container_type = _Container;
#ifdef __cpp_lib_concepts
    using difference_type = ptrdiff_t;
    using _Wrapped_iter = ranges::iterator_t<_Container>;
#else // ^^^ implementing Ranges / no Ranges vvv
    using difference_type = void;
    using _Wrapped_iter = typename _Container::iterator;
#endif // __cpp_lib_concepts

    _CONSTEXPR20 insert_iterator(_Container& _Cont, _Wrapped_iter _Where)
        : container(_STD addressof(_Cont)), iter(_STD move(_Where)) {}

    _CONSTEXPR20 insert_iterator& operator=(const typename _Container::value_type& _Val) {
        // insert into container and increment stored iterator
        iter = container->insert(iter, _Val);
        ++iter;
        return *this;
    }

    _CONSTEXPR20 insert_iterator& operator=(typename _Container::value_type&& _Val) { // push value into container
        iter = container->insert(iter, _STD move(_Val));
        ++iter;
        return *this;
    }

    _NODISCARD _CONSTEXPR20 insert_iterator& operator*() { // pretend to return designated value
        return *this;
    }

    _CONSTEXPR20 insert_iterator& operator++() { // pretend to preincrement
        return *this;
    }

    _CONSTEXPR20 insert_iterator& operator++(int) { // pretend to postincrement
        return *this;
    }

protected:
    _Container* container;
    _Wrapped_iter iter;
};

#ifndef __cpp_lib_concepts
template <class _Container>
_NODISCARD _CONSTEXPR20 insert_iterator<_Container> inserter(_Container& _Cont, typename _Container::iterator _Where) {
    return insert_iterator<_Container>(_Cont, _Where);
}
#else // ^^^ No Ranges / Ranges vvv
_EXPORT_STD template <class _Container>
_NODISCARD constexpr insert_iterator<_Container> inserter(_Container& _Cont, _RANGES iterator_t<_Container> _Where) {
    return insert_iterator<_Container>(_Cont, _Where);
}

_EXPORT_STD template <semiregular _Se>
class move_sentinel {
public:
    constexpr move_sentinel() = default;

    constexpr explicit move_sentinel(_Se _Val) noexcept(is_nothrow_move_constructible_v<_Se>) // strengthened
        : _Last(_STD move(_Val)) {}

    template <class _Se2>
        requires convertible_to<const _Se2&, _Se>
    constexpr move_sentinel(const move_sentinel<_Se2>& _Val) noexcept(
        is_nothrow_constructible_v<_Se, const _Se2&>) // strengthened
        : _Last(_Val._Get_last()) {}

    template <class _Se2>
        requires assignable_from<_Se&, const _Se2&>
    constexpr move_sentinel& operator=(const move_sentinel<_Se2>& _Val) noexcept(
        is_nothrow_assignable_v<_Se&, const _Se2&>) /* strengthened */ {
        _Last = _Val._Get_last();
        return *this;
    }

    _NODISCARD constexpr _Se base() const noexcept(is_nothrow_copy_constructible_v<_Se>) /* strengthened */ {
        return _Last;
    }

    _NODISCARD constexpr const _Se& _Get_last() const noexcept {
        return _Last;
    }

    using _Prevent_inheriting_unwrap = move_sentinel;

    // clang-format off
    _NODISCARD constexpr move_sentinel<_Unwrapped_t<const _Se&>> _Unwrapped() const&
        noexcept(noexcept(move_sentinel<_Unwrapped_t<const _Se&>>{_Last._Unwrapped()}))
        requires _RANGES _Weakly_unwrappable_sentinel<_Se> {
        // clang-format on
        return move_sentinel<_Unwrapped_t<const _Se&>>{_Last._Unwrapped()};
    }
    // clang-format off
    _NODISCARD constexpr move_sentinel<_Unwrapped_t<_Se>> _Unwrapped() &&
        noexcept(noexcept(move_sentinel<_Unwrapped_t<_Se>>{_STD move(_Last)._Unwrapped()}))
        requires _RANGES _Weakly_unwrappable_sentinel<_Se> {
        // clang-format on
        return move_sentinel<_Unwrapped_t<_Se>>{_STD move(_Last)._Unwrapped()};
    }

private:
    _Se _Last{};
};
#endif // __cpp_lib_concepts

_EXPORT_STD template <class _Ty, class _Elem = char, class _Traits = char_traits<_Elem>, class _Diff = ptrdiff_t>
class istream_iterator {
public:
    using iterator_category = input_iterator_tag;
    using value_type = _Ty;
    using difference_type = _Diff;
    using pointer = const _Ty*;
    using reference = const _Ty&;
    using char_type = _Elem;
    using traits_type = _Traits;
    using istream_type = basic_istream<_Elem, _Traits>;

    static_assert(conjunction_v<is_default_constructible<_Ty>, is_copy_constructible<_Ty>, is_copy_assignable<_Ty>>,
        "istream_iterator<T> requires T to be default constructible, copy constructible, and copy assignable. "
        "(N4950 [istream.iterator]/2)");

    constexpr istream_iterator() noexcept(is_nothrow_default_constructible_v<_Ty>) /* strengthened */ {}

#ifdef __cpp_lib_concepts
    constexpr istream_iterator(default_sentinel_t) noexcept(
        is_nothrow_default_constructible_v<_Ty>) /* strengthened */ {}
#endif // __cpp_lib_concepts

    istream_iterator(istream_type& _Istr) : _Myistr(_STD addressof(_Istr)) {
        _Getval();
    }

    _NODISCARD const _Ty& operator*() const noexcept /* strengthened */ {
        _STL_ASSERT(_Myistr, "The stored stream pointer in_stream must be non-null");
        return _Myval;
    }

    _NODISCARD const _Ty* operator->() const noexcept /* strengthened */ {
        _STL_ASSERT(_Myistr, "The stored stream pointer in_stream must be non-null");
        return _STD addressof(_Myval);
    }

    istream_iterator& operator++() {
        _Getval();
        return *this;
    }

    istream_iterator operator++(int) {
        istream_iterator _Tmp = *this;
        _Getval();
        return _Tmp;
    }

    _NODISCARD bool _Equal(const istream_iterator& _Right) const noexcept {
        return _Myistr == _Right._Myistr;
    }

#ifdef __cpp_lib_concepts
    _NODISCARD_FRIEND bool operator==(const istream_iterator& _Left, default_sentinel_t) noexcept /* strengthened */ {
        return !_Left._Myistr;
    }
#endif // __cpp_lib_concepts

private:
    void _Getval() { // get a _Ty value if possible
        _STL_ASSERT(_Myistr, "The stored stream pointer in_stream must be non-null");
        if (!(*_Myistr >> _Myval)) {
            _Myistr = nullptr;
        }
    }

    istream_type* _Myistr{ nullptr }; // pointer to input stream
    _Ty _Myval{}; // lookahead value (valid if _Myistr is not null)
};

_EXPORT_STD template <class _Ty, class _Elem, class _Traits, class _Diff>
_NODISCARD bool operator==(const istream_iterator<_Ty, _Elem, _Traits, _Diff>& _Left,
    const istream_iterator<_Ty, _Elem, _Traits, _Diff>& _Right) noexcept /* strengthened */ {
    return _Left._Equal(_Right);
}

#if !_HAS_CXX20
template <class _Ty, class _Elem, class _Traits, class _Diff>
_NODISCARD bool operator!=(const istream_iterator<_Ty, _Elem, _Traits, _Diff>& _Left,
    const istream_iterator<_Ty, _Elem, _Traits, _Diff>& _Right) noexcept /* strengthened */ {
    return !(_Left == _Right);
}
#endif // !_HAS_CXX20

_EXPORT_STD template <class _Ty, class _Elem = char, class _Traits = char_traits<_Elem>>
class ostream_iterator {
public:
    using iterator_category = output_iterator_tag;
    using value_type = void;
#ifdef __cpp_lib_concepts
    using difference_type = ptrdiff_t;
#else
    using difference_type = void;
#endif
    using pointer = void;
    using reference = void;
    using char_type = _Elem;
    using traits_type = _Traits;
    using ostream_type = basic_ostream<_Elem, _Traits>;

    ostream_iterator(ostream_type& _Ostr, const _Elem* const _Delim = nullptr) noexcept /* strengthened */
        : _Mydelim(_Delim), _Myostr(_STD addressof(_Ostr)) {}

    ostream_iterator& operator=(const _Ty& _Val) { // insert value into output stream, followed by delimiter
        *_Myostr << _Val;
        if (_Mydelim) {
            *_Myostr << _Mydelim;
        }

        return *this;
    }

    _NODISCARD ostream_iterator& operator*() noexcept /* strengthened */ {
        return *this;
    }

    ostream_iterator& operator++() noexcept /* strengthened */ {
        return *this;
    }

    ostream_iterator& operator++(int) noexcept /* strengthened */ {
        return *this;
    }

private:
    const _Elem* _Mydelim; // pointer to delimiter string (NB: not freed)
    ostream_type* _Myostr; // pointer to output stream
};

_EXPORT_STD template <class _Elem, class _Traits>
class istreambuf_iterator {
public:
    using iterator_category = input_iterator_tag;
    using value_type = _Elem;
    using difference_type = typename _Traits::off_type;
    using pointer = const _Elem*;
    using reference = _Elem;
    using char_type = _Elem;
    using traits_type = _Traits;
    using int_type = typename traits_type::int_type;
    using streambuf_type = basic_streambuf<_Elem, _Traits>;
    using istream_type = basic_istream<_Elem, _Traits>;

    constexpr istreambuf_iterator() noexcept : _Strbuf(nullptr), _Got(true), _Val() {}
#ifdef __cpp_lib_concepts
    constexpr istreambuf_iterator(default_sentinel_t) noexcept : _Strbuf(nullptr), _Got(true), _Val() {}
#endif // __cpp_lib_concepts

    istreambuf_iterator(istream_type& _Istr) noexcept : _Strbuf(_Istr.rdbuf()), _Got(!_Strbuf), _Val() {}

    istreambuf_iterator(streambuf_type* _Sb) noexcept : _Strbuf(_Sb), _Got(!_Sb), _Val() {}

private:
    class _Istreambuf_proxy {
    public:
        _NODISCARD _Elem operator*() const noexcept(is_nothrow_copy_constructible_v<_Elem>) /* strengthened */ {
            return _Keep;
        }

    private:
        friend istreambuf_iterator;
        _Istreambuf_proxy(streambuf_type* _Strbuf_, _Elem _Keep_) noexcept(
            is_nothrow_copy_constructible_v<_Elem>) // strengthened
            : _Strbuf(_Strbuf_), _Keep(_Keep_) {}

        streambuf_type* _Strbuf;
        _Elem _Keep;
    };

public:
    istreambuf_iterator(const _Istreambuf_proxy& _Px) noexcept : _Strbuf(_Px._Strbuf), _Got(!_Strbuf), _Val() {}

    _NODISCARD _Elem operator*() const {
        if (!_Got) {
            _Peek();
        }

#if _ITERATOR_DEBUG_LEVEL == 2
        _STL_VERIFY(_Strbuf, "istreambuf_iterator is not dereferenceable");
#endif // _ITERATOR_DEBUG_LEVEL == 2

        return _Val;
    }

    istreambuf_iterator& operator++() {
#if _ITERATOR_DEBUG_LEVEL == 2
        _STL_VERIFY(_Strbuf, "istreambuf_iterator is not incrementable");
#endif // _ITERATOR_DEBUG_LEVEL == 2

        _Inc();
        return *this;
    }

    _Istreambuf_proxy operator++(int) {
        if (!_Got) {
            _Peek();
        }

        _Istreambuf_proxy _Tmp{ _Strbuf, _Val };
        ++*this;
        return _Tmp;
    }

    _NODISCARD bool equal(const istreambuf_iterator& _Right) const {
        if (!_Got) {
            _Peek();
        }

        if (!_Right._Got) {
            _Right._Peek();
        }

        return (!_Strbuf && !_Right._Strbuf) || (_Strbuf && _Right._Strbuf);
    }

#ifdef __cpp_lib_concepts
    _NODISCARD_FRIEND bool operator==(const istreambuf_iterator& _Left, default_sentinel_t) {
        if (!_Left._Got) {
            _Left._Peek();
        }

        return !_Left._Strbuf;
    }
#endif // __cpp_lib_concepts

private:
    void _Inc() { // skip to next input element
        if (!_Strbuf || traits_type::eq_int_type(traits_type::eof(), _Strbuf->sbumpc())) {
            _Strbuf = nullptr;
            _Got = true;
        }
        else {
            _Got = false;
        }
    }

    _Elem _Peek() const { // peek at next input element
        int_type _Meta;
        if (!_Strbuf || traits_type::eq_int_type(traits_type::eof(), _Meta = _Strbuf->sgetc())) {
            _Strbuf = nullptr;
        }
        else {
            _Val = traits_type::to_char_type(_Meta);
        }

        _Got = true;
        return _Val;
    }

    mutable streambuf_type* _Strbuf; // the wrapped stream buffer
    mutable bool _Got; // true if _Val is valid
    mutable _Elem _Val; // next element to deliver
};

_EXPORT_STD template <class _Elem, class _Traits>
_NODISCARD bool operator==(
    const istreambuf_iterator<_Elem, _Traits>& _Left, const istreambuf_iterator<_Elem, _Traits>& _Right) {
    return _Left.equal(_Right);
}

#if !_HAS_CXX20
template <class _Elem, class _Traits>
_NODISCARD bool operator!=(
    const istreambuf_iterator<_Elem, _Traits>& _Left, const istreambuf_iterator<_Elem, _Traits>& _Right) {
    return !(_Left == _Right);
}
#endif // !_HAS_CXX20

_EXPORT_STD template <class _Elem, class _Traits>
class ostreambuf_iterator {
public:
    using iterator_category = output_iterator_tag;
    using value_type = void;
#ifdef __cpp_lib_concepts
    using difference_type = ptrdiff_t;
#else
    using difference_type = void;
#endif
    using pointer = void;
    using reference = void;
    using char_type = _Elem;
    using traits_type = _Traits;
    using streambuf_type = basic_streambuf<_Elem, _Traits>;
    using ostream_type = basic_ostream<_Elem, _Traits>;

    ostreambuf_iterator(streambuf_type* _Sb) noexcept : _Strbuf(_Sb) {}

    ostreambuf_iterator(ostream_type& _Ostr) noexcept : _Strbuf(_Ostr.rdbuf()) {}

    ostreambuf_iterator& operator=(_Elem _Right) { // store element and increment
        if (!_Strbuf || traits_type::eq_int_type(_Traits::eof(), _Strbuf->sputc(_Right))) {
            _Failed = true;
        }

        return *this;
    }

    _NODISCARD ostreambuf_iterator& operator*() noexcept /* strengthened */ {
        return *this;
    }

    ostreambuf_iterator& operator++() noexcept /* strengthened */ {
        return *this;
    }

    ostreambuf_iterator& operator++(int) noexcept /* strengthened */ {
        return *this;
    }

    _NODISCARD bool failed() const noexcept {
        return _Failed;
    }

private:
    bool _Failed = false; // true if any stores have failed
    streambuf_type* _Strbuf;
};

#ifdef __cpp_lib_concepts
enum class _Variantish_state : unsigned char { _Nothing, _Holds_first, _Holds_second };

struct _Variantish_empty_tag {
    explicit _Variantish_empty_tag() = default;
};

template <class _Ty1, class _Ty2>
class _Variantish {
public:
    constexpr explicit _Variantish(_Variantish_empty_tag) noexcept : _Contains{ _Variantish_state::_Nothing } {}

    constexpr _Variantish() noexcept(is_nothrow_default_constructible_v<_Ty1>)
        requires default_initializable<_Ty1>
    : _First{}, _Contains{ _Variantish_state::_Holds_first } {}

    template <class... _Types>
    constexpr explicit _Variantish(in_place_type_t<_Ty1>, _Types&&... _Args) noexcept(
        is_nothrow_constructible_v<_Ty1, _Types...>)
        : _First(_STD forward<_Types>(_Args)...), _Contains{ _Variantish_state::_Holds_first } {}

    template <class... _Types>
        requires _Different_from<_Ty1, _Ty2>
    constexpr explicit _Variantish(in_place_type_t<_Ty2>, _Types&&... _Args) noexcept(
        is_nothrow_constructible_v<_Ty2, _Types...>)
        : _Second(_STD forward<_Types>(_Args)...), _Contains{ _Variantish_state::_Holds_second } {}

    // clang-format off
    template <class _Uty1, class _Uty2>
        requires _Different_from<_Variantish<_Uty1, _Uty2>, _Variantish>
    constexpr _Variantish(const _Variantish<_Uty1, _Uty2>& _That) noexcept(
        is_nothrow_constructible_v<_Ty1, const _Uty1&>&& is_nothrow_constructible_v<_Ty2, const _Uty2&>)
        : _Contains{ _That._Contains } {
        // clang-format on
        switch (_That._Contains) {
        case _Variantish_state::_Holds_first:
            _STD _Construct_in_place(_First, _That._Get_first());
            break;
        case _Variantish_state::_Holds_second:
            _STD _Construct_in_place(_Second, _That._Get_second());
            break;
        case _Variantish_state::_Nothing:
            break;
        }
    }

    // clang-format off
    constexpr _Variantish(const _Variantish&) requires is_trivially_copy_constructible_v<_Ty1>
        && is_trivially_copy_constructible_v<_Ty2> = default;
    // clang-format on

    constexpr _Variantish(const _Variantish& _That) noexcept(
        is_nothrow_copy_constructible_v<_Ty1>&& is_nothrow_copy_constructible_v<_Ty2>)
        : _Contains{ _That._Contains } {
        switch (_Contains) {
        case _Variantish_state::_Holds_first:
            _STD _Construct_in_place(_First, _That._Get_first());
            break;
        case _Variantish_state::_Holds_second:
            _STD _Construct_in_place(_Second, _That._Get_second());
            break;
        case _Variantish_state::_Nothing:
            break;
        }
    }

    // clang-format off
    constexpr _Variantish(_Variantish&&) requires is_trivially_move_constructible_v<_Ty1>
        && is_trivially_move_constructible_v<_Ty2> = default;
    // clang-format on

    constexpr _Variantish(_Variantish&& _That) noexcept(
        is_nothrow_move_constructible_v<_Ty1>&& is_nothrow_move_constructible_v<_Ty2>)
        : _Contains{ _That._Contains } {
        switch (_Contains) {
        case _Variantish_state::_Holds_first:
            _STD _Construct_in_place(_First, _STD move(_That._Get_first()));
            break;
        case _Variantish_state::_Holds_second:
            _STD _Construct_in_place(_Second, _STD move(_That._Get_second()));
            break;
        case _Variantish_state::_Nothing:
            break;
        }
    }

    constexpr ~_Variantish() {
        _Raw_clear();
    }

    // clang-format off
    constexpr ~_Variantish() requires is_trivially_destructible_v<_Ty1>&& is_trivially_destructible_v<_Ty2> = default;

    constexpr _Variantish& operator=(const _Variantish&) requires is_trivially_destructible_v<_Ty1>
        && is_trivially_destructible_v<_Ty2>
        && is_trivially_copy_constructible_v<_Ty1>
        && is_trivially_copy_constructible_v<_Ty2>
        && is_trivially_copy_assignable_v<_Ty1>
        && is_trivially_copy_assignable_v<_Ty2> = default;
    // clang-format on

    constexpr _Variantish& operator=(const _Variantish& _That) noexcept(
        is_nothrow_copy_constructible_v<_Ty1>&& is_nothrow_copy_constructible_v<_Ty2>&&
        is_nothrow_copy_assignable_v<_Ty1>&& is_nothrow_copy_assignable_v<_Ty2>) {
        if (_Contains == _That._Contains) {
            switch (_Contains) {
            case _Variantish_state::_Holds_first:
                _Get_first() = _That._Get_first();
                break;
            case _Variantish_state::_Holds_second:
                _Get_second() = _That._Get_second();
                break;
            case _Variantish_state::_Nothing:
                break;
            }

            return *this;
        }

        _Clear();

        switch (_That._Contains) {
        case _Variantish_state::_Holds_first:
            _STD _Construct_in_place(_First, _That._Get_first());
            break;
        case _Variantish_state::_Holds_second:
            _STD _Construct_in_place(_Second, _That._Get_second());
            break;
        case _Variantish_state::_Nothing:
            break;
        }

        _Contains = _That._Contains;

        return *this;
    }

    // clang-format off
    constexpr _Variantish& operator=(_Variantish&&) requires is_trivially_destructible_v<_Ty1>
        && is_trivially_destructible_v<_Ty2>
        && is_trivially_move_constructible_v<_Ty1>
        && is_trivially_move_constructible_v<_Ty2>
        && is_trivially_move_assignable_v<_Ty1>
        && is_trivially_move_assignable_v<_Ty2> = default;
    // clang-format on

    constexpr _Variantish& operator=(_Variantish&& _That) noexcept(
        is_nothrow_move_constructible_v<_Ty1>&& is_nothrow_move_constructible_v<_Ty2>&&
        is_nothrow_move_assignable_v<_Ty1>&& is_nothrow_move_assignable_v<_Ty2>) {
        if (_Contains == _That._Contains) {
            switch (_Contains) {
            case _Variantish_state::_Holds_first:
                _Get_first() = _STD move(_That._Get_first());
                break;
            case _Variantish_state::_Holds_second:
                _Get_second() = _STD move(_That._Get_second());
                break;
            case _Variantish_state::_Nothing:
                break;
            }

            return *this;
        }

        _Clear();

        switch (_That._Contains) {
        case _Variantish_state::_Holds_first:
            _STD _Construct_in_place(_First, _STD move(_That._Get_first()));
            break;
        case _Variantish_state::_Holds_second:
            _STD _Construct_in_place(_Second, _STD move(_That._Get_second()));
            break;
        case _Variantish_state::_Nothing:
            break;
        }

        _Contains = _That._Contains;

        return *this;
    }

    // clang-format off
    template <class _Uty1, class _Uty2>
        requires _Different_from<_Variantish<_Uty1, _Uty2>, _Variantish>
    constexpr _Variantish& operator=(const _Variantish<_Uty1, _Uty2>& _That) noexcept(
        is_nothrow_constructible_v<_Ty1, const _Uty1&>&& is_nothrow_constructible_v<_Ty2, const _Uty2&>
        && is_nothrow_assignable_v<_Ty1&, const _Uty1&>&& is_nothrow_assignable_v<_Ty2&, const _Uty2&>) {
        // clang-format on
        if (_Contains == _That._Contains) {
            switch (_Contains) {
            case _Variantish_state::_Holds_first:
                _Get_first() = _That._Get_first();
                break;
            case _Variantish_state::_Holds_second:
                _Get_second() = _That._Get_second();
                break;
            case _Variantish_state::_Nothing:
                break;
            }

            return *this;
        }

        _Clear();

        switch (_That._Contains) {
        case _Variantish_state::_Holds_first:
            _STD _Construct_in_place(_First, _That._Get_first());
            break;
        case _Variantish_state::_Holds_second:
            _STD _Construct_in_place(_Second, _That._Get_second());
            break;
        case _Variantish_state::_Nothing:
            break;
        }

        _Contains = _That._Contains;

        return *this;
    }

    // clang-format off
    friend constexpr void swap(_Variantish& _Left, _Variantish& _Right) noexcept(
        is_nothrow_move_constructible_v<_Ty1>&& is_nothrow_move_constructible_v<_Ty2>
        && is_nothrow_swappable_v<_Ty1>&& is_nothrow_swappable_v<_Ty2>)
        requires (!_Is_trivially_swappable_v<_Ty1> || !_Is_trivially_swappable_v<_Ty2>) {
        // clang-format on
        if (_Left._Contains == _Right._Contains) {
            switch (_Left._Contains) {
            case _Variantish_state::_Holds_first:
                _RANGES swap(_Left._Get_first(), _Right._Get_first());
                break;
            case _Variantish_state::_Holds_second:
                _RANGES swap(_Left._Get_second(), _Right._Get_second());
                break;
            case _Variantish_state::_Nothing:
                break;
            }

            return;
        }

        auto _Tmp = _STD move(_Left);
        _Left = _STD move(_Right);
        _Right = _STD move(_Tmp);
    }

    constexpr void _Raw_clear() noexcept {
        switch (_Contains) {
        case _Variantish_state::_Holds_first:
            _First.~_Ty1();
            break;
        case _Variantish_state::_Holds_second:
            _Second.~_Ty2();
            break;
        case _Variantish_state::_Nothing:
            break;
        }
    }

    _NODISCARD constexpr _Ty1& _Get_first() noexcept {
        return _First;
    }
    _NODISCARD constexpr const _Ty1& _Get_first() const noexcept {
        return _First;
    }

    _NODISCARD constexpr _Ty2& _Get_second() noexcept {
        return _Second;
    }
    _NODISCARD constexpr const _Ty2& _Get_second() const noexcept {
        return _Second;
    }

    constexpr void _Clear() noexcept {
        _Raw_clear();
        _Contains = _Variantish_state::_Nothing;
    }

    template <class... _Types>
    constexpr void _Emplace_first(_Types&&... _Args) noexcept(is_nothrow_constructible_v<_Ty1, _Types...>) {
        _Clear();

        _STD _Construct_in_place(_First, _STD forward<_Types>(_Args)...);
        _Contains = _Variantish_state::_Holds_first;
    }

    template <class... _Types>
    constexpr void _Emplace_second(_Types&&... _Args) noexcept(is_nothrow_constructible_v<_Ty2, _Types...>) {
        _Clear();

        _STD _Construct_in_place(_Second, _STD forward<_Types>(_Args)...);
        _Contains = _Variantish_state::_Holds_second;
    }

    union {
        remove_cv_t<_Ty1> _First;
        remove_cv_t<_Ty2> _Second;
    };

    _Variantish_state _Contains;
};

// clang-format off
template <class _Iter>
concept _Use_postfix_proxy = !requires(_Iter & __it) { { *__it++ } -> _Can_reference; }
&& indirectly_readable<_Iter>
&& constructible_from<iter_value_t<_Iter>, iter_reference_t<_Iter>>
&& move_constructible<iter_value_t<_Iter>>;
// clang-format on

_EXPORT_STD template <input_or_output_iterator _Iter, sentinel_for<_Iter> _Se>
    requires (!same_as<_Iter, _Se>&& copyable<_Iter>)
class common_iterator {
private:
    struct _Proxy_base {
        iter_value_t<_Iter> _Keep;

        constexpr explicit _Proxy_base(iter_reference_t<_Iter>&& _Right) noexcept(
            is_nothrow_constructible_v<iter_value_t<_Iter>, iter_reference_t<_Iter>>) // strengthened
            : _Keep(_STD forward<iter_reference_t<_Iter>>(_Right)) {}
    };

public:
    // clang-format off
    constexpr common_iterator() requires default_initializable<_Iter> = default;
    // clang-format on

    constexpr common_iterator(_Iter _Right) noexcept(is_nothrow_move_constructible_v<_Iter>) // strengthened
        : _Val{ in_place_type<_Iter>, _STD move(_Right) } {}

    constexpr common_iterator(_Se _Right) noexcept(is_nothrow_move_constructible_v<_Se>) // strengthened
        : _Val{ in_place_type<_Se>, _STD move(_Right) } {}

    constexpr explicit common_iterator(_Variantish_empty_tag _Tag) noexcept : _Val{ _Tag } {}

    template <class _OIter, class _OSe>
        requires convertible_to<const _OIter&, _Iter>&& convertible_to<const _OSe&, _Se>
    constexpr common_iterator(const common_iterator<_OIter, _OSe>& _Right) noexcept(
        is_nothrow_constructible_v<_Iter, const _OIter&>&& is_nothrow_constructible_v<_Se, const _OSe&>) // strengthened
        : _Val{ _Right._Get_val() } {}

    // clang-format off
    template <class _OIter, class _OSe>
        requires convertible_to<const _OIter&, _Iter>&& convertible_to<const _OSe&, _Se>
    && assignable_from<_Iter&, const _OIter&>&& assignable_from<_Se&, const _OSe&>
        constexpr common_iterator& operator=(const common_iterator<_OIter, _OSe>& _Right) noexcept(
            is_nothrow_constructible_v<_Iter, const _OIter&>&& is_nothrow_constructible_v<_Se, const _OSe&>
            && is_nothrow_assignable_v<_Iter&, const _OIter&>
            && is_nothrow_assignable_v<_Se&, const _OSe&>) /* strengthened */ {
        // clang-format on
        _Val = _Right._Get_val();
        return *this;
    }

    _NODISCARD constexpr decltype(auto) operator*() {
#if _ITERATOR_DEBUG_LEVEL != 0
        _STL_VERIFY(_Val._Contains == _Variantish_state::_Holds_first,
            "common_iterator can only be dereferenced if it holds an iterator");
#endif // _ITERATOR_DEBUG_LEVEL != 0
        return *_Val._Get_first();
    }

    _NODISCARD constexpr decltype(auto) operator*() const
        requires _Dereferenceable<const _Iter>
    {
#if _ITERATOR_DEBUG_LEVEL != 0
        _STL_VERIFY(_Val._Contains == _Variantish_state::_Holds_first,
            "common_iterator can only be dereferenced if it holds an iterator");
#endif // _ITERATOR_DEBUG_LEVEL != 0
        return *_Val._Get_first();
    }

    // clang-format off
    _NODISCARD constexpr auto operator->() const
        requires indirectly_readable<const _Iter>
    && (_Has_member_arrow<const _Iter&> || is_reference_v<iter_reference_t<_Iter>>
        || constructible_from<iter_value_t<_Iter>, iter_reference_t<_Iter>>) {
        // clang-format on
#if _ITERATOR_DEBUG_LEVEL != 0
        _STL_VERIFY(_Val._Contains == _Variantish_state::_Holds_first,
            "common_iterator can only be dereferenced if it holds an iterator");
#endif // _ITERATOR_DEBUG_LEVEL != 0
        if constexpr (_Has_member_arrow<const _Iter&> || is_pointer_v<_Iter>) {
            return _Val._Get_first();
        }
        else if constexpr (is_reference_v<iter_reference_t<_Iter>>) {
            auto&& _Tmp = *_Val._Get_first();
            return _STD addressof(_Tmp);
        }
        else {
            class _Arrow_proxy : private _Proxy_base {
            public:
                friend common_iterator;

                using _Proxy_base::_Proxy_base;

                _NODISCARD constexpr const iter_value_t<_Iter>* operator->() const noexcept {
                    return _STD addressof(this->_Keep);
                }
            };

            return _Arrow_proxy{ *_Val._Get_first() };
        }
    }

    constexpr common_iterator& operator++() {
#if _ITERATOR_DEBUG_LEVEL != 0
        _STL_VERIFY(_Val._Contains == _Variantish_state::_Holds_first,
            "common_iterator can only be incremented if it holds an iterator");
#endif // _ITERATOR_DEBUG_LEVEL != 0
        ++_Val._Get_first();
        return *this;
    }

    constexpr decltype(auto) operator++(int) {
#if _ITERATOR_DEBUG_LEVEL != 0
        _STL_VERIFY(_Val._Contains == _Variantish_state::_Holds_first,
            "common_iterator can only be incremented if it holds an iterator");
#endif // _ITERATOR_DEBUG_LEVEL != 0
        if constexpr (forward_iterator<_Iter>) {
            common_iterator _Tmp = *this;
            ++_Val._Get_first();
            return _Tmp;
        }
        else if constexpr (_Use_postfix_proxy<_Iter>) {
            class _Postfix_proxy : private _Proxy_base {
            public:
                friend common_iterator;

                using _Proxy_base::_Proxy_base;

                _NODISCARD constexpr const iter_value_t<_Iter>& operator*() const noexcept {
                    return this->_Keep;
                }
            };

            _Postfix_proxy _Tmp{ *_Val._Get_first() };
            ++_Val._Get_first();
            return _Tmp;
        }
        else {
            return _Val._Get_first()++;
        }
    }

    template <class _OIter, sentinel_for<_Iter> _OSe>
        requires sentinel_for<_Se, _OIter>
    _NODISCARD_FRIEND constexpr bool operator==(
        const common_iterator& _Left, const common_iterator<_OIter, _OSe>& _Right) {
        auto& _Right_val = _Right._Get_val();
#if _ITERATOR_DEBUG_LEVEL != 0
        _STL_VERIFY(
            _Left._Val._Contains != _Variantish_state::_Nothing && _Right_val._Contains != _Variantish_state::_Nothing,
            "common_iterators can only be compared if both hold a value");
#endif // _ITERATOR_DEBUG_LEVEL != 0

        if (_Left._Val._Contains == _Variantish_state::_Holds_first) {
            if (_Right_val._Contains == _Variantish_state::_Holds_first) {
                if constexpr (equality_comparable_with<_Iter, _OIter>) {
                    return _Left._Val._Get_first() == _Right_val._Get_first();
                }
                else {
                    return true;
                }
            }
            else {
                return _Left._Val._Get_first() == _Right_val._Get_second();
            }
        }
        else {
            if (_Right_val._Contains == _Variantish_state::_Holds_first) {
                return _Left._Val._Get_second() == _Right_val._Get_first();
            }
            else {
                return true;
            }
        }
    }

    template <sized_sentinel_for<_Iter> _OIter, sized_sentinel_for<_Iter> _OSe>
        requires sized_sentinel_for<_Se, _OIter>
    _NODISCARD_FRIEND constexpr iter_difference_t<_OIter> operator-(
        const common_iterator& _Left, const common_iterator<_OIter, _OSe>& _Right) {
        auto& _Right_val = _Right._Get_val();
#if _ITERATOR_DEBUG_LEVEL != 0
        _STL_VERIFY(
            _Left._Val._Contains != _Variantish_state::_Nothing && _Right_val._Contains != _Variantish_state::_Nothing,
            "Cannot take difference of valueless common_iterators");
#endif // _ITERATOR_DEBUG_LEVEL != 0

        if (_Left._Val._Contains == _Variantish_state::_Holds_first) {
            if (_Right_val._Contains == _Variantish_state::_Holds_first) {
                return _Left._Val._Get_first() - _Right_val._Get_first();
            }
            else {
                return _Left._Val._Get_first() - _Right_val._Get_second();
            }
        }
        else {
            if (_Right_val._Contains == _Variantish_state::_Holds_first) {
                return _Left._Val._Get_second() - _Right_val._Get_first();
            }
            else {
                return 0;
            }
        }
    }

    _NODISCARD_FRIEND constexpr iter_rvalue_reference_t<_Iter> iter_move(const common_iterator& _Right) noexcept(
        noexcept(_RANGES iter_move(_Right._Val._Get_first())))
        requires input_iterator<_Iter>
    {
#if _ITERATOR_DEBUG_LEVEL != 0
        _STL_VERIFY(_Right._Val._Contains == _Variantish_state::_Holds_first,
            "can only iter_move from common_iterator if it holds an iterator");
#endif // _ITERATOR_DEBUG_LEVEL != 0
        return _RANGES iter_move(_Right._Val._Get_first());
    }

    template <indirectly_swappable<_Iter> _OIter, class _OSe>
    friend constexpr void iter_swap(const common_iterator& _Left, const common_iterator<_OIter, _OSe>& _Right) noexcept(
        noexcept(_RANGES iter_swap(_Left._Val._Get_first(), _Right._Get_val()._Get_first()))) {
        auto& _Right_val = _Right._Get_val();
#if _ITERATOR_DEBUG_LEVEL != 0
        _STL_VERIFY(_Left._Val._Contains == _Variantish_state::_Holds_first
            && _Right_val._Contains == _Variantish_state::_Holds_first,
            "can only iter_swap common_iterators if both hold iterators");
#endif // _ITERATOR_DEBUG_LEVEL != 0
        return _RANGES iter_swap(_Left._Val._Get_first(), _Right_val._Get_first());
    }

    _NODISCARD constexpr _Variantish<_Iter, _Se>& _Get_val() noexcept {
        return _Val;
    }
    _NODISCARD constexpr const _Variantish<_Iter, _Se>& _Get_val() const noexcept {
        return _Val;
    }

private:
    _Variantish<_Iter, _Se> _Val;
};

template <class _Iter, class _Se>
struct incrementable_traits<common_iterator<_Iter, _Se>> {
    using difference_type = iter_difference_t<_Iter>;
};

template <class, class>
struct _Common_iterator_pointer_type {
    using pointer = void;
};

template <class _Iter, class _Se>
    requires _Has_member_arrow<const common_iterator<_Iter, _Se>&>
struct _Common_iterator_pointer_type<_Iter, _Se> {
    using pointer = decltype(_STD declval<const common_iterator<_Iter, _Se>&>().operator->());
};

template <class _Iter>
concept _Has_forward_category = requires {
    typename _Iter_cat_t<_Iter>;
        requires derived_from<_Iter_cat_t<_Iter>, forward_iterator_tag>;
};

template <input_iterator _Iter, class _Se>
struct iterator_traits<common_iterator<_Iter, _Se>> {
    using iterator_concept = conditional_t<forward_iterator<_Iter>, forward_iterator_tag, input_iterator_tag>;
    using iterator_category = conditional_t<_Has_forward_category<_Iter>, forward_iterator_tag, input_iterator_tag>;
    using value_type = iter_value_t<_Iter>;
    using difference_type = iter_difference_t<_Iter>;
    using pointer = typename _Common_iterator_pointer_type<_Iter, _Se>::pointer;
    using reference = iter_reference_t<_Iter>;
};

template <class _Iter>
struct _Counted_iterator_value_type_base {};

template <indirectly_readable _Iter>
struct _Counted_iterator_value_type_base<_Iter> {
    using value_type = iter_value_t<_Iter>;
};

template <class _Iter>
struct _Counted_iterator_category_base : _Counted_iterator_value_type_base<_Iter> {};

template <_Has_member_iterator_category _Iter>
struct _Counted_iterator_category_base<_Iter> : _Counted_iterator_value_type_base<_Iter> {
    using iterator_category = typename _Iter::iterator_category;
};

template <class _Iter>
struct _Counted_iterator_concept_base : _Counted_iterator_category_base<_Iter> {};

template <_Has_member_iterator_concept _Iter>
struct _Counted_iterator_concept_base<_Iter> : _Counted_iterator_category_base<_Iter> {
    using iterator_concept = typename _Iter::iterator_concept;
};

_EXPORT_STD template <input_or_output_iterator _Iter>
class counted_iterator : public _Counted_iterator_concept_base<_Iter> {
public:
    using iterator_type = _Iter;
    using difference_type = iter_difference_t<_Iter>;

    // [counted.iter.const]
    // clang-format off
    constexpr counted_iterator() requires default_initializable<_Iter> = default;
    // clang-format on

    constexpr counted_iterator(_Iter _Right, const iter_difference_t<_Iter> _Diff) noexcept(
        is_nothrow_move_constructible_v<_Iter>) // strengthened
        : _Current(_STD move(_Right)), _Length(_Diff) {
#if _ITERATOR_DEBUG_LEVEL != 0
        _STL_VERIFY(_Diff >= 0, "counted_iterator requires non-negative length n");
#endif // _ITERATOR_DEBUG_LEVEL != 0
    }

    template <class _Other>
        requires convertible_to<const _Other&, _Iter>
    constexpr counted_iterator(const counted_iterator<_Other>& _Right) noexcept(
        is_nothrow_constructible_v<_Iter, const _Other&>) // strengthened
        : _Current(_Right.base()), _Length(_Right.count()) {}

    template <class _Other>
        requires assignable_from<_Iter&, const _Other&>
    constexpr counted_iterator& operator=(const counted_iterator<_Other>& _Right) noexcept(
        is_nothrow_assignable_v<_Iter&, const _Other&>) /* strengthened */ {
        _Current = _Right.base();
        _Length = _Right.count();
        return *this;
    }

    // [counted.iter.access]
    _NODISCARD constexpr const _Iter& base() const& noexcept {
        return _Current;
    }

    _NODISCARD constexpr _Iter base() && noexcept(is_nothrow_move_constructible_v<_Iter>) /* strengthened */ {
        return _STD move(_Current);
    }

    _NODISCARD constexpr iter_difference_t<_Iter> count() const noexcept {
        return _Length;
    }

    // [counted.iter.elem]
    _NODISCARD constexpr decltype(auto) operator*() noexcept(noexcept(*_Current)) /* strengthened */ {
#if _ITERATOR_DEBUG_LEVEL != 0
        _STL_VERIFY(_Length > 0, "counted_iterator dereference beyond end of range");
#endif // _ITERATOR_DEBUG_LEVEL != 0
        return *_Current;
    }

    _NODISCARD constexpr decltype(auto) operator*() const noexcept(noexcept(*_Current)) /* strengthened */
        requires _Dereferenceable<const _Iter>
    {
#if _ITERATOR_DEBUG_LEVEL != 0
        _STL_VERIFY(_Length > 0, "counted_iterator dereference beyond end of range");
#endif // _ITERATOR_DEBUG_LEVEL != 0
        return *_Current;
    }

    _NODISCARD constexpr auto operator->() const noexcept
        requires contiguous_iterator<_Iter>
    {
        return _STD to_address(_Current);
    }

    _NODISCARD constexpr decltype(auto) operator[](const iter_difference_t<_Iter> _Diff) const
        requires random_access_iterator<_Iter>
    {
#if _ITERATOR_DEBUG_LEVEL != 0
        _STL_VERIFY(_Diff < _Length, "counted_iterator index out of range");
#endif // _ITERATOR_DEBUG_LEVEL != 0
        return _Current[_Diff];
    }

    // [counted.iter.nav]
    constexpr counted_iterator& operator++() {
#if _ITERATOR_DEBUG_LEVEL != 0
        _STL_VERIFY(_Length > 0, "counted_iterator increment beyond end of range");
#endif // _ITERATOR_DEBUG_LEVEL != 0
        ++_Current;
        --_Length;
        return *this;
    }

    constexpr decltype(auto) operator++(int) {
#if _ITERATOR_DEBUG_LEVEL != 0
        _STL_VERIFY(_Length > 0, "counted_iterator increment beyond end of range");
#endif // _ITERATOR_DEBUG_LEVEL != 0
        --_Length;
        _TRY_BEGIN
            return _Current++;

        _CATCH_ALL
            ++_Length;

        _RERAISE;
        _CATCH_END
    }

    constexpr counted_iterator operator++(int)
        requires forward_iterator<_Iter>
    {
#if _ITERATOR_DEBUG_LEVEL != 0
        _STL_VERIFY(_Length > 0, "counted_iterator increment beyond end of range");
#endif // _ITERATOR_DEBUG_LEVEL != 0
        counted_iterator _Tmp = *this;
        ++_Current;
        --_Length;
        return _Tmp;
    }

    constexpr counted_iterator& operator--()
        requires bidirectional_iterator<_Iter>
    {
        --_Current;
        ++_Length;
        return *this;
    }

    constexpr counted_iterator operator--(int)
        requires bidirectional_iterator<_Iter>
    {
        counted_iterator _Tmp = *this;
        --_Current;
        ++_Length;
        return _Tmp;
    }

    _NODISCARD constexpr counted_iterator operator+(const iter_difference_t<_Iter> _Diff) const
        requires random_access_iterator<_Iter>
    {
        return counted_iterator{ _Current + _Diff, static_cast<iter_difference_t<_Iter>>(_Length - _Diff) };
    }

    _NODISCARD_FRIEND constexpr counted_iterator operator+(
        const iter_difference_t<_Iter> _Diff, const counted_iterator& _Right)
        requires random_access_iterator<_Iter>
    {
        return counted_iterator{ _Right._Current + _Diff, static_cast<iter_difference_t<_Iter>>(_Right._Length - _Diff) };
    }

    constexpr counted_iterator& operator+=(const iter_difference_t<_Iter> _Diff)
        requires random_access_iterator<_Iter>
    {
#if _ITERATOR_DEBUG_LEVEL != 0
        _STL_VERIFY(_Diff <= _Length, "counted_iterator seek beyond end of range");
#endif // _ITERATOR_DEBUG_LEVEL != 0
        _Current += _Diff;
        _Length -= _Diff;
        return *this;
    }

    _NODISCARD constexpr counted_iterator operator-(const iter_difference_t<_Iter> _Diff) const
        requires random_access_iterator<_Iter>
    {
        return counted_iterator{ _Current - _Diff, static_cast<iter_difference_t<_Iter>>(_Length + _Diff) };
    }

    template <common_with<_Iter> _Other>
    _NODISCARD_FRIEND constexpr iter_difference_t<_Other> operator-(
        const counted_iterator& _Left, const counted_iterator<_Other>& _Right) noexcept /* strengthened */ {
#if _ITERATOR_DEBUG_LEVEL != 0
        _Same_sequence(_Left, _Right);
#endif // _ITERATOR_DEBUG_LEVEL != 0
        return _Right.count() - _Left._Length;
    }

    _NODISCARD_FRIEND constexpr iter_difference_t<_Iter> operator-(
        const counted_iterator& _Left, default_sentinel_t) noexcept /* strengthened */ {
        return -_Left._Length;
    }

    _NODISCARD_FRIEND constexpr iter_difference_t<_Iter> operator-(
        default_sentinel_t, const counted_iterator& _Right) noexcept /* strengthened */ {
        return _Right._Length;
    }

    constexpr counted_iterator& operator-=(const iter_difference_t<_Iter> _Diff)
        requires random_access_iterator<_Iter>
    {
#if _ITERATOR_DEBUG_LEVEL != 0
        _STL_VERIFY(-_Diff <= _Length, "counted_iterator decrement beyond end of range");
#endif // _ITERATOR_DEBUG_LEVEL != 0
        _Current -= _Diff;
        _Length += _Diff;
        return *this;
    }

    // [counted.iter.cmp]
    template <common_with<_Iter> _Other>
    _NODISCARD_FRIEND constexpr bool operator==(
        const counted_iterator& _Left, const counted_iterator<_Other>& _Right) noexcept /* strengthened */ {
#if _ITERATOR_DEBUG_LEVEL != 0
        _Same_sequence(_Left, _Right);
#endif // _ITERATOR_DEBUG_LEVEL != 0
        return _Left._Length == _Right.count();
    }

    _NODISCARD_FRIEND constexpr bool operator==(const counted_iterator& _Left, default_sentinel_t) noexcept
        /* strengthened */ {
        return _Left._Length == 0;
    }

    template <common_with<_Iter> _Other>
    _NODISCARD_FRIEND constexpr strong_ordering operator<=>(
        const counted_iterator& _Left, const counted_iterator<_Other>& _Right) noexcept /* strengthened */ {
#if _ITERATOR_DEBUG_LEVEL != 0
        _Same_sequence(_Left, _Right);
#endif // _ITERATOR_DEBUG_LEVEL != 0
        return _Right.count() <=> _Left._Length;
    }

    // [counted.iter.cust]
    _NODISCARD_FRIEND constexpr iter_rvalue_reference_t<_Iter> iter_move(const counted_iterator& _Right) noexcept(
        noexcept(_RANGES iter_move(_Right._Current)))
        requires input_iterator<_Iter>
    {
        return _RANGES iter_move(_Right._Current);
    }

    template <indirectly_swappable<_Iter> _Other>
    friend constexpr void iter_swap(const counted_iterator& _Left, const counted_iterator<_Other>& _Right) noexcept(
        noexcept(_RANGES iter_swap(_Left._Current, _Right.base()))) {
        _RANGES iter_swap(_Left._Current, _Right.base());
    }

#if _ITERATOR_DEBUG_LEVEL != 0
    template <common_with<_Iter> _Other>
    friend constexpr void _Same_sequence(
        const counted_iterator& _Left, const counted_iterator<_Other>& _Right) noexcept {
#ifdef _M_CEE // TRANSITION, VSO-1665606
        (void) _Left;
        (void)_Right;
#else // ^^^ workaround / no workaround vvv

        // Per N4950 [counted.iterator]/3, two counted_iterators x and y refer to elements of the same sequence iff
        // for some integer n, next(x.base(), x.count() + n) and next(y.base(), y.count() + n)
        // "refer to the same (possibly past-the-end) element".
        // Iterator equality is a fair proxy for the vaguely-defined "refer to the same element".
        if constexpr (forward_iterator<_Iter> && forward_iterator<_Other>) {
            using _CIter = common_type_t<_Iter, _Other>;
            using _CDiff = common_type_t<iter_difference_t<_Iter>, iter_difference_t<_Other>>;

            const _CDiff _Diff = static_cast<_CDiff>(_Left._Length) - static_cast<_CDiff>(_Right.count());
            if (_Diff < 0) {
                _STL_VERIFY(
                    static_cast<_CIter>(_Left._Current) == _RANGES next(static_cast<_CIter>(_Right.base()), -_Diff),
                    "counted_iterators are from different ranges");
            }
            else {
                _STL_VERIFY(
                    _RANGES next(static_cast<_CIter>(_Left._Current), _Diff) == static_cast<_CIter>(_Right.base()),
                    "counted_iterators are from different ranges");
            }
        }
#endif // ^^^ no workaround ^^^
    }
#endif // _ITERATOR_DEBUG_LEVEL != 0

    template <common_with<_Iter> _Other>
    friend constexpr void _Verify_range(const counted_iterator& _Left, const counted_iterator<_Other>& _Right) {
        if constexpr (_Range_verifiable_v<_Iter, _Other>) {
            _Verify_range(_Left._Current, _Right.base());
        }
#if _ITERATOR_DEBUG_LEVEL != 0
        _Same_sequence(_Left, _Right);
#endif // _ITERATOR_DEBUG_LEVEL != 0
    }

    constexpr void _Verify_offset(const iter_difference_t<_Iter> _Off) const {
#if _ITERATOR_DEBUG_LEVEL != 0
        _STL_VERIFY(_Off <= _Length, "Offset larger than counted_iterator length");
#endif // _ITERATOR_DEBUG_LEVEL != 0
        if constexpr (_Offset_verifiable_v<_Iter>) {
            _Current._Verify_offset(_Off);
        }
    }

    using _Prevent_inheriting_unwrap = counted_iterator;

    _NODISCARD constexpr counted_iterator<_Unwrapped_t<const _Iter&>> _Unwrapped() const& noexcept(
        noexcept(counted_iterator<_Unwrapped_t<const _Iter&>>{_Current._Unwrapped(), _Length}))
        requires _Unwrappable_v<const _Iter&>
    {
        return counted_iterator<_Unwrapped_t<const _Iter&>>{_Current._Unwrapped(), _Length};
    }

    _NODISCARD constexpr counted_iterator<_Unwrapped_t<_Iter>> _Unwrapped() && noexcept(
        noexcept(counted_iterator<_Unwrapped_t<_Iter>>{_STD move(_Current)._Unwrapped(), _Length}))
        requires _Unwrappable_v<_Iter>
    {
        return counted_iterator<_Unwrapped_t<_Iter>>{_STD move(_Current)._Unwrapped(), _Length};
    }

    static constexpr bool _Unwrap_when_unverified = _Do_unwrap_when_unverified_v<_Iter>;

    template <class _Other>
        requires _Wrapped_seekable_v<_Iter, const _Other&>
    constexpr void _Seek_to(const counted_iterator<_Other>& _It) {
        _Current._Seek_to(_It.base());
        _Length = _It.count();
    }

    template <class _Other>
        requires _Wrapped_seekable_v<_Iter, _Other>
    constexpr void _Seek_to(counted_iterator<_Other>&& _It) {
        _Current._Seek_to(_STD move(_It).base());
        _Length = _It.count();
    }

private:
    _Iter _Current{};
    iter_difference_t<_Iter> _Length = 0;
};

template <input_iterator _Iter>
    requires (!_Is_from_primary<iterator_traits<_Iter>>)
struct iterator_traits<counted_iterator<_Iter>> : iterator_traits<_Iter> {
    using pointer = conditional_t<contiguous_iterator<_Iter>, add_pointer_t<iter_reference_t<_Iter>>, void>;
};
#endif // __cpp_lib_concepts

_STD_END

_STDEXT_BEGIN
using _STD iterator_traits;
using _STD size_t;

template <class _Ptr>
class checked_array_iterator { // wrap a pointer with checking
    static_assert(_STD is_pointer_v<_Ptr>, "checked_array_iterator requires pointers");

public:
    using iterator_category = typename iterator_traits<_Ptr>::iterator_category;
    using value_type = typename iterator_traits<_Ptr>::value_type;
    using difference_type = typename iterator_traits<_Ptr>::difference_type;
    using pointer = typename iterator_traits<_Ptr>::pointer;
    using reference = typename iterator_traits<_Ptr>::reference;

    constexpr checked_array_iterator() noexcept : _Myarray(nullptr), _Mysize(0), _Myindex(0) {}

    constexpr checked_array_iterator(const _Ptr _Array, const size_t _Size, const size_t _Index = 0) noexcept
        : _Myarray(_Array), _Mysize(_Size), _Myindex(_Index) {
        _STL_VERIFY(_Index <= _Size, "checked_array_iterator construction index out of range");
    }

    _NODISCARD constexpr _Ptr base() const noexcept {
        return _Myarray + _Myindex;
    }

    _NODISCARD constexpr reference operator*() const noexcept {
        return *operator->();
    }

    _NODISCARD constexpr pointer operator->() const noexcept {
        _STL_VERIFY(_Myarray, "cannot dereference value-initialized or null checked_array_iterator");
        _STL_VERIFY(_Myindex < _Mysize, "cannot dereference end checked_array_iterator");
        return _Myarray + _Myindex;
    }

    constexpr checked_array_iterator& operator++() noexcept {
        _STL_VERIFY(_Myarray, "cannot increment value-initialized or null checked_array_iterator");
        _STL_VERIFY(_Myindex < _Mysize, "cannot increment checked_array_iterator past end");
        ++_Myindex;
        return *this;
    }

    constexpr checked_array_iterator operator++(int) noexcept {
        checked_array_iterator _Tmp = *this;
        ++*this;
        return _Tmp;
    }

    constexpr checked_array_iterator& operator--() noexcept {
        _STL_VERIFY(_Myarray, "cannot decrement value-initialized or null checked_array_iterator");
        _STL_VERIFY(_Myindex != 0, "cannot decrement checked_array_iterator before begin");
        --_Myindex;
        return *this;
    }

    constexpr checked_array_iterator operator--(int) noexcept {
        checked_array_iterator _Tmp = *this;
        --*this;
        return _Tmp;
    }

    constexpr checked_array_iterator& operator+=(const difference_type _Off) noexcept {
        if (_Off != 0) {
            _STL_VERIFY(_Myarray, "cannot seek value-initialized or null checked_array_iterator");
        }

        if (_Off < 0) {
            _STL_VERIFY(
                _Myindex >= size_t{ 0 } - static_cast<size_t>(_Off), "cannot seek checked_array_iterator before begin");
        }

        if (_Off > 0) {
            _STL_VERIFY(
                _Mysize - _Myindex >= static_cast<size_t>(_Off), "cannot seek checked_array_iterator after end");
        }

        _Myindex += _Off;
        return *this;
    }

    _NODISCARD constexpr checked_array_iterator operator+(const difference_type _Off) const noexcept {
        checked_array_iterator _Tmp = *this;
        _Tmp += _Off;
        return _Tmp;
    }

    _NODISCARD_FRIEND constexpr checked_array_iterator operator+(
        const difference_type _Off, const checked_array_iterator<_Ptr>& _Next) noexcept {
        return _Next + _Off;
    }

    constexpr checked_array_iterator& operator-=(const difference_type _Off) noexcept {
        if (_Off != 0) {
            _STL_VERIFY(_Myarray, "cannot seek value-initialized or null checked_array_iterator");
        }

        if (_Off > 0) {
            _STL_VERIFY(_Myindex >= static_cast<size_t>(_Off), "cannot seek checked_array_iterator before begin");
        }

        if (_Off < 0) {
            _STL_VERIFY(_Mysize - _Myindex >= size_t{ 0 } - static_cast<size_t>(_Off),
                "cannot seek checked_array_iterator after end");
        }

        _Myindex -= _Off;
        return *this;
    }

    _NODISCARD constexpr checked_array_iterator operator-(const difference_type _Off) const noexcept {
        checked_array_iterator _Tmp = *this;
        _Tmp -= _Off;
        return _Tmp;
    }

    _NODISCARD constexpr difference_type operator-(const checked_array_iterator& _Right) const noexcept {
        _STL_VERIFY(_Myarray == _Right._Myarray && _Mysize == _Right._Mysize,
            "cannot subtract incompatible checked_array_iterators");
        return static_cast<difference_type>(_Myindex - _Right._Myindex);
    }

    _NODISCARD constexpr reference operator[](const difference_type _Off) const noexcept {
        return *(*this + _Off);
    }

    _NODISCARD constexpr bool operator==(const checked_array_iterator& _Right) const noexcept {
        _STL_VERIFY(_Myarray == _Right._Myarray && _Mysize == _Right._Mysize,
            "cannot compare incompatible checked_array_iterators for equality");
        return _Myindex == _Right._Myindex;
    }

#if _HAS_CXX20
    _NODISCARD constexpr _STD strong_ordering operator<=>(const checked_array_iterator& _Right) const noexcept {
        _STL_VERIFY(_Myarray == _Right._Myarray && _Mysize == _Right._Mysize,
            "cannot compare incompatible checked_array_iterators");
        return _Myindex <=> _Right._Myindex;
    }
#else // ^^^ _HAS_CXX20 / !_HAS_CXX20 vvv
    _NODISCARD constexpr bool operator!=(const checked_array_iterator& _Right) const noexcept {
        return !(*this == _Right);
    }

    _NODISCARD constexpr bool operator<(const checked_array_iterator& _Right) const noexcept {
        _STL_VERIFY(_Myarray == _Right._Myarray && _Mysize == _Right._Mysize,
            "cannot compare incompatible checked_array_iterators");
        return _Myindex < _Right._Myindex;
    }

    _NODISCARD constexpr bool operator>(const checked_array_iterator& _Right) const noexcept {
        return _Right < *this;
    }

    _NODISCARD constexpr bool operator<=(const checked_array_iterator& _Right) const noexcept {
        return !(_Right < *this);
    }

    _NODISCARD constexpr bool operator>=(const checked_array_iterator& _Right) const noexcept {
        return !(*this < _Right);
    }
#endif // !_HAS_CXX20

    friend constexpr void _Verify_range(
        const checked_array_iterator& _First, const checked_array_iterator& _Last) noexcept {
        _STL_VERIFY(_First._Myarray == _Last._Myarray && _First._Mysize == _Last._Mysize,
            "mismatching checked_array_iterators");
        _STL_VERIFY(_First._Myindex <= _Last._Myindex, "transposed checked_array_iterator range");
    }

    constexpr void _Verify_offset(const difference_type _Off) const noexcept {
        if (_Off < 0) {
            _STL_VERIFY(_Myindex >= size_t{ 0 } - static_cast<size_t>(_Off),
                "cannot seek checked_array_iterator iterator before begin");
        }

        if (_Off > 0) {
            _STL_VERIFY(_Mysize - _Myindex >= static_cast<size_t>(_Off),
                "cannot seek checked_array_iterator iterator after end");
        }
    }

    using _Prevent_inheriting_unwrap = checked_array_iterator;

    _NODISCARD constexpr _Ptr _Unwrapped() const noexcept {
        return _Myarray + _Myindex;
    }

    constexpr void _Seek_to(_Ptr _It) noexcept {
        _Myindex = static_cast<size_t>(_It - _Myarray);
    }

private:
    _Ptr _Myarray; // beginning of array
    size_t _Mysize; // size of array
    size_t _Myindex; // offset into array
};

template <class _Ptr>
_NODISCARD constexpr checked_array_iterator<_Ptr> make_checked_array_iterator(
    const _Ptr _Array, const size_t _Size, const size_t _Index = 0) {
    return checked_array_iterator<_Ptr>(_Array, _Size, _Index);
}

template <class _Ptr>
class unchecked_array_iterator { // wrap a pointer without checking, to silence warnings
    static_assert(_STD is_pointer_v<_Ptr>, "unchecked_array_iterator requires pointers");

public:
    using iterator_category = typename iterator_traits<_Ptr>::iterator_category;
    using value_type = typename iterator_traits<_Ptr>::value_type;
    using difference_type = typename iterator_traits<_Ptr>::difference_type;
    using pointer = typename iterator_traits<_Ptr>::pointer;
    using reference = typename iterator_traits<_Ptr>::reference;

    constexpr unchecked_array_iterator() noexcept : _Myptr(nullptr) {}

    constexpr explicit unchecked_array_iterator(const _Ptr _Src) noexcept : _Myptr(_Src) {}

    _NODISCARD constexpr _Ptr base() const noexcept {
        return _Myptr;
    }

    _NODISCARD constexpr reference operator*() const noexcept {
        return *_Myptr;
    }

    _NODISCARD constexpr pointer operator->() const noexcept {
        return _Myptr;
    }

    constexpr unchecked_array_iterator& operator++() noexcept {
        ++_Myptr;
        return *this;
    }

    constexpr unchecked_array_iterator operator++(int) noexcept {
        unchecked_array_iterator _Tmp = *this;
        ++_Myptr;
        return _Tmp;
    }

    constexpr unchecked_array_iterator& operator--() noexcept {
        --_Myptr;
        return *this;
    }

    constexpr unchecked_array_iterator operator--(int) noexcept {
        unchecked_array_iterator _Tmp = *this;
        --_Myptr;
        return _Tmp;
    }

    constexpr unchecked_array_iterator& operator+=(const difference_type _Off) noexcept {
        _Myptr += _Off;
        return *this;
    }

    _NODISCARD constexpr unchecked_array_iterator operator+(const difference_type _Off) const noexcept {
        unchecked_array_iterator _Tmp = *this;
        _Tmp += _Off;
        return _Tmp;
    }

    _NODISCARD_FRIEND constexpr unchecked_array_iterator operator+(
        const difference_type _Off, const unchecked_array_iterator& _Next) noexcept {
        return _Next + _Off;
    }

    constexpr unchecked_array_iterator& operator-=(const difference_type _Off) noexcept {
        return *this += -_Off;
    }

    _NODISCARD constexpr unchecked_array_iterator operator-(const difference_type _Off) const noexcept {
        unchecked_array_iterator _Tmp = *this;
        _Tmp -= _Off;
        return _Tmp;
    }

    _NODISCARD constexpr difference_type operator-(const unchecked_array_iterator& _Right) const noexcept {
        return _Myptr - _Right._Myptr;
    }

    _NODISCARD constexpr reference operator[](const difference_type _Off) const noexcept {
        return *(*this + _Off);
    }

    _NODISCARD constexpr bool operator==(const unchecked_array_iterator& _Right) const noexcept {
        return _Myptr == _Right._Myptr;
    }

#if _HAS_CXX20
    _NODISCARD constexpr _STD strong_ordering operator<=>(const unchecked_array_iterator& _Right) const noexcept {
        return _Myptr <=> _Right._Myptr;
    }
#else // ^^^ _HAS_CXX20 / !_HAS_CXX20 vvv
    _NODISCARD constexpr bool operator!=(const unchecked_array_iterator& _Right) const noexcept {
        return !(*this == _Right);
    }

    _NODISCARD constexpr bool operator<(const unchecked_array_iterator& _Right) const noexcept {
        return _Myptr < _Right._Myptr;
    }

    _NODISCARD constexpr bool operator>(const unchecked_array_iterator& _Right) const noexcept {
        return _Right < *this;
    }

    _NODISCARD constexpr bool operator<=(const unchecked_array_iterator& _Right) const noexcept {
        return !(_Right < *this);
    }

    _NODISCARD constexpr bool operator>=(const unchecked_array_iterator& _Right) const noexcept {
        return !(*this < _Right);
    }
#endif // !_HAS_CXX20

#if _ITERATOR_DEBUG_LEVEL != 0
    friend constexpr void _Verify_range(
        const unchecked_array_iterator _First, const unchecked_array_iterator _Last) noexcept {
        _STD _Verify_range(_First._Myptr, _Last._Myptr);
    }
#endif // _ITERATOR_DEBUG_LEVEL != 0

    using _Prevent_inheriting_unwrap = unchecked_array_iterator;

    static constexpr bool _Unwrap_when_unverified = true;

    _NODISCARD constexpr _Ptr _Unwrapped() const noexcept {
        return _Myptr;
    }

    constexpr void _Seek_to(_Ptr _It) noexcept {
        _Myptr = _It;
    }

private:
    _Ptr _Myptr; // underlying pointer
};

template <class _Ptr>
_NODISCARD unchecked_array_iterator<_Ptr> make_unchecked_array_iterator(const _Ptr _It) noexcept {
    return unchecked_array_iterator<_Ptr>(_It);
}
_STDEXT_END

#pragma pop_macro("new")
_STL_RESTORE_CLANG_WARNINGS
#pragma warning(pop)
#pragma pack(pop)
#endif // _STL_COMPILER_PREPROCESSOR
#endif // _ITERATOR_
