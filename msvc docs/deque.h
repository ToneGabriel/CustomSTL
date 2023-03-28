// deque standard header

// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once
#ifndef _DEQUE_
#define _DEQUE_
#include <yvals_core.h>
#if _STL_COMPILER_PREPROCESSOR
#include <xmemory>

#if _HAS_CXX17
#include <xpolymorphic_allocator.h>
#endif // _HAS_CXX17

#pragma pack(push, _CRT_PACKING)
#pragma warning(push, _STL_WARNING_LEVEL)
#pragma warning(disable : _STL_DISABLED_WARNINGS)
_STL_DISABLE_CLANG_WARNINGS
#pragma push_macro("new")
#undef new

_STD_BEGIN
template <class _Mydeque>
class _Deque_unchecked_const_iterator {
private:
    using _Size_type = typename _Mydeque::size_type;

    static constexpr int _Block_size = _Mydeque::_Block_size;

public:
    using iterator_category = random_access_iterator_tag;

    using value_type      = typename _Mydeque::value_type;
    using difference_type = typename _Mydeque::difference_type;
    using pointer         = typename _Mydeque::const_pointer;
    using reference       = const value_type&;

    _Deque_unchecked_const_iterator() noexcept : _Mycont(), _Myoff(0) {}

    _Deque_unchecked_const_iterator(_Size_type _Off, const _Container_base12* _Pdeque) noexcept
        : _Mycont(static_cast<const _Mydeque*>(_Pdeque)), _Myoff(_Off) {}

    _NODISCARD reference operator*() const noexcept {
        _Size_type _Block = _Mycont->_Getblock(_Myoff);
        _Size_type _Off   = _Myoff % _Block_size;
        return _Mycont->_Map[_Block][_Off];
    }

    _NODISCARD pointer operator->() const noexcept {
        return pointer_traits<pointer>::pointer_to(**this);
    }

    _Deque_unchecked_const_iterator& operator++() noexcept {
        ++_Myoff;
        return *this;
    }

    _Deque_unchecked_const_iterator operator++(int) noexcept {
        _Deque_unchecked_const_iterator _Tmp = *this;
        ++_Myoff;
        return _Tmp;
    }

    _Deque_unchecked_const_iterator& operator--() noexcept {
        --_Myoff;
        return *this;
    }

    _Deque_unchecked_const_iterator operator--(int) noexcept {
        _Deque_unchecked_const_iterator _Tmp = *this;
        --_Myoff;
        return _Tmp;
    }

    _Deque_unchecked_const_iterator& operator+=(const difference_type _Off) noexcept {
        _Myoff += _Off;
        return *this;
    }

    _NODISCARD _Deque_unchecked_const_iterator operator+(const difference_type _Off) const noexcept {
        _Deque_unchecked_const_iterator _Tmp = *this;
        _Tmp += _Off;
        return _Tmp;
    }

    _NODISCARD_FRIEND _Deque_unchecked_const_iterator operator+(
        const difference_type _Off, _Deque_unchecked_const_iterator _Next) noexcept {
        _Next += _Off;
        return _Next;
    }

    _Deque_unchecked_const_iterator& operator-=(const difference_type _Off) noexcept {
        _Myoff -= _Off;
        return *this;
    }

    _NODISCARD _Deque_unchecked_const_iterator operator-(const difference_type _Off) const noexcept {
        _Deque_unchecked_const_iterator _Tmp = *this;
        _Tmp -= _Off;
        return _Tmp;
    }

    _NODISCARD difference_type operator-(const _Deque_unchecked_const_iterator& _Right) const noexcept {
        return static_cast<difference_type>(_Myoff - _Right._Myoff);
    }

    _NODISCARD reference operator[](const difference_type _Off) const noexcept {
        return *(*this + _Off);
    }

    _NODISCARD bool operator==(const _Deque_unchecked_const_iterator& _Right) const noexcept {
        return _Myoff == _Right._Myoff;
    }

#if _HAS_CXX20
    _NODISCARD strong_ordering operator<=>(const _Deque_unchecked_const_iterator& _Right) const noexcept {
        return _Myoff <=> _Right._Myoff;
    }
#else // ^^^ _HAS_CXX20 ^^^ / vvv !_HAS_CXX20 vvv
    _NODISCARD bool operator!=(const _Deque_unchecked_const_iterator& _Right) const noexcept {
        return !(*this == _Right);
    }

    _NODISCARD bool operator<(const _Deque_unchecked_const_iterator& _Right) const noexcept {
        return _Myoff < _Right._Myoff;
    }

    _NODISCARD bool operator>(const _Deque_unchecked_const_iterator& _Right) const noexcept {
        return _Right < *this;
    }

    _NODISCARD bool operator<=(const _Deque_unchecked_const_iterator& _Right) const noexcept {
        return !(_Right < *this);
    }

    _NODISCARD bool operator>=(const _Deque_unchecked_const_iterator& _Right) const noexcept {
        return !(*this < _Right);
    }
#endif // !_HAS_CXX20

    const _Container_base12* _Getcont() const noexcept { // get container pointer
        return _Mycont;
    }

    const _Mydeque* _Mycont;
    _Size_type _Myoff; // offset of element in deque
};

template <class _Mydeque>
class _Deque_unchecked_iterator : public _Deque_unchecked_const_iterator<_Mydeque> {
private:
    using _Size_type = typename _Mydeque::size_type;
    using _Mybase    = _Deque_unchecked_const_iterator<_Mydeque>;

public:
    using iterator_category = random_access_iterator_tag;

    using value_type      = typename _Mydeque::value_type;
    using difference_type = typename _Mydeque::difference_type;
    using pointer         = typename _Mydeque::pointer;
    using reference       = value_type&;

    using _Mybase::_Mybase;

    _NODISCARD reference operator*() const noexcept {
        return const_cast<reference>(_Mybase::operator*());
    }

    _NODISCARD pointer operator->() const noexcept {
        return pointer_traits<pointer>::pointer_to(**this);
    }

    _Deque_unchecked_iterator& operator++() noexcept {
        _Mybase::operator++();
        return *this;
    }

    _Deque_unchecked_iterator operator++(int) noexcept {
        _Deque_unchecked_iterator _Tmp = *this;
        _Mybase::operator++();
        return _Tmp;
    }

    _Deque_unchecked_iterator& operator--() noexcept {
        _Mybase::operator--();
        return *this;
    }

    _Deque_unchecked_iterator operator--(int) noexcept {
        _Deque_unchecked_iterator _Tmp = *this;
        _Mybase::operator--();
        return _Tmp;
    }

    _Deque_unchecked_iterator& operator+=(const difference_type _Off) noexcept {
        _Mybase::operator+=(_Off);
        return *this;
    }

    _NODISCARD _Deque_unchecked_iterator operator+(const difference_type _Off) const noexcept {
        _Deque_unchecked_iterator _Tmp = *this;
        _Tmp += _Off;
        return _Tmp;
    }

    _NODISCARD_FRIEND _Deque_unchecked_iterator operator+(
        const difference_type _Off, _Deque_unchecked_iterator _Next) noexcept {
        _Next += _Off;
        return _Next;
    }

    _Deque_unchecked_iterator& operator-=(const difference_type _Off) noexcept {
        _Mybase::operator-=(_Off);
        return *this;
    }

    _NODISCARD _Deque_unchecked_iterator operator-(const difference_type _Off) const noexcept {
        _Deque_unchecked_iterator _Tmp = *this;
        _Tmp -= _Off;
        return _Tmp;
    }

    _NODISCARD difference_type operator-(const _Mybase& _Right) const noexcept {
        return _Mybase::operator-(_Right);
    }

    _NODISCARD reference operator[](const difference_type _Off) const noexcept {
        return const_cast<reference>(_Mybase::operator[](_Off));
    }
};

template <class _Mydeque>
class _Deque_const_iterator : public _Iterator_base12 {
private:
    using _Size_type = typename _Mydeque::size_type;

    static constexpr int _Block_size = _Mydeque::_Block_size;

public:
    using iterator_category = random_access_iterator_tag;

    using value_type      = typename _Mydeque::value_type;
    using difference_type = typename _Mydeque::difference_type;
    using pointer         = typename _Mydeque::const_pointer;
    using reference       = const value_type&;

    using _Mydeque_t = _Mydeque; // helper for expression evaluator
    enum { _EEN_DS = _Block_size }; // helper for expression evaluator
    _Deque_const_iterator() noexcept : _Myoff(0) {
        _Setcont(nullptr);
    }

    _Deque_const_iterator(_Size_type _Off, const _Container_base12* _Pdeque) noexcept : _Myoff(_Off) {
        _Setcont(static_cast<const _Mydeque*>(_Pdeque));
    }

    _NODISCARD reference operator*() const noexcept {
        const auto _Mycont = static_cast<const _Mydeque*>(this->_Getcont());
#if _ITERATOR_DEBUG_LEVEL != 0
        _STL_VERIFY(_Mycont, "cannot dereference value-initialized deque iterator");
        _STL_VERIFY(_Mycont->_Myoff <= this->_Myoff && this->_Myoff < _Mycont->_Myoff + _Mycont->_Mysize,
            "cannot deference out of range deque iterator");
#endif // _ITERATOR_DEBUG_LEVEL != 0

        _Size_type _Block = _Mycont->_Getblock(_Myoff);
        _Size_type _Off   = _Myoff % _Block_size;
        return _Mycont->_Map[_Block][_Off];
    }

    _NODISCARD pointer operator->() const noexcept {
        return pointer_traits<pointer>::pointer_to(**this);
    }

    _Deque_const_iterator& operator++() noexcept {
#if _ITERATOR_DEBUG_LEVEL != 0
        const auto _Mycont = static_cast<const _Mydeque*>(this->_Getcont());
        _STL_VERIFY(_Mycont, "cannot increment value-initialized deque iterator");
        _STL_VERIFY(this->_Myoff < _Mycont->_Myoff + _Mycont->_Mysize, "cannot increment deque iterator past end");
#endif // _ITERATOR_DEBUG_LEVEL != 0

        ++_Myoff;
        return *this;
    }

    _Deque_const_iterator operator++(int) noexcept {
        _Deque_const_iterator _Tmp = *this;
        ++*this;
        return _Tmp;
    }

    _Deque_const_iterator& operator--() noexcept {
#if _ITERATOR_DEBUG_LEVEL != 0
        const auto _Mycont = static_cast<const _Mydeque*>(this->_Getcont());
        _STL_VERIFY(_Mycont, "cannot decrement value-initialized deque iterator");
        _STL_VERIFY(_Mycont->_Myoff < this->_Myoff, "cannot decrement deque iterator before begin");
#endif // _ITERATOR_DEBUG_LEVEL != 0

        --_Myoff;
        return *this;
    }

    _Deque_const_iterator operator--(int) noexcept {
        _Deque_const_iterator _Tmp = *this;
        --*this;
        return _Tmp;
    }

    _Deque_const_iterator& operator+=(const difference_type _Off) noexcept {
#if _ITERATOR_DEBUG_LEVEL != 0
        if (_Off != 0) {
            const auto _Mycont = static_cast<const _Mydeque*>(this->_Getcont());
            _STL_VERIFY(_Mycont, "cannot seek value-initialized deque iterator");
            _STL_VERIFY(
                _Mycont->_Myoff <= this->_Myoff + _Off && this->_Myoff + _Off <= _Mycont->_Myoff + _Mycont->_Mysize,
                "cannot seek deque iterator out of range");
        }
#endif // _ITERATOR_DEBUG_LEVEL != 0

        _Myoff += _Off;
        return *this;
    }

    _NODISCARD _Deque_const_iterator operator+(const difference_type _Off) const noexcept {
        _Deque_const_iterator _Tmp = *this;
        _Tmp += _Off;
        return _Tmp;
    }

    _NODISCARD_FRIEND _Deque_const_iterator operator+(
        const difference_type _Off, _Deque_const_iterator _Next) noexcept {
        _Next += _Off;
        return _Next;
    }

    _Deque_const_iterator& operator-=(const difference_type _Off) noexcept {
        return *this += -_Off;
    }

    _NODISCARD _Deque_const_iterator operator-(const difference_type _Off) const noexcept {
        _Deque_const_iterator _Tmp = *this;
        _Tmp -= _Off;
        return _Tmp;
    }

    _NODISCARD difference_type operator-(const _Deque_const_iterator& _Right) const noexcept {
        _Compat(_Right);
        return static_cast<difference_type>(this->_Myoff - _Right._Myoff);
    }

    _NODISCARD reference operator[](const difference_type _Off) const noexcept {
        return *(*this + _Off);
    }

    _NODISCARD bool operator==(const _Deque_const_iterator& _Right) const noexcept {
        _Compat(_Right);
        return this->_Myoff == _Right._Myoff;
    }

#if _HAS_CXX20
    _NODISCARD strong_ordering operator<=>(const _Deque_const_iterator& _Right) const noexcept {
        _Compat(_Right);
        return this->_Myoff <=> _Right._Myoff;
    }
#else // ^^^ _HAS_CXX20 ^^^ / vvv !_HAS_CXX20 vvv
    _NODISCARD bool operator!=(const _Deque_const_iterator& _Right) const noexcept {
        return !(*this == _Right);
    }

    _NODISCARD bool operator<(const _Deque_const_iterator& _Right) const noexcept {
        _Compat(_Right);
        return this->_Myoff < _Right._Myoff;
    }

    _NODISCARD bool operator>(const _Deque_const_iterator& _Right) const noexcept {
        return _Right < *this;
    }

    _NODISCARD bool operator<=(const _Deque_const_iterator& _Right) const noexcept {
        return !(_Right < *this);
    }

    _NODISCARD bool operator>=(const _Deque_const_iterator& _Right) const noexcept {
        return !(*this < _Right);
    }
#endif // !_HAS_CXX20

    void _Compat(const _Deque_const_iterator& _Right) const noexcept { // test for compatible iterator pair
#if _ITERATOR_DEBUG_LEVEL == 0
        (void) _Right;
#else // _ITERATOR_DEBUG_LEVEL == 0
        _STL_VERIFY(this->_Getcont() == _Right._Getcont(), "deque iterators incompatible");
#endif // _ITERATOR_DEBUG_LEVEL == 0
    }

    void _Setcont(const _Mydeque* _Pdeque) noexcept { // set container pointer
        this->_Adopt(_Pdeque);
    }

    using _Prevent_inheriting_unwrap = _Deque_const_iterator;

    _NODISCARD _Deque_unchecked_const_iterator<_Mydeque> _Unwrapped() const noexcept {
        return {this->_Myoff, this->_Getcont()};
    }

    void _Verify_offset(const difference_type _Off) const noexcept {
#if _ITERATOR_DEBUG_LEVEL == 0
        (void) _Off;
#else // ^^^ _ITERATOR_DEBUG_LEVEL == 0 ^^^ // vvv _ITERATOR_DEBUG_LEVEL != 0 vvv
        if (_Off != 0) {
            const auto _Mycont = static_cast<const _Mydeque*>(this->_Getcont());
            _STL_VERIFY(_Mycont, "cannot use value-initialized deque iterator");
            _STL_VERIFY(
                _Mycont->_Myoff <= this->_Myoff + _Off && this->_Myoff + _Off <= _Mycont->_Myoff + _Mycont->_Mysize,
                "cannot seek deque iterator out of range");
        }
#endif // _ITERATOR_DEBUG_LEVEL == 0
    }

#if _ITERATOR_DEBUG_LEVEL != 0
    friend void _Verify_range(const _Deque_const_iterator& _First, const _Deque_const_iterator& _Last) noexcept {
        // note _Compat check inside operator<=
        _STL_VERIFY(_First <= _Last, "deque iterators transposed");
    }
#endif // _ITERATOR_DEBUG_LEVEL != 0

    void _Seek_to(const _Deque_unchecked_const_iterator<_Mydeque>& _UIt) noexcept {
        _Myoff = _UIt._Myoff;
    }

    _Size_type _Myoff; // offset of element in deque
};

template <class _Mydeque>
class _Deque_iterator : public _Deque_const_iterator<_Mydeque> {
private:
    using _Size_type = typename _Mydeque::size_type;
    using _Mybase    = _Deque_const_iterator<_Mydeque>;

public:
    using _Deque_unchecked_type = _Deque_unchecked_iterator<_Mydeque>;
    using iterator_category     = random_access_iterator_tag;

    using value_type      = typename _Mydeque::value_type;
    using difference_type = typename _Mydeque::difference_type;
    using pointer         = typename _Mydeque::pointer;
    using reference       = value_type&;

    using _Mybase::_Mybase;

    _NODISCARD reference operator*() const noexcept {
        return const_cast<reference>(_Mybase::operator*());
    }

    _NODISCARD pointer operator->() const noexcept {
        return pointer_traits<pointer>::pointer_to(**this);
    }

    _Deque_iterator& operator++() noexcept {
        _Mybase::operator++();
        return *this;
    }

    _Deque_iterator operator++(int) noexcept {
        _Deque_iterator _Tmp = *this;
        _Mybase::operator++();
        return _Tmp;
    }

    _Deque_iterator& operator--() noexcept {
        _Mybase::operator--();
        return *this;
    }

    _Deque_iterator operator--(int) noexcept {
        _Deque_iterator _Tmp = *this;
        _Mybase::operator--();
        return _Tmp;
    }

    _Deque_iterator& operator+=(const difference_type _Off) noexcept {
        _Mybase::operator+=(_Off);
        return *this;
    }

    _NODISCARD _Deque_iterator operator+(const difference_type _Off) const noexcept {
        _Deque_iterator _Tmp = *this;
        _Tmp += _Off;
        return _Tmp;
    }

    _NODISCARD_FRIEND _Deque_iterator operator+(const difference_type _Off, _Deque_iterator _Next) noexcept {
        _Next += _Off;
        return _Next;
    }

    _Deque_iterator& operator-=(const difference_type _Off) noexcept {
        _Mybase::operator-=(_Off);
        return *this;
    }

    using _Mybase::operator-;

    _NODISCARD _Deque_iterator operator-(const difference_type _Off) const noexcept {
        _Deque_iterator _Tmp = *this;
        _Tmp -= _Off;
        return _Tmp;
    }

    _NODISCARD reference operator[](const difference_type _Off) const noexcept {
        return const_cast<reference>(_Mybase::operator[](_Off));
    }

    using _Prevent_inheriting_unwrap = _Deque_iterator;

    _NODISCARD _Deque_unchecked_iterator<_Mydeque> _Unwrapped() const noexcept {
        return {this->_Myoff, this->_Getcont()};
    }
};

template <class _Value_type, class _Size_type, class _Difference_type, class _Pointer, class _Const_pointer,
    class _Reference, class _Const_reference, class _Mapptr_type>
struct _Deque_iter_types {
    using value_type      = _Value_type;
    using size_type       = _Size_type;
    using difference_type = _Difference_type;
    using pointer         = _Pointer;
    using const_pointer   = _Const_pointer;
    using _Mapptr         = _Mapptr_type;
};

template <class _Ty>
struct _Deque_simple_types : _Simple_types<_Ty> {
    using _Mapptr = _Ty**;
};

template <class _Val_types>
class _Deque_val : public _Container_base12 {
public:
    using value_type      = typename _Val_types::value_type;
    using size_type       = typename _Val_types::size_type;
    using difference_type = typename _Val_types::difference_type;
    using pointer         = typename _Val_types::pointer;
    using const_pointer   = typename _Val_types::const_pointer;
    using reference       = value_type&;
    using const_reference = const value_type&;
    using _Mapptr         = typename _Val_types::_Mapptr;

private:
    static constexpr size_t _Bytes = sizeof(value_type);

public:
    static constexpr int _Block_size = _Bytes <= 1 ? 16
                                     : _Bytes <= 2 ? 8
                                     : _Bytes <= 4 ? 4
                                     : _Bytes <= 8 ? 2
                                                   : 1; // elements per block (a power of 2)

    _Deque_val() noexcept : _Map(), _Mapsize(0), _Myoff(0), _Mysize(0) {}

    size_type _Getblock(size_type _Off) const noexcept {
        // NB: _Mapsize and _Block_size are guaranteed to be powers of 2
        return (_Off / _Block_size) & (_Mapsize - 1);
    }

    _Mapptr _Map; // pointer to array of pointers to blocks
    size_type _Mapsize; // size of map array, zero or 2^N
    size_type _Myoff; // offset of initial element
    size_type _Mysize; // current length of sequence
};

_EXPORT_STD template <class _Ty, class _Alloc = allocator<_Ty>>
class deque {
private:
    friend _Tidy_guard<deque>;
    static_assert(!_ENFORCE_MATCHING_ALLOCATORS || is_same_v<_Ty, typename _Alloc::value_type>,
        _MISMATCHED_ALLOCATOR_MESSAGE("deque<T, Allocator>", "T"));

    using _Alty           = _Rebind_alloc_t<_Alloc, _Ty>;
    using _Alty_traits    = allocator_traits<_Alty>;
    using _Alpty          = _Rebind_alloc_t<_Alloc, typename _Alty_traits::pointer>;
    using _Alpty_traits   = allocator_traits<_Alpty>;
    using _Mapptr         = typename _Alpty_traits::pointer;
    using _Alproxy_ty     = _Rebind_alloc_t<_Alty, _Container_proxy>;
    using _Alproxy_traits = allocator_traits<_Alproxy_ty>;

    using _Scary_val = _Deque_val<conditional_t<_Is_simple_alloc_v<_Alty>, _Deque_simple_types<_Ty>,
        _Deque_iter_types<_Ty, typename _Alty_traits::size_type, typename _Alty_traits::difference_type,
            typename _Alty_traits::pointer, typename _Alty_traits::const_pointer, _Ty&, const _Ty&, _Mapptr>>>;

    static constexpr int _Minimum_map_size = 8;
    static constexpr int _Block_size       = _Scary_val::_Block_size;

public:
    using allocator_type  = _Alloc;
    using value_type      = _Ty;
    using size_type       = typename _Alty_traits::size_type;
    using difference_type = typename _Alty_traits::difference_type;
    using pointer         = typename _Alty_traits::pointer;
    using const_pointer   = typename _Alty_traits::const_pointer;
    using reference       = _Ty&;
    using const_reference = const _Ty&;

    using iterator                  = _Deque_iterator<_Scary_val>;
    using const_iterator            = _Deque_const_iterator<_Scary_val>;
    using _Unchecked_iterator       = _Deque_unchecked_iterator<_Scary_val>;
    using _Unchecked_const_iterator = _Deque_unchecked_const_iterator<_Scary_val>;

    using reverse_iterator       = _STD reverse_iterator<iterator>;
    using const_reverse_iterator = _STD reverse_iterator<const_iterator>;
    enum { _EEN_DS = _Block_size }; // helper for expression evaluator

    deque() : _Mypair(_Zero_then_variadic_args_t{}) {
        _Get_data()._Alloc_proxy(static_cast<_Alproxy_ty>(_Getal()));
    }

    explicit deque(const _Alloc& _Al) : _Mypair(_One_then_variadic_args_t{}, _Al) {
        _Get_data()._Alloc_proxy(static_cast<_Alproxy_ty>(_Getal()));
    }

    explicit deque(_CRT_GUARDOVERFLOW size_type _Count, const _Alloc& _Al = _Alloc())
        : _Mypair(_One_then_variadic_args_t{}, _Al) {
        _Alproxy_ty _Alproxy(_Getal());
        _Container_proxy_ptr12<_Alproxy_ty> _Proxy(_Alproxy, _Get_data());
        resize(_Count);
        _Proxy._Release();
    }

    deque(_CRT_GUARDOVERFLOW size_type _Count, const _Ty& _Val) : _Mypair(_Zero_then_variadic_args_t{}) {
        _Alproxy_ty _Alproxy(_Getal());
        _Container_proxy_ptr12<_Alproxy_ty> _Proxy(_Alproxy, _Get_data());
        _Construct_n(_Count, _Val);
        _Proxy._Release();
    }

    deque(_CRT_GUARDOVERFLOW size_type _Count, const _Ty& _Val, const _Alloc& _Al)
        : _Mypair(_One_then_variadic_args_t{}, _Al) {
        _Alproxy_ty _Alproxy(_Getal());
        _Container_proxy_ptr12<_Alproxy_ty> _Proxy(_Alproxy, _Get_data());
        _Construct_n(_Count, _Val);
        _Proxy._Release();
    }

    deque(const deque& _Right)
        : _Mypair(_One_then_variadic_args_t{}, _Alty_traits::select_on_container_copy_construction(_Right._Getal())) {
        _Alproxy_ty _Alproxy(_Getal());
        _Container_proxy_ptr12<_Alproxy_ty> _Proxy(_Alproxy, _Get_data());
        _Construct(_Right._Unchecked_begin(), _Right._Unchecked_end());
        _Proxy._Release();
    }

    deque(const deque& _Right, const _Identity_t<_Alloc>& _Al) : _Mypair(_One_then_variadic_args_t{}, _Al) {
        _Alproxy_ty _Alproxy(_Getal());
        _Container_proxy_ptr12<_Alproxy_ty> _Proxy(_Alproxy, _Get_data());
        _Construct(_Right._Unchecked_begin(), _Right._Unchecked_end());
        _Proxy._Release();
    }

    template <class _Iter, enable_if_t<_Is_iterator_v<_Iter>, int> = 0>
    deque(_Iter _First, _Iter _Last) : _Mypair(_Zero_then_variadic_args_t{}) {
        _Alproxy_ty _Alproxy(_Getal());
        _Container_proxy_ptr12<_Alproxy_ty> _Proxy(_Alproxy, _Get_data());
        _Construct(_First, _Last);
        _Proxy._Release();
    }

    template <class _Iter, enable_if_t<_Is_iterator_v<_Iter>, int> = 0>
    deque(_Iter _First, _Iter _Last, const _Alloc& _Al) : _Mypair(_One_then_variadic_args_t{}, _Al) {
        _Alproxy_ty _Alproxy(_Getal());
        _Container_proxy_ptr12<_Alproxy_ty> _Proxy(_Alproxy, _Get_data());
        _Construct(_First, _Last);
        _Proxy._Release();
    }

#if _HAS_CXX23 && defined(__cpp_lib_concepts) // TRANSITION, GH-395
    template <_Container_compatible_range<_Ty> _Rng>
    deque(from_range_t, _Rng&& _Range) : _Mypair(_Zero_then_variadic_args_t{}) {
        _Alproxy_ty _Alproxy(_Getal());
        _Container_proxy_ptr12<_Alproxy_ty> _Proxy(_Alproxy, _Get_data());
        _Construct(_RANGES _Ubegin(_Range), _RANGES _Uend(_Range));
        _Proxy._Release();
    }

    template <_Container_compatible_range<_Ty> _Rng>
    deque(from_range_t, _Rng&& _Range, const _Alloc& _Al) : _Mypair(_One_then_variadic_args_t{}, _Al) {
        _Alproxy_ty _Alproxy(_Getal());
        _Container_proxy_ptr12<_Alproxy_ty> _Proxy(_Alproxy, _Get_data());
        _Construct(_RANGES _Ubegin(_Range), _RANGES _Uend(_Range));
        _Proxy._Release();
    }
#endif // _HAS_CXX23 && defined(__cpp_lib_concepts)

private:
    template <class _Iter, class _Sent>
    void _Construct(_Iter _First, const _Sent _Last) { // initialize from input range [_First, _Last)
        _Tidy_guard<deque> _Guard{this};
        for (; _First != _Last; ++_First) {
            _Emplace_back_internal(*_First);
        }

        _Guard._Target = nullptr;
    }

    void _Construct_n(size_type _Count, const _Ty& _Val) { // construct from _Count * _Val
        _Tidy_guard<deque> _Guard{this};
        for (; _Count > 0; --_Count) {
            _Emplace_back_internal(_Val);
        }

        _Guard._Target = nullptr;
    }

public:
    deque(deque&& _Right) : _Mypair(_One_then_variadic_args_t{}, _STD move(_Right._Getal())) {
        _Get_data()._Alloc_proxy(static_cast<_Alproxy_ty>(_Getal()));
        _Take_contents(_Right);
    }

    deque(deque&& _Right, const _Identity_t<_Alloc>& _Al) : _Mypair(_One_then_variadic_args_t{}, _Al) {
        _Alproxy_ty _Alproxy(_Getal());
        if constexpr (!_Alty_traits::is_always_equal::value) {
            if (_Getal() != _Right._Getal()) {
                _Container_proxy_ptr12<_Alproxy_ty> _Proxy(_Alproxy, _Get_data());
                _Construct(_STD make_move_iterator(_Right._Unchecked_begin()),
                    _STD make_move_iterator(_Right._Unchecked_end()));
                _Proxy._Release();
                return;
            }
        }

        _Get_data()._Alloc_proxy(_Alproxy);
        _Take_contents(_Right);
    }

    deque& operator=(deque&& _Right) noexcept(_Alty_traits::is_always_equal::value) {
        if (this == _STD addressof(_Right)) {
            return *this;
        }

        auto& _Al                 = _Getal();
        auto& _Right_al           = _Right._Getal();
        constexpr auto _Pocma_val = _Choose_pocma_v<_Alty>;
        if constexpr (_Pocma_val == _Pocma_values::_Propagate_allocators) {
            if (_Al != _Right_al) {
                _Alproxy_ty _Alproxy(_Al);
                _Alproxy_ty _Right_alproxy(_Right_al);
                _Container_proxy_ptr12<_Alproxy_ty> _Proxy(_Right_alproxy, _Leave_proxy_unbound{});
                _Tidy();
                _Pocma(_Al, _Right_al);
                _Proxy._Bind(_Alproxy, _STD addressof(_Get_data()));
                _Take_contents(_Right);
                return *this;
            }
        } else if constexpr (_Pocma_val == _Pocma_values::_No_propagate_allocators) {
            if (_Al != _Right_al) {
                assign(_STD make_move_iterator(_Right._Unchecked_begin()),
                    _STD make_move_iterator(_Right._Unchecked_end()));
                return *this;
            }
        }

        _Tidy();
        _Pocma(_Al, _Right_al);
        _Take_contents(_Right);

        return *this;
    }

private:
    void _Take_contents(deque& _Right) noexcept {
        // move from _Right, stealing its contents
        // pre: _Getal() == _Right._Getal()
        auto& _My_data    = _Get_data();
        auto& _Right_data = _Right._Get_data();
        _My_data._Swap_proxy_and_iterators(_Right_data);
        _My_data._Map     = _Right_data._Map;
        _My_data._Mapsize = _Right_data._Mapsize;
        _My_data._Myoff   = _Right_data._Myoff;
        _My_data._Mysize  = _Right_data._Mysize;

        _Right_data._Map     = nullptr;
        _Right_data._Mapsize = 0;
        _Right_data._Myoff   = 0;
        _Right_data._Mysize  = 0;
    }

public:
    void push_front(_Ty&& _Val) {
        emplace_front(_STD move(_Val));
    }

#if _HAS_CXX23 && defined(__cpp_lib_concepts) // TRANSITION, GH-395
    template <_Container_compatible_range<_Ty> _Rng>
    void prepend_range(_Rng&& _Range) {
        _Orphan_all();

        const auto _Oldsize = _Mysize();
        _Restore_old_size_guard<_Pop_direction::_Front> _Guard{this, _Oldsize};
        if constexpr (_RANGES bidirectional_range<_Rng>) {
            const auto _UFirst = _RANGES _Ubegin(_Range);
            auto _ULast        = _RANGES _Get_final_iterator_unwrapped(_Range);
            while (_UFirst != _ULast) {
                _Emplace_front_internal(*--_ULast); // prepend in order
            }
        } else {
            auto _UFirst      = _RANGES _Ubegin(_Range);
            const auto _ULast = _RANGES _Uend(_Range);
            for (; _UFirst != _ULast; ++_UFirst) {
                _Emplace_front_internal(*_UFirst); // prepend flipped
            }

            const auto _Num = static_cast<difference_type>(_Mysize() - _Oldsize);
            _STD reverse(begin(), begin() + _Num); // Per LWG-3742
        }
        _Guard._Container = nullptr;
    }
#endif // _HAS_CXX23 && defined(__cpp_lib_concepts)

    void push_back(_Ty&& _Val) {
        _Orphan_all();
        _Emplace_back_internal(_STD move(_Val));
    }

#if _HAS_CXX23 && defined(__cpp_lib_concepts) // TRANSITION, GH-395
    template <_Container_compatible_range<_Ty> _Rng>
    void append_range(_Rng&& _Range) {
        _Orphan_all();

        const auto _Oldsize = _Mysize();
        auto _UFirst        = _RANGES _Ubegin(_Range);
        const auto _ULast   = _RANGES _Uend(_Range);

        _Restore_old_size_guard<_Pop_direction::_Back> _Guard{this, _Oldsize};
        for (; _UFirst != _ULast; ++_UFirst) {
            _Emplace_back_internal(*_UFirst);
        }
        _Guard._Container = nullptr;
    }
#endif // _HAS_CXX23 && defined(__cpp_lib_concepts)

    iterator insert(const_iterator _Where, _Ty&& _Val) {
        return emplace(_Where, _STD move(_Val));
    }

    template <class... _Valty>
    decltype(auto) emplace_front(_Valty&&... _Val) {
        _Orphan_all();
        _Emplace_front_internal(_STD forward<_Valty>(_Val)...);
#if _HAS_CXX17
        return front();
#endif // _HAS_CXX17
    }

    template <class... _Valty>
    decltype(auto) emplace_back(_Valty&&... _Val) {
        _Orphan_all();
        _Emplace_back_internal(_STD forward<_Valty>(_Val)...);

#if _HAS_CXX17
        return back();
#endif // _HAS_CXX17
    }

    template <class... _Valty>
    iterator emplace(const_iterator _Where, _Valty&&... _Val) {
        const auto _Off = static_cast<size_type>(_Where - begin());

#if _ITERATOR_DEBUG_LEVEL == 2
        _STL_VERIFY(_Off <= _Mysize(), "deque emplace iterator outside range");
#endif // _ITERATOR_DEBUG_LEVEL == 2

        if (_Off <= _Mysize() / 2) { // closer to front, push to front then rotate
            emplace_front(_STD forward<_Valty>(_Val)...);
            _STD rotate(begin(), _Next_iter(begin()), begin() + static_cast<difference_type>(1 + _Off));
        } else { // closer to back, push to back then rotate
            emplace_back(_STD forward<_Valty>(_Val)...);
            _STD rotate(begin() + static_cast<difference_type>(_Off), _Prev_iter(end()), end());
        }
        return begin() + static_cast<difference_type>(_Off);
    }

    deque(initializer_list<_Ty> _Ilist, const _Alloc& _Al = allocator_type())
        : _Mypair(_One_then_variadic_args_t{}, _Al) {
        _Alproxy_ty _Alproxy(_Getal());
        _Container_proxy_ptr12<_Alproxy_ty> _Proxy(_Alproxy, _Get_data());
        _Construct(_Ilist.begin(), _Ilist.end());
        _Proxy._Release();
    }

    deque& operator=(initializer_list<_Ty> _Ilist) {
        assign(_Ilist.begin(), _Ilist.end());
        return *this;
    }

    void assign(initializer_list<_Ty> _Ilist) {
        assign(_Ilist.begin(), _Ilist.end());
    }

    iterator insert(const_iterator _Where, initializer_list<_Ty> _Ilist) {
        return insert(_Where, _Ilist.begin(), _Ilist.end());
    }

    ~deque() noexcept {
        _Tidy();
        _Alproxy_ty _Proxy_allocator(_Getal());
        _Delete_plain_internal(_Proxy_allocator, _STD exchange(_Get_data()._Myproxy, nullptr));
    }

    deque& operator=(const deque& _Right) {
        if (this == _STD addressof(_Right)) {
            return *this;
        }

        auto& _Al       = _Getal();
        auto& _Right_al = _Right._Getal();
        if constexpr (_Choose_pocca_v<_Alty>) {
            if (_Al != _Right_al) {
                _Tidy();
                _Get_data()._Reload_proxy(static_cast<_Alproxy_ty>(_Al), static_cast<_Alproxy_ty>(_Right_al));
            }
        }

        _Pocca(_Al, _Right_al);
        assign(_Right._Unchecked_begin(), _Right._Unchecked_end());

        return *this;
    }

    _NODISCARD iterator begin() noexcept {
        return iterator(_Myoff(), _STD addressof(_Get_data()));
    }

    _NODISCARD const_iterator begin() const noexcept {
        return const_iterator(_Myoff(), _STD addressof(_Get_data()));
    }

    _NODISCARD iterator end() noexcept {
        return iterator(_Myoff() + _Mysize(), _STD addressof(_Get_data()));
    }

    _NODISCARD const_iterator end() const noexcept {
        return const_iterator(_Myoff() + _Mysize(), _STD addressof(_Get_data()));
    }

    _Unchecked_iterator _Unchecked_begin() noexcept {
        return _Unchecked_iterator(_Myoff(), _STD addressof(_Get_data()));
    }

    _Unchecked_const_iterator _Unchecked_begin() const noexcept {
        return _Unchecked_const_iterator(_Myoff(), _STD addressof(_Get_data()));
    }

    _Unchecked_iterator _Unchecked_end() noexcept {
        return _Unchecked_iterator(_Myoff() + _Mysize(), _STD addressof(_Get_data()));
    }

    _Unchecked_const_iterator _Unchecked_end() const noexcept {
        return _Unchecked_const_iterator(_Myoff() + _Mysize(), _STD addressof(_Get_data()));
    }

    iterator _Make_iter(const_iterator _Where) const noexcept {
        return iterator(_Where._Myoff, _STD addressof(_Get_data()));
    }

    _NODISCARD reverse_iterator rbegin() noexcept {
        return reverse_iterator(end());
    }

    _NODISCARD const_reverse_iterator rbegin() const noexcept {
        return const_reverse_iterator(end());
    }

    _NODISCARD reverse_iterator rend() noexcept {
        return reverse_iterator(begin());
    }

    _NODISCARD const_reverse_iterator rend() const noexcept {
        return const_reverse_iterator(begin());
    }

    _NODISCARD const_iterator cbegin() const noexcept {
        return begin();
    }

    _NODISCARD const_iterator cend() const noexcept {
        return end();
    }

    _NODISCARD const_reverse_iterator crbegin() const noexcept {
        return rbegin();
    }

    _NODISCARD const_reverse_iterator crend() const noexcept {
        return rend();
    }

    void shrink_to_fit() {
        size_type _Oldcapacity = _Block_size * _Mapsize();
        size_type _Newcapacity = _Oldcapacity / 2;

        if (_Newcapacity < _Block_size * _Minimum_map_size) {
            _Newcapacity = _Block_size * _Minimum_map_size;
        }

        if ((empty() && _Mapsize() > 0)
            || (!empty() && size() <= _Newcapacity && _Newcapacity < _Oldcapacity)) { // worth shrinking, do it
            deque _Tmp(_STD make_move_iterator(begin()), _STD make_move_iterator(end()));
            swap(_Tmp);
        }
    }

    void resize(_CRT_GUARDOVERFLOW size_type _Newsize) {
        _Orphan_all();
        while (_Mysize() < _Newsize) {
            _Emplace_back_internal();
        }

        while (_Mysize() > _Newsize) {
            pop_back();
        }
    }

    void resize(_CRT_GUARDOVERFLOW size_type _Newsize, const _Ty& _Val) {
        _Orphan_all();
        while (_Mysize() < _Newsize) {
            _Emplace_back_internal(_Val);
        }

        while (_Mysize() > _Newsize) {
            pop_back();
        }
    }

    _NODISCARD size_type size() const noexcept {
        return _Mysize();
    }

    _NODISCARD size_type max_size() const noexcept {
        return (_STD min)(
            static_cast<size_type>((numeric_limits<difference_type>::max)()), _Alty_traits::max_size(_Getal()));
    }

    _NODISCARD_EMPTY_MEMBER bool empty() const noexcept {
        return _Mysize() == 0;
    }

    _NODISCARD allocator_type get_allocator() const noexcept {
        return static_cast<allocator_type>(_Getal());
    }

    _NODISCARD const_reference at(size_type _Pos) const {
        if (_Mysize() <= _Pos) {
            _Xran();
        }

        return *(begin() + static_cast<difference_type>(_Pos));
    }

    _NODISCARD reference at(size_type _Pos) {
        if (_Mysize() <= _Pos) {
            _Xran();
        }

        return *(begin() + static_cast<difference_type>(_Pos));
    }

    _NODISCARD const_reference operator[](size_type _Pos) const noexcept /* strengthened */ {
#if _CONTAINER_DEBUG_LEVEL > 0
        _STL_VERIFY(_Pos < _Mysize(), "deque subscript out of range");
#endif // _CONTAINER_DEBUG_LEVEL > 0

        return *(_Unchecked_begin() + static_cast<difference_type>(_Pos));
    }

    _NODISCARD reference operator[](size_type _Pos) noexcept /* strengthened */ {
#if _CONTAINER_DEBUG_LEVEL > 0
        _STL_VERIFY(_Pos < _Mysize(), "deque subscript out of range");
#endif // _CONTAINER_DEBUG_LEVEL > 0

        return *(_Unchecked_begin() + static_cast<difference_type>(_Pos));
    }

    _NODISCARD reference front() noexcept /* strengthened */ {
#if _CONTAINER_DEBUG_LEVEL > 0
        _STL_VERIFY(!empty(), "front() called on empty deque");
#endif // _CONTAINER_DEBUG_LEVEL > 0

        return *_Unchecked_begin();
    }

    _NODISCARD const_reference front() const noexcept /* strengthened */ {
#if _CONTAINER_DEBUG_LEVEL > 0
        _STL_VERIFY(!empty(), "front() called on empty deque");
#endif // _CONTAINER_DEBUG_LEVEL > 0

        return *_Unchecked_begin();
    }

    _NODISCARD reference back() noexcept /* strengthened */ {
#if _CONTAINER_DEBUG_LEVEL > 0
        _STL_VERIFY(!empty(), "back() called on empty deque");
#endif // _CONTAINER_DEBUG_LEVEL > 0

        return *_Prev_iter(_Unchecked_end());
    }

    _NODISCARD const_reference back() const noexcept /* strengthened */ {
#if _CONTAINER_DEBUG_LEVEL > 0
        _STL_VERIFY(!empty(), "back() called on empty deque");
#endif // _CONTAINER_DEBUG_LEVEL > 0

        return *_Prev_iter(_Unchecked_end());
    }

    void push_front(const _Ty& _Val) {
        emplace_front(_Val);
    }

    void pop_front() noexcept /* strengthened */ {
#if _ITERATOR_DEBUG_LEVEL == 2
        if (empty()) {
            _STL_REPORT_ERROR("deque empty before pop");
        } else { // something to erase, do it
            _Orphan_off(_Myoff());
            size_type _Block = _Getblock(_Myoff());
            _Alty_traits::destroy(_Getal(), _Unfancy(_Map()[_Block] + _Myoff() % _Block_size));
            if (--_Mysize() == 0) {
                _Myoff() = 0;
            } else {
                ++_Myoff();
            }
        }

#else // _ITERATOR_DEBUG_LEVEL == 2
        size_type _Block = _Getblock(_Myoff());
        _Alty_traits::destroy(_Getal(), _Unfancy(_Map()[_Block] + _Myoff() % _Block_size));
        if (--_Mysize() == 0) {
            _Myoff() = 0;
        } else {
            ++_Myoff();
        }
#endif // _ITERATOR_DEBUG_LEVEL == 2
    }

private:
    template <class... _Tys>
    void _Emplace_back_internal(_Tys&&... _Vals) {
        if ((_Myoff() + _Mysize()) % _Block_size == 0 && _Mapsize() <= (_Mysize() + _Block_size) / _Block_size) {
            _Growmap(1);
        }
        _Myoff() &= _Mapsize() * _Block_size - 1;
        size_type _Newoff = _Myoff() + _Mysize();
        size_type _Block  = _Getblock(_Newoff);
        if (_Map()[_Block] == nullptr) {
            _Map()[_Block] = _Getal().allocate(_Block_size);
        }

        _Alty_traits::construct(
            _Getal(), _Unfancy(_Map()[_Block] + _Newoff % _Block_size), _STD forward<_Tys>(_Vals)...);

        ++_Mysize();
    }

    template <class... _Tys>
    void _Emplace_front_internal(_Tys&&... _Vals) {
        if (_Myoff() % _Block_size == 0 && _Mapsize() <= (_Mysize() + _Block_size) / _Block_size) {
            _Growmap(1);
        }
        _Myoff() &= _Mapsize() * _Block_size - 1;
        size_type _Newoff      = _Myoff() != 0 ? _Myoff() : _Mapsize() * _Block_size;
        const size_type _Block = _Getblock(--_Newoff);
        if (_Map()[_Block] == nullptr) {
            _Map()[_Block] = _Getal().allocate(_Block_size);
        }

        _Alty_traits::construct(
            _Getal(), _Unfancy(_Map()[_Block] + _Newoff % _Block_size), _STD forward<_Tys>(_Vals)...);

        _Myoff() = _Newoff;
        ++_Mysize();
    }

public:
    void push_back(const _Ty& _Val) {
        _Orphan_all();
        _Emplace_back_internal(_Val);
    }

    void pop_back() noexcept /* strengthened */ {
#if _ITERATOR_DEBUG_LEVEL == 2
        if (empty()) {
            _STL_REPORT_ERROR("deque empty before pop");
        } else { // something to erase, do it
            size_type _Newoff = _Myoff() + _Mysize() - 1;
            _Orphan_off(_Newoff);
            size_type _Block = _Getblock(_Newoff);
            _Alty_traits::destroy(_Getal(), _Unfancy(_Map()[_Block] + _Newoff % _Block_size));
            if (--_Mysize() == 0) {
                _Myoff() = 0;
            }
        }

#else // _ITERATOR_DEBUG_LEVEL == 2
        size_type _Newoff = _Myoff() + _Mysize() - 1;
        size_type _Block  = _Getblock(_Newoff);
        _Alty_traits::destroy(_Getal(), _Unfancy(_Map()[_Block] + _Newoff % _Block_size));
        if (--_Mysize() == 0) {
            _Myoff() = 0;
        }
#endif // _ITERATOR_DEBUG_LEVEL == 2
    }

private:
    template <class _Iter, class _Sent>
    void _Assign_range(_Iter _First, const _Sent _Last) {
        _Orphan_all();
        auto _Myfirst      = _Unchecked_begin();
        const auto _Mylast = _Unchecked_end();
        // Reuse existing elements
        for (; _Myfirst != _Mylast; ++_Myfirst, (void) ++_First) {
            if (_First == _Last) {
                _Erase_last_n(static_cast<size_type>(_Mylast - _Myfirst));
                return;
            }

            *_Myfirst = *_First;
        }

        // Allocate new elements for remaining tail of values
        for (; _First != _Last; ++_First) {
            _Emplace_back_internal(*_First);
        }
    }

public:
    template <class _Iter, enable_if_t<_Is_iterator_v<_Iter>, int> = 0>
    void assign(_Iter _First, _Iter _Last) {
        _Adl_verify_range(_First, _Last);
        _Assign_range(_Get_unwrapped(_First), _Get_unwrapped(_Last));
    }

#if _HAS_CXX23 && defined(__cpp_lib_concepts) // TRANSITION, GH-395
    template <_Container_compatible_range<_Ty> _Rng>
    void assign_range(_Rng&& _Range) {
        _Assign_range(_RANGES _Ubegin(_Range), _RANGES _Uend(_Range));
    }
#endif // _HAS_CXX23 && defined(__cpp_lib_concepts)

    void assign(_CRT_GUARDOVERFLOW size_type _Count, const _Ty& _Val) { // assign _Count * _Val
        _Orphan_all();
        auto _Myfirst       = _Unchecked_begin();
        const auto _Oldsize = _Mysize();
        auto _Assign_count  = (_STD min)(_Count, _Oldsize);
        for (; _Assign_count > 0; --_Assign_count) {
            *_Myfirst = _Val;
            ++_Myfirst;
        }

        const auto _Shrink_by = _Oldsize - _Assign_count;
        auto _Extend_by       = _Count - _Assign_count;
        _Erase_last_n(_Shrink_by);
        for (; _Extend_by > 0; --_Extend_by) {
            _Emplace_back_internal(_Val);
        }
    }

    iterator insert(const_iterator _Where, const _Ty& _Val) {
        size_type _Off = static_cast<size_type>(_Where - begin());

#if _ITERATOR_DEBUG_LEVEL == 2
        _STL_VERIFY(_Off <= _Mysize(), "deque insert iterator outside range");
#endif // _ITERATOR_DEBUG_LEVEL == 2

        if (_Off <= _Mysize() / 2) { // closer to front, push to front then copy
            push_front(_Val);
            _STD rotate(begin(), _Next_iter(begin()), begin() + static_cast<difference_type>(1 + _Off));
        } else { // closer to back, push to back then copy
            push_back(_Val);
            _STD rotate(begin() + static_cast<difference_type>(_Off), _Prev_iter(end()), end());
        }

        return begin() + static_cast<difference_type>(_Off);
    }

    iterator insert(const_iterator _Where, _CRT_GUARDOVERFLOW size_type _Count, const _Ty& _Val) {
        // insert _Count * _Val at _Where
        size_type _Off = static_cast<size_type>(_Where - begin());
        _Insert_n(_Where, _Count, _Val);
        return begin() + static_cast<difference_type>(_Off);
    }

private:
    enum class _Pop_direction : bool {
        _Front,
        _Back,
    };

    template <_Pop_direction _Direction>
    struct _NODISCARD _Restore_old_size_guard {
        deque* _Container;
        const size_type _Oldsize;

        ~_Restore_old_size_guard() { // restore old size, at least
            if (_Container) {
                while (_Oldsize < _Container->_Mysize()) {
                    if constexpr (_Direction == _Pop_direction::_Front) {
                        _Container->pop_front();
                    } else {
                        _Container->pop_back();
                    }
                }
            }
        }
    };

    enum class _Is_bidi : bool { _No, _Yes };

    template <_Is_bidi _Bidi, class _Iter, class _Sent>
    iterator _Insert_range(const size_type _Off, _Iter _First, _Sent _Last) {
        // insert [_First, _Last) at begin() + _Off
        // Pre: _Bidi == _Is_bidi::_Yes implies that _Iter and _Sent are the same type, and that
        //      _Iter is either a bidirectional_iterator or a Cpp17BidirectionalIterator.

#if _ITERATOR_DEBUG_LEVEL == 2
        _STL_VERIFY(_Mysize() >= _Off, "deque insert iterator outside range");
#endif // _ITERATOR_DEBUG_LEVEL == 2

        if (_First == _Last) {
            return begin() + static_cast<difference_type>(_Off);
        }

        const size_type _Oldsize = _Mysize();

        _Orphan_all();
        if (_Off <= _Oldsize / 2) { // closer to front, push to front then rotate
            _Restore_old_size_guard<_Pop_direction::_Front> _Guard{this, _Oldsize};
            if constexpr (_Bidi == _Is_bidi::_Yes) {
                while (_First != _Last) {
                    _Emplace_front_internal(*--_Last); // prepend in order
                }
            } else {
                for (; _First != _Last; ++_First) {
                    _Emplace_front_internal(*_First); // prepend flipped
                }
            }

            _Guard._Container = nullptr;

            const auto _Num     = static_cast<difference_type>(_Mysize() - _Oldsize);
            const auto _Myfirst = _Unchecked_begin();
            const auto _Mymid   = _Myfirst + _Num;
            if constexpr (_Bidi == _Is_bidi::_No) {
                _STD reverse(_Myfirst, _Mymid); // flip new stuff in place
            }
            _STD rotate(_Myfirst, _Mymid, _Mymid + static_cast<difference_type>(_Off));
            return begin() + static_cast<difference_type>(_Off);
        }

        _Restore_old_size_guard<_Pop_direction::_Back> _Guard{this, _Oldsize};
        for (; _First != _Last; ++_First) {
            _Emplace_back_internal(*_First);
        }

        _Guard._Container = nullptr;

        const auto _Myfirst = _Unchecked_begin();
        _STD rotate(_Myfirst + static_cast<difference_type>(_Off), _Myfirst + static_cast<difference_type>(_Oldsize),
            _Unchecked_end());

        return begin() + static_cast<difference_type>(_Off);
    }

public:
    template <class _Iter, enable_if_t<_Is_iterator_v<_Iter>, int> = 0>
    iterator insert(const_iterator _Where, _Iter _First, _Iter _Last) {
        // insert [_First, _Last) at _Where
        _Adl_verify_range(_First, _Last);
        const size_type _Off = static_cast<size_type>(_Where - begin());
        return _Insert_range<static_cast<_Is_bidi>(_Is_cpp17_bidi_iter_v<_Iter>)>(
            _Off, _Get_unwrapped(_First), _Get_unwrapped(_Last));
    }

#if _HAS_CXX23 && defined(__cpp_lib_concepts) // TRANSITION, GH-395
    template <_Container_compatible_range<_Ty> _Rng>
    iterator insert_range(const_iterator _Where, _Rng&& _Range) {
        const size_type _Off = static_cast<size_type>(_Where - begin());

        if constexpr (_RANGES bidirectional_range<_Rng>) {
            return _Insert_range<_Is_bidi::_Yes>(
                _Off, _RANGES _Ubegin(_Range), _RANGES _Get_final_iterator_unwrapped(_Range));
        } else {
            return _Insert_range<_Is_bidi::_No>(_Off, _RANGES _Ubegin(_Range), _RANGES _Uend(_Range));
        }
    }
#endif // _HAS_CXX23 && defined(__cpp_lib_concepts)

    iterator erase(const_iterator _Where) noexcept(is_nothrow_move_assignable_v<value_type>) /* strengthened */ {
        return erase(_Where, _Next_iter(_Where));
    }

    iterator erase(const_iterator _First_arg, const_iterator _Last_arg) noexcept(
        is_nothrow_move_assignable_v<value_type>) /* strengthened */ {
        iterator _First = _Make_iter(_First_arg);
        iterator _Last  = _Make_iter(_Last_arg);

#if _ITERATOR_DEBUG_LEVEL == 2
        _STL_VERIFY(_First <= _Last && begin() <= _First && _Last <= end(), "deque erase iterator outside range");
        _Adl_verify_range(_First, _Last);

        auto _Off   = static_cast<size_type>(_First - begin());
        auto _Count = static_cast<size_type>(_Last - _First);
        bool _Moved = _Off > 0 && _Off + _Count < _Mysize();

#else // _ITERATOR_DEBUG_LEVEL == 2
        auto _Off   = static_cast<size_type>(_First - begin());
        auto _Count = static_cast<size_type>(_Last - _First);
#endif // _ITERATOR_DEBUG_LEVEL == 2

        if (_Count == 0) {
            return _First;
        }

        if (_Off < static_cast<size_type>(end() - _Last)) { // closer to front
            _STD move_backward(begin(), _First, _Last); // copy over hole
            for (; _Count > 0; --_Count) {
                pop_front(); // pop copied elements
            }
        } else { // closer to back
            _STD move(_Last, end(), _First); // copy over hole
            for (; _Count > 0; --_Count) {
                pop_back(); // pop copied elements
            }
        }

#if _ITERATOR_DEBUG_LEVEL == 2
        if (_Moved) {
            _Orphan_all();
        }
#endif // _ITERATOR_DEBUG_LEVEL == 2

        return begin() + static_cast<difference_type>(_Off);
    }

private:
    void _Erase_last_n(size_type _Count) noexcept {
        for (; _Count > 0; --_Count) {
            pop_back();
        }
    }

public:
    void clear() noexcept { // erase all
        _Tidy();
    }

    void swap(deque& _Right) noexcept /* strengthened */ {
        if (this != _STD addressof(_Right)) {
            _Pocs(_Getal(), _Right._Getal());
            auto& _My_data    = _Get_data();
            auto& _Right_data = _Right._Get_data();
            _My_data._Swap_proxy_and_iterators(_Right_data);
            _Swap_adl(_My_data._Map, _Right_data._Map);
            _STD swap(_My_data._Mapsize, _Right_data._Mapsize);
            _STD swap(_My_data._Myoff, _Right_data._Myoff);
            _STD swap(_My_data._Mysize, _Right_data._Mysize);
        }
    }

private:
    void _Insert_n(const_iterator _Where, size_type _Count, const _Ty& _Val) { // insert _Count * _Val at _Where
        iterator _Mid;
        size_type _Num;
        size_type _Off     = static_cast<size_type>(_Where - begin());
        size_type _Oldsize = _Mysize();
        size_type _Rem     = _Oldsize - _Off;

#if _ITERATOR_DEBUG_LEVEL == 2
        _STL_VERIFY(_Off <= _Oldsize, "deque insert iterator outside range");
#endif // _ITERATOR_DEBUG_LEVEL == 2

        if (_Off < _Rem) { // closer to front
            _Restore_old_size_guard<_Pop_direction::_Front> _Guard{this, _Oldsize};
            if (_Off < _Count) { // insert longer than prefix
                for (_Num = _Count - _Off; _Num > 0; --_Num) {
                    push_front(_Val); // push excess values
                }
                for (_Num = _Off; _Num > 0; --_Num) {
                    push_front(begin()[static_cast<difference_type>(_Count - 1)]); // push prefix
                }

                _Mid = begin() + static_cast<difference_type>(_Count);
                _STD fill(_Mid, _Mid + static_cast<difference_type>(_Off), _Val); // fill in rest of values
            } else { // insert not longer than prefix
                for (_Num = _Count; _Num > 0; --_Num) {
                    push_front(begin()[static_cast<difference_type>(_Count - 1)]); // push part of prefix
                }

                _Mid = begin() + static_cast<difference_type>(_Count);
                _Alloc_temporary2<_Alty> _Tmp(_Getal(), _Val); // in case _Val is in sequence
                _STD move(_Mid + static_cast<difference_type>(_Count), _Mid + static_cast<difference_type>(_Off),
                    _Mid); // copy rest of prefix
                _STD fill(begin() + static_cast<difference_type>(_Off), _Mid + static_cast<difference_type>(_Off),
                    _Tmp._Get_value()); // fill in values
            }
            _Guard._Container = nullptr;
        } else { // closer to back
            _Restore_old_size_guard<_Pop_direction::_Back> _Guard{this, _Oldsize};
            if (_Rem < _Count) { // insert longer than suffix
                _Orphan_all();
                for (_Num = _Count - _Rem; _Num > 0; --_Num) {
                    _Emplace_back_internal(_Val); // push excess values
                }
                for (_Num = 0; _Num < _Rem; ++_Num) {
                    _Emplace_back_internal(begin()[static_cast<difference_type>(_Off + _Num)]); // push suffix
                }

                _Mid = begin() + static_cast<difference_type>(_Off);
                _STD fill(_Mid, _Mid + static_cast<difference_type>(_Rem), _Val); // fill in rest of values
            } else { // insert not longer than prefix
                for (_Num = 0; _Num < _Count; ++_Num) {
                    _Emplace_back_internal(
                        begin()[static_cast<difference_type>(_Off + _Rem - _Count + _Num)]); // push part of prefix
                }

                _Mid = begin() + static_cast<difference_type>(_Off);
                _Alloc_temporary2<_Alty> _Tmp(_Getal(), _Val); // in case _Val is in sequence
                _STD move_backward(_Mid, _Mid + static_cast<difference_type>(_Rem - _Count),
                    _Mid + static_cast<difference_type>(_Rem)); // copy rest of prefix
                _STD fill(_Mid, _Mid + static_cast<difference_type>(_Count),
                    _Tmp._Get_value()); // fill in values
            }
            _Guard._Container = nullptr;
        }
    }

    [[noreturn]] void _Xlen() const {
        _Xlength_error("deque<T> too long");
    }

    [[noreturn]] void _Xran() const {
        _Xout_of_range("invalid deque<T> subscript");
    }

    void _Growmap(size_type _Count) { // grow map by at least _Count pointers, _Mapsize() a power of 2
        static_assert(_Minimum_map_size > 1, "The _Xlen() test should always be performed.");

        _Alpty _Almap(_Getal());
        size_type _Newsize = _Mapsize() > 0 ? _Mapsize() : 1;
        while (_Newsize - _Mapsize() < _Count || _Newsize < _Minimum_map_size) {
            // scale _Newsize to 2^N >= _Mapsize() + _Count
            if (max_size() / _Block_size - _Newsize < _Newsize) {
                _Xlen(); // result too long
            }

            _Newsize *= 2;
        }
        _Count = _Newsize - _Mapsize();

        size_type _Myboff = _Myoff() / _Block_size;
        _Mapptr _Newmap   = _Almap.allocate(_Mapsize() + _Count);
        _Mapptr _Myptr    = _Newmap + _Myboff;

        _Myptr = _STD uninitialized_copy(_Map() + _Myboff, _Map() + _Mapsize(), _Myptr); // copy initial to end
        if (_Myboff <= _Count) { // increment greater than offset of initial block
            _Myptr = _STD uninitialized_copy(_Map(), _Map() + _Myboff, _Myptr); // copy rest of old
            _Uninitialized_value_construct_n_unchecked1(_Myptr, _Count - _Myboff); // clear suffix of new
            _Uninitialized_value_construct_n_unchecked1(_Newmap, _Myboff); // clear prefix of new
        } else { // increment not greater than offset of initial block
            _STD uninitialized_copy(_Map(), _Map() + _Count, _Myptr); // copy more old
            _Myptr = _STD uninitialized_copy(_Map() + _Count, _Map() + _Myboff, _Newmap); // copy rest of old
            _Uninitialized_value_construct_n_unchecked1(_Myptr, _Count); // clear rest to initial block
        }

        if (_Map() != nullptr) {
            _Destroy_range(_Map(), _Map() + _Mapsize());
            _Almap.deallocate(_Map(), _Mapsize()); // free storage for old
        }

        _Map() = _Newmap; // point at new
        _Mapsize() += _Count;
    }

    void _Tidy() noexcept { // free all storage
        _Orphan_all();

        _Alpty _Almap(_Getal());
        while (!empty()) {
            pop_back();
        }

        if (_Map() != nullptr) {
            for (size_type _Block = _Mapsize(); _Block > 0;) { // free storage for a block and destroy pointer
                if (_Map()[--_Block]) { // free block
                    _Getal().deallocate(_Map()[_Block], _Block_size);
                }
                _Destroy_in_place(_Map()[_Block]); // destroy pointer to block
            }

            _Almap.deallocate(_Map(), _Mapsize()); // free storage for map
        }

        _Mapsize() = 0;
        _Map()     = nullptr;
    }

#if _ITERATOR_DEBUG_LEVEL == 2
    void _Orphan_off(size_type _Offlo) const noexcept { // orphan iterators with specified offset(s)
        size_type _Offhigh = _Myoff() + _Mysize() <= _Offlo + 1 ? static_cast<size_type>(-1) : _Offlo;
        if (_Offlo == _Myoff()) {
            _Offlo = 0;
        }

        _Lockit _Lock(_LOCK_DEBUG);
        _Iterator_base12** _Pnext = &_Get_data()._Myproxy->_Myfirstiter;
        while (*_Pnext) {
            const auto _Pnextoff = static_cast<const_iterator&>(**_Pnext)._Myoff;
            if (_Pnextoff < _Offlo || _Offhigh < _Pnextoff) {
                _Pnext = &(*_Pnext)->_Mynextiter;
            } else { // orphan the iterator
                (*_Pnext)->_Myproxy = nullptr;
                *_Pnext             = (*_Pnext)->_Mynextiter;
            }
        }
    }
#endif // _ITERATOR_DEBUG_LEVEL == 2

    size_type _Getblock(size_type _Off) const noexcept {
        return _Get_data()._Getblock(_Off);
    }

    void _Orphan_all() noexcept {
        _Get_data()._Orphan_all();
    }

    _Alty& _Getal() noexcept {
        return _Mypair._Get_first();
    }

    const _Alty& _Getal() const noexcept {
        return _Mypair._Get_first();
    }

    _Scary_val& _Get_data() noexcept {
        return _Mypair._Myval2;
    }

    const _Scary_val& _Get_data() const noexcept {
        return _Mypair._Myval2;
    }

    _Mapptr& _Map() noexcept {
        return _Get_data()._Map;
    }

    const _Mapptr& _Map() const noexcept {
        return _Get_data()._Map;
    }

    size_type& _Mapsize() noexcept {
        return _Get_data()._Mapsize;
    }

    const size_type& _Mapsize() const noexcept {
        return _Get_data()._Mapsize;
    }

    size_type& _Myoff() noexcept {
        return _Get_data()._Myoff;
    }

    const size_type& _Myoff() const noexcept {
        return _Get_data()._Myoff;
    }

    size_type& _Mysize() noexcept {
        return _Get_data()._Mysize;
    }

    const size_type& _Mysize() const noexcept {
        return _Get_data()._Mysize;
    }

    _Compressed_pair<_Alty, _Scary_val> _Mypair;
};

#if _HAS_CXX17
template <class _Iter, class _Alloc = allocator<_Iter_value_t<_Iter>>,
    enable_if_t<conjunction_v<_Is_iterator<_Iter>, _Is_allocator<_Alloc>>, int> = 0>
deque(_Iter, _Iter, _Alloc = _Alloc()) -> deque<_Iter_value_t<_Iter>, _Alloc>;
#endif // _HAS_CXX17

#if _HAS_CXX23 && defined(__cpp_lib_concepts) // TRANSITION, GH-395
template <_RANGES input_range _Rng, class _Alloc = allocator<_RANGES range_value_t<_Rng>>,
    enable_if_t<_Is_allocator<_Alloc>::value, int> = 0>
deque(from_range_t, _Rng&&, _Alloc = _Alloc()) -> deque<_RANGES range_value_t<_Rng>, _Alloc>;
#endif // _HAS_CXX23 && defined(__cpp_lib_concepts)

_EXPORT_STD template <class _Ty, class _Alloc>
void swap(deque<_Ty, _Alloc>& _Left, deque<_Ty, _Alloc>& _Right) noexcept /* strengthened */ {
    _Left.swap(_Right);
}

_EXPORT_STD template <class _Ty, class _Alloc>
_NODISCARD bool operator==(const deque<_Ty, _Alloc>& _Left, const deque<_Ty, _Alloc>& _Right) {
    return _Left.size() == _Right.size()
        && _STD equal(_Left._Unchecked_begin(), _Left._Unchecked_end(), _Right._Unchecked_begin());
}

#if !_HAS_CXX20
template <class _Ty, class _Alloc>
_NODISCARD bool operator!=(const deque<_Ty, _Alloc>& _Left, const deque<_Ty, _Alloc>& _Right) {
    return !(_Left == _Right);
}
#endif // !_HAS_CXX20

#ifdef __cpp_lib_concepts
_EXPORT_STD template <class _Ty, class _Alloc>
_NODISCARD _Synth_three_way_result<_Ty> operator<=>(const deque<_Ty, _Alloc>& _Left, const deque<_Ty, _Alloc>& _Right) {
    return _STD lexicographical_compare_three_way(_Left._Unchecked_begin(), _Left._Unchecked_end(),
        _Right._Unchecked_begin(), _Right._Unchecked_end(), _Synth_three_way{});
}
#else // ^^^ defined(__cpp_lib_concepts) / !defined(__cpp_lib_concepts) vvv
template <class _Ty, class _Alloc>
_NODISCARD bool operator<(const deque<_Ty, _Alloc>& _Left, const deque<_Ty, _Alloc>& _Right) {
    return _STD lexicographical_compare(
        _Left._Unchecked_begin(), _Left._Unchecked_end(), _Right._Unchecked_begin(), _Right._Unchecked_end());
}

template <class _Ty, class _Alloc>
_NODISCARD bool operator<=(const deque<_Ty, _Alloc>& _Left, const deque<_Ty, _Alloc>& _Right) {
    return !(_Right < _Left);
}

template <class _Ty, class _Alloc>
_NODISCARD bool operator>(const deque<_Ty, _Alloc>& _Left, const deque<_Ty, _Alloc>& _Right) {
    return _Right < _Left;
}

template <class _Ty, class _Alloc>
_NODISCARD bool operator>=(const deque<_Ty, _Alloc>& _Left, const deque<_Ty, _Alloc>& _Right) {
    return !(_Left < _Right);
}
#endif // ^^^ !defined(__cpp_lib_concepts) ^^^

#if _HAS_CXX20
_EXPORT_STD template <class _Ty, class _Alloc, class _Uty>
typename deque<_Ty, _Alloc>::size_type erase(deque<_Ty, _Alloc>& _Cont, const _Uty& _Val) {
    return _Erase_remove(_Cont, _Val);
}

_EXPORT_STD template <class _Ty, class _Alloc, class _Pr>
typename deque<_Ty, _Alloc>::size_type erase_if(deque<_Ty, _Alloc>& _Cont, _Pr _Pred) {
    return _Erase_remove_if(_Cont, _Pass_fn(_Pred));
}
#endif // _HAS_CXX20

#if _HAS_CXX17
namespace pmr {
    _EXPORT_STD template <class _Ty>
    using deque = _STD deque<_Ty, polymorphic_allocator<_Ty>>;
} // namespace pmr
#endif // _HAS_CXX17
_STD_END

#pragma pop_macro("new")
_STL_RESTORE_CLANG_WARNINGS
#pragma warning(pop)
#pragma pack(pop)
#endif // _STL_COMPILER_PREPROCESSOR
#endif // _DEQUE_
