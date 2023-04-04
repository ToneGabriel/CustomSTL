#pragma once
#include "Utility.h"
#include <iostream>

CUSTOM_BEGIN

template<class Signature>
class Function;

template<class RetType, class... ArgTypes>
class Function<RetType(ArgTypes...)>        // Wrapper template to function pointer
{
private:
    using Invoker           = RetType(*)(const Function&, ArgTypes&&...);
    using Copiator          = void(*)(const Function&);
    using Destructor        = void(*)(const Function&);

private:
    void* _functor          = nullptr;
    Invoker _invoker        = nullptr;
    Copiator _copiator      = nullptr;
    Destructor _destructor  = nullptr;

private:
    // Core functions

    template<class Func>
    static RetType _invoke(const Function& target, ArgTypes&&... args) {
        return (*reinterpret_cast<Func*>(target._functor))(custom::forward<ArgTypes>(args)...);
    }

    template<class Func>
    static void* _copy(const Function& target) {
        return reinterpret_cast<void*>(new Func(*reinterpret_cast<Func*>(target._functor)));
    }

    template<class Func>
    static void _destruct(const Function& target) {
        delete reinterpret_cast<Func*>(target._functor);
    }

public:
    // Constructors

    Function() = default;

    // TODO: enable_if
    template<typename Func>
    Function(Func&& f)
        :   _invoker(reinterpret_cast<Invoker>(_invoke<typename std::decay<Func>::type>)),
            _copiator(reinterpret_cast<Copiator>(_copy<typename std::decay<Func>::type>)),
            _destructor(reinterpret_cast<Destructor>(_destruct<typename std::decay<Func>::type>)),
            _functor(reinterpret_cast<void*>(new typename std::decay<Func>::type(custom::forward<Func>(f))))
            { std::cout << "Init\n"; }

    Function(const Function& other)
        :   _invoker(other._invoker),
            _copiator(other._copiator),
            _destructor(other._destructor)
    {
        std::cout << "Copy\n";
        //_functor = _copiator(other);
    }

    Function(Function&& other) {
        std::cout << "Move\n";

        _invoker            = other._invoker;
        _copiator           = other._copiator;
        _destructor         = other._destructor;
        _functor            = other._functor;

        other._invoker      = nullptr;
        other._copiator     = nullptr;
        other._destructor   = nullptr;
        other._functor      = nullptr;
    }

    ~Function() {
        std::cout << "Destroy\n";
        if (_functor != nullptr)
            _destructor(*this);
    }
    
public:
    // Operators

    RetType operator()(ArgTypes&&... args) const {
        if (_functor == nullptr) 
            throw std::runtime_error("call an empty function object");
        
        return _invoker(*this, custom::forward<ArgTypes>(args)...);
    }

    Function& operator=(const Function& other) {
        if (_functor != other._functor)
        {

        }

        return *this;
    }

    Function& operator=(Function&& other) {
        if (_functor != other._functor)
        {

        }

        return *this;
    }

    bool operator==(const Function& other) const {
        // TODO: implement
        return false;
    }

    bool operator!=(const Function& other) const {
		return !(*this == other);
    }

}; // END Function template

CUSTOM_END


// ==============================================================

// #include <type_traits>
// #include <utility>
// #include <stdexcept>

// template <typename... Args> class function;

// template <typename R, typename... Args>
// class function<R(Args...)> {
// public:
//   /** construct empty function object */
//   function(): m_functor(nullptr), m_invoke(nullptr), m_delete(nullptr), m_copy(nullptr) {}

//   /** destruct function object */
//   ~function() {
//     if (m_delete) m_delete(*this);
//   }

//   /** construct a valid function object from the copy of the given function pointer or functor */
//   template <typename F, typename std::enable_if<!std::is_same<typename std::decay<F>::type, function>::value, int>::type = 1>
//   function(F&& functor): function(1, cast_mem_fn(std::forward<F>(functor))) {}

//   /** run function call */
//   R operator()(Args... args) const {
//     if (!m_functor || !m_invoke) throw std::runtime_error("call an empty function object");
//     return (*m_invoke)(*this, static_cast<Args>(args)...);
//   }

//   /** copy constructor */
//   function(const function &other): m_functor(other.m_copy ? (*other.m_copy)(other) : nullptr),
//     m_invoke(other.m_invoke), m_delete(other.m_delete), m_copy(other.m_copy) {}

//   /** move constructor */
//   function(function &&other): m_functor(other.m_functor), m_invoke(other.m_invoke),
//     m_delete(other.m_delete), m_copy(other.m_copy) {
//     other.m_functor = nullptr;
//     other.m_invoke = nullptr;
//     other.m_delete = nullptr;
//     other.m_copy = nullptr;
//   }

//   /** copy assignment */
//   function& operator=(const function &other) {
//     if (this == &other) return *this;
//     if (m_delete) m_delete(*this);
//     m_functor = other.m_copy ? (*other.m_copy)(other) : nullptr;
//     m_invoke = other.m_invoke;
//     m_delete = other.m_delete;
//     m_copy = other.m_copy;
//     return *this;
//   }

//   /** move assignment */
//   function& operator=(function &&other) {
//     if (m_delete) m_delete(*this);
//     m_functor = other.m_functor;
//     m_invoke = other.m_invoke;
//     m_delete = other.m_delete;
//     m_copy = other.m_copy;
//     other.m_functor = nullptr;
//     other.m_invoke = nullptr;
//     other.m_delete = nullptr;
//     other.m_copy = nullptr;
//     return *this;
//   }

// private:

//   template <typename F>
//   function(int dummy, F&& functor): m_functor(reinterpret_cast<char*>(new
//     typename std::decay<F>::type(std::forward<F>(functor)))) {
//     typedef typename std::decay<F>::type functor_type;
//     static_assert(std::is_same<R, decltype(invoke_impl<functor_type>(
//       function(), std::declval<Args>()...))>::value, "invalid functor type");
//     static_assert(std::is_copy_constructible<functor_type>::value,
//       "uncopyable functor type");
//     m_invoke = invoke_impl<functor_type>;
//     m_delete = delete_impl<functor_type>;
//     m_copy = copy_impl<functor_type>;
//   }

//   template <typename F>
//   F&& cast_mem_fn(F&& f) {
//     return static_cast<F&&>(f);
//   }

//   /** a pointer to class member cannot be casted to char* */
//   /** conversion to a functor can solve this issue */
//   template <typename CLASS, typename METHOD>
//   class pmf_wrapper {
//     METHOD CLASS::*m_pmf;

//   public:
//     pmf_wrapper(METHOD CLASS::*pmf): m_pmf(pmf) {}

//     R operator()(Args... args) {
//       return invoke<METHOD>(static_cast<Args>(args)...);
//     }

//   private:
//     template <typename T, typename std::enable_if<std::is_same<CLASS,
//       typename std::decay<T>::type>::value, int>::type = 1>
//     T&& transfer(T&& inst) {
//       return static_cast<T&&>(inst);
//     }

//     template <typename T, typename std::enable_if<std::is_same<CLASS,
//       typename std::decay<decltype(*T())>::type>::value, int>::type = 1>
//     auto transfer(T&& ptr)->decltype(*ptr) {
//       return *ptr;
//     }
    
//     /** member function pointer */
//     template <typename MT,
//       typename std::enable_if<std::is_function<MT>::value, int>::type = 1,
//       typename T, typename... SUBARGS>
//     R invoke(T&& inst, SUBARGS&&... args) {
//       return (transfer(std::forward<T>(inst)).*m_pmf)(std::forward<SUBARGS>(args)...);
//     }

//     /** data member pointer */
//     template <typename MT,
//       typename std::enable_if<!std::is_function<MT>::value, int>::type = 1,
//       typename T>
//     R invoke(T&& inst) {
//       return transfer(std::forward<T>(inst)).*m_pmf;
//     }
//   };

//   template <typename CLASS, typename METHOD>
//   pmf_wrapper<CLASS, METHOD> cast_mem_fn(METHOD CLASS::*pmf) {
//     return pmf_wrapper<CLASS, METHOD>(pmf);
//   }

//   template <typename F>
//   static auto invoke_impl(const function &obj, Args... args)->
//     decltype(std::declval<F>()(static_cast<Args>(args)...)) {
//     return (*reinterpret_cast<F*>(obj.m_functor))(static_cast<Args>(args)...);
//   }

//   template <typename F>
//   static void delete_impl(const function &obj) {
//     delete reinterpret_cast<F*>(obj.m_functor);
//   }

//   template <typename F>
//   static char *copy_impl(const function &obj) {
//     return reinterpret_cast<char*>(new F(*reinterpret_cast<const F*>(obj.m_functor)));
//   }

//   /** pointer to the internal function pointer/functor object (on heap) */
//   char *m_functor;

//   /** call m_functor */
//   R (*m_invoke)(const function &obj, Args... args);

//   /** destroy m_functor */
//   void (*m_delete)(const function &obj);

//   /** copy m_functor */
//   char *(*m_copy)(const function &obj);

// };

// ==============================================================
