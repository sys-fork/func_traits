#include <utility>
#include <type_traits>

struct fntraits {
private:
    struct utility {
        template <class ...>
        struct pack {};
    
        template <class T>
        struct remove_modification {
            using type = T;
        };

        template <class T>
        struct remove_modification<T&> : remove_modification<T> {};

        template <class T>
        struct remove_modification<T&&> : remove_modification<T> {};

        template <class T>
        struct remove_modification<T const *> : remove_modification<T *> {};

        template<class T>
        struct remove_modification<const T> : remove_modification<T> {};

        template <class T>
        struct remove_modification<T **> : remove_modification<T> {};

        template <class, class, class = std::void_t<>>
        struct is_callable_with_impl : std::false_type {};

        template <class Lambda, class ...RequiredArgs>
        struct is_callable_with_impl
        <Lambda, pack<RequiredArgs...>,
            std::void_t<decltype(std::declval<Lambda>()(std::declval<RequiredArgs>()...))>
        > : std::true_type {};

        template <class Ret, class ...Args, class ...RequiredArgs>
        struct is_callable_with_impl
        <Ret(*)(Args...), pack<RequiredArgs...>, 
            std::void_t<decltype(std::declval<Ret(*)(Args...)>()(std::declval<RequiredArgs>()...))>
        > : std::true_type {};

        template <class Ret, class ...Args, class PackedRequiredArgs>
        struct is_callable_with_impl<Ret(Args...), PackedRequiredArgs>
            : is_callable_with_impl<Ret(*)(Args...), PackedRequiredArgs> {};

        template <class Ret, class C, class ...Args, class ...RequiredArgs>
        struct is_callable_with_impl
        <Ret(C::*)(Args...), pack<RequiredArgs...>,
            std::void_t<decltype(
                (std::declval<C>().*std::declval<Ret(C::*)(Args...)>())(std::declval<RequiredArgs>()...)
            )>
        > : std::true_type {};

        template <class Ret, class C, class ...Args, class ...RequiredArgs>
        struct is_callable_with_impl
        <Ret(C::*)(Args...) const, pack<RequiredArgs...>,
            std::void_t<decltype(
                (std::declval<C>().*std::declval<Ret(C::*)(Args...)>())(std::declval<RequiredArgs>()...)
            )>
        > : std::true_type {};

        template <class Ret, class C, class ...Args, class ...RequiredArgs>
        struct is_callable_with_impl
        <Ret(C::*)(Args...) const noexcept, pack<RequiredArgs...>,
            std::void_t<decltype(
                (std::declval<C>().*std::declval<Ret(C::*)(Args...)>())(std::declval<RequiredArgs>()...)
            )>
        > : std::true_type {};

        template <class>
        struct is_callable_impl;

        template <class, class = std::void_t<>>
        struct diverge_lambda {
            using type = std::false_type;
        };

        template <class Lambda>
        struct diverge_lambda<Lambda, std::void_t<decltype(&Lambda::operator())>> {
            using type = is_callable_impl<decltype(&Lambda::operator())>;
        };

        template <class Lambda>
        struct is_callable_impl : diverge_lambda<Lambda>::type {};

        template <class Ret, class ...Args>
        struct is_callable_impl<Ret(*)(Args...)> : is_callable_with_impl<Ret(*)(Args...), pack<Args...>> {};

        template <class Ret, class ...Args>
        struct is_callable_impl<Ret(Args...)> : is_callable_with_impl<Ret(*)(Args...), pack<Args...>> {};

        template <class Ret, class C, class ...Args>
        struct is_callable_impl<Ret(C::*)(Args...)> : is_callable_with_impl<Ret(C::*)(Args...), pack<Args...>> {};

        template <class Ret, class C, class ...Args>
        struct is_callable_impl<Ret(C::*)(Args...) const> : is_callable_with_impl<Ret(C::*)(Args...), pack<Args...>> {};

        template <class, class, class = std::void_t<>>
        struct noexception_with_impl {};

        template <class Func, class ...RequiredArgs>
        struct noexception_with_impl
        <Func, pack<RequiredArgs...>,
            std::void_t<decltype(std::declval<Func>()(std::declval<RequiredArgs>()...))>
        >
        {
            static constexpr bool value = noexcept(
                std::declval<Func>()(std::declval<RequiredArgs>()...)
            );
        };

        template <class Ret, class C, class ...Args, class ...RequiredArgs>
        struct noexception_with_impl
        <Ret(C::*)(Args...), pack<RequiredArgs...>,
            std::void_t<decltype((std::declval<C>().*std::declval<Ret(C::*)(Args...)>())(std::declval<RequiredArgs>()...))>
        >
        {
            static constexpr bool value = noexcept(
                (std::declval<C>().*std::declval<Ret(C::*)(Args...)>())(std::declval<RequiredArgs>()...)
            );
        };

        template <class Lambda>
        struct noexception_impl : noexception_impl<decltype(&Lambda::operator())> {};

        template <class Ret, class ...Args>
        struct noexception_impl<Ret(*)(Args...)> : noexception_with_impl<Ret(*)(Args...), pack<Args...>> {};

        template <class Ret, class ...Args>
        struct noexception_impl<Ret(Args...)> : noexception_with_impl<Ret(*)(Args...), pack<Args...>> {};

        template <class Ret, class C, class ...Args>
        struct noexception_impl<Ret(C::*)(Args...)> : noexception_with_impl<Ret(C::*)(Args...), pack<Args...>> {};

        template <class Ret, class C, class ...Args>
        struct noexception_impl<Ret(C::*)(Args...) const> : std::false_type {};

        template <class Ret, class C, class ...Args>
        struct noexception_impl<Ret(C::*)(Args...) const noexcept> : std::true_type {};
    };
public:
    template <class Func, class RequiredArgsHead, class ...RequiredArgsTail>
    struct is_callable_with
        : utility::is_callable_with_impl<typename utility::remove_modification<Func>::type, utility::pack<RequiredArgsHead, RequiredArgsTail...>> {};

    template <class Func>
    struct is_callable
        : utility::is_callable_impl<typename utility::remove_modification<Func>::type> {};

    template <class Func, class RequiredArgsHead, class ...RequiredArgs>
    struct noexception_with
        : utility::noexception_with_impl<typename utility::remove_modification<Func>::type, utility::pack<RequiredArgsHead, RequiredArgs...>> {};

    template <class Func>
    struct noexception
        : utility::noexception_impl<typename utility::remove_modification<Func>::type> {};
};
