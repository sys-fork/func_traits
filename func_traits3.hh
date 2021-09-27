#include <utility>
#include <type_traits>

namespace fntraits {
    struct utility {
        template <class ...>
        struct pack {};

    protected:
        template <class, class, class = std::void_t<>>
        struct func_validation : std::false_type {};

        template <class Lambda, class ...RequiredArgs>
        struct func_validation
        <Lambda, pack<RequiredArgs...>,
            std::void_t<decltype(std::declval<Lambda>()(std::declval<RequiredArgs>()...))>
        > : std::true_type {};
        
        template <class Ret, class ...Args, class ...RequiredArgs>
        struct func_validation
        <Ret(Args...), pack<RequiredArgs...>, 
            std::void_t<decltype(std::declval<Ret(*)(Args...)>()(std::declval<RequiredArgs>()...))>
        > : std::true_type {};

        template <class Ret, class C, class ...Args, class ...RequiredArgs>
        struct func_validation
        <Ret(C::*)(Args...), pack<RequiredArgs...>,
            std::void_t<decltype(
                (std::declval<C>().*std::declval<Ret(C::*)(Args...)>())(std::declval<RequiredArgs>()...)
            )>
        > : std::true_type {};

        template <class Lambda, class ...Args>
        struct func_exception {
            static constexpr bool value = noexcept(std::declval<Lambda>()(std::declval<Args>()...));
        };

        template <class Ret, class ...Args, class ...RequiredArgs>
        struct func_exception<Ret(Args...), RequiredArgs...>
            : func_exception<Ret(*)(Args...), RequiredArgs...> {};

        template <class Ret, class C, class ...Args, class ...RequiredArgs>
        struct func_exception<Ret(C::*)(Args...), RequiredArgs...> {
            static constexpr bool value
                = noexcept(
                    (std::declval<C>().*std::declval<Ret(C::*)(Args...)>())(std::declval<RequiredArgs>()...)
                );
        };

        template <class Func, class ...RequiredArgs>
        struct func_decltype {
            using type = decltype(std::declval<Func>()(std::declval<RequiredArgs>()...));
        };

        template <class Ret, class ...Args, class ...RequiredArgs>
        struct func_decltype<Ret(Args...), RequiredArgs...> : func_decltype<Ret(*)(Args...), RequiredArgs...> {};

        template <class Ret, class C, class ...Args, class ...RequiredArgs>
        struct func_decltype<Ret(C::*)(Args...), RequiredArgs...> {
            using type = decltype(
                (std::declval<C>().*std::declval<Ret(C::*)(Args...)>())(std::declval<RequiredArgs>()...)
            );
        };

        template <std::size_t, class>
        struct Args_N {};

        template <std::size_t N, class Head, class ...Tail>
        struct Args_N<N, pack<Head, Tail...>> : Args_N<N - 1, pack<Tail...>> {};

        template <class Head, class ...Tail>
        struct Args_N<0, pack<Head, Tail...>> {
            using type = Head;
        };
    };

    template <class Func, class ...RequiredArgs>
    struct inspect_impl : virtual protected utility {
        static constexpr bool validation = func_validation<Func, pack<RequiredArgs...>>::value;
        static constexpr bool exception = func_exception<Func, RequiredArgs...>::value;
        using ret = typename func_decltype<Func, RequiredArgs...>::type;
    };

    template <class Lambda>
    struct inspect_impl<Lambda> : inspect_impl<decltype(&Lambda::operator())> {};

    template <class Ret, class ...Args>
    struct inspect_impl<Ret(Args...)> : virtual protected utility {
        static constexpr bool exception = func_exception<Ret(Args...), Args...>::value;
        using ret = Ret;
        static constexpr std::size_t len = sizeof...(Args);
        template <std::size_t N>
        using arg_type = typename Args_N<N, pack<Args...>>::type;
        using args_pack = pack<Args...>;
    };

    template <class Ret, class ...Args>
    struct inspect_impl<Ret(*)(Args...)> : inspect_impl<Ret(Args...)> {};

    template <class Ret, class C, class ...Args>
    struct inspect_impl<Ret(C::*)(Args...)> : inspect_impl<Ret(Args...)>, virtual protected utility {
        static constexpr bool exception = func_exception<Ret(C::*)(Args...), Args...>::value;
    };

    template <class Ret, class C, class ...Args>
    struct inspect_impl<Ret(C::*)(Args...) const> : inspect_impl<Ret(C::*)(Args...)> {};

    template <class T>
    struct remove_modification {
        using type = T;
    };

    template <class T>
    struct remove_modification<T&> : remove_modification<T> {};

    template <class T>
    struct remove_modification<T&&> : remove_modification<T> {};

    template <class T>
    struct remove_modification<T const *> : remove_modification<T> {};

    template<class T>
    struct remove_modification<const T> : remove_modification<T> {};

    template <class T>
    struct remove_modification<T **> : remove_modification<T> {};

    template <class Func, class ...RequiredArgs>
    struct inspect
        : inspect_impl<typename remove_modification<Func>::type, RequiredArgs...> {};

    template <class Func, class ...RequiredArgs>
    static constexpr bool validation = inspect<Func, RequiredArgs...>::validation;

    template <class Func, class ...RequiredArgs>
    static constexpr bool exception = inspect<Func, RequiredArgs...>::exception;
    
    template <class Func>
    static constexpr std::size_t len = inspect<Func>::len;
    
    template <class Func, class ...RequiredArgs>
    using ret_type = typename inspect<Func, RequiredArgs...>::ret;

    template <class Func, std::size_t N>
    using arg_type = typename inspect<Func>::template arg_type<N>;

    template <class Func>
    using args_pack = typename inspect<Func>::args_pack;
}

