#include <utility>

template <class Pred, class = std::void<>>
struct is_closure : std::true_type {
    using type = Pred;
};

template <class Pred>
struct is_closure<Pred, std::void_t<
    decltype(typename Pred::operator ())>> : std::false_type {
    using type = typename Pred::operator ();
};

template <bool, class>
struct is_closure_func_traits_impl;

template <class Pred>
struct is_closure_func_traits_impl<true, Pred> {
    using type = func_traits_closure<Pred>;
};

template <class Pred>
struct is_closure_func_traits_impl<false, Pred> {
    using type = func_traits_nonclosure<Pred>;
};

template <class Pred>
struct is_closure_func_traits
    : is_closure_func_traits_impl<is_closure<Pred>::value, Pred> {};

template <class Pred, class ...Args>
struct func_traits : func_traits_closure<Pred, Args...> {};

template <class Pred>
struct func_traits<Pred> : typename is_closure_func_traits<Pred>::type {};

template <class Ret, class ...Args>
struct func_traits<Ret(Args...)> : func_traits_nonclosure<Ret, Args...> {};



