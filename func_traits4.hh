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

        template <class, class, class = std::void_t<>>
        struct noexception_impl_impl {};

        template <class Func, class ...RequiredArgs>
        struct noexception_impl_impl
        <Func, pack<RequiredArgs...>,
            std::void_t<decltype(std::declval<Func>()(std::declval<RequiredArgs>()...))>
        >
        {
            static constexpr bool value = noexcept(
                std::declval<Func>()(std::declval<RequiredArgs>()...)
            );
        };

        template <class Ret, class C, class ...Args, class ...RequiredArgs>
        struct noexception_impl_impl
        <Ret(C::*)(Args...), pack<RequiredArgs...>,
            std::void_t<decltype((std::declval<C>().*std::declval<Ret(C::*)(Args...)>())(std::declval<RequiredArgs>()...))>
        >
        {
            static constexpr bool value = noexcept(
                (std::declval<C>().*std::declval<Ret(C::*)(Args...)>())(std::declval<RequiredArgs>()...)
            );
        };

        template <class Func, class ...RequiredArgs>
        struct noexception_impl : noexception_impl_impl<Func, pack<RequiredArgs...>> {};

        template <class Lambda>
        struct noexception_impl<Lambda> : noexception_impl<decltype(&Lambda::operator())> {}; // (1)
        // このあと (2) に解決されるはずだがどうやら再び (1) に解決されているようだ
        // この現象はラムダ式を noexcept 指定したときにだけ起こる

        template <class Ret, class ...Args>
        struct noexception_impl<Ret(*)(Args...)> : noexception_impl_impl<Ret(*)(Args...), pack<Args...>> {};

        template <class Ret, class C, class ...Args>
        struct noexception_impl<Ret(C::*)(Args...)> : noexception_impl_impl<Ret(C::*)(Args...), pack<Args...>> {};

        template <class Ret, class C, class ...Args>
        struct noexception_impl<Ret(C::*)(Args...) const> : noexception_impl_impl<Ret(C::*)(Args...), pack<Args...>> {}; // (2)
    };
public:
    template <class Func, class ...RequiredArgs>
    struct is_callable_with
        : utility::is_callable_with_impl<typename utility::remove_modification<Func>::type, utility::pack<RequiredArgs...>> {};

    template <class Func, class ...RequiredArgs>
    struct noexception
        : utility::noexception_impl<typename utility::remove_modification<Func>::type, RequiredArgs...> {};
};
