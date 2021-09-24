#include <type_traits>

struct func_traits_utils {
    template <class ...>
    struct pack {
        virtual ~pack() = 0;
    };

    template <class, class, class = std::void_t<>>
    struct func_validation_impl : std::false_type {};

    template <class Pred, class ...Args>
    struct func_validation_impl<Pred, pack<Args...>,
        std::void_t<decltype(std::declval<Pred>()(std::declval<Args>...))>>
        : std::true_type {};

    template <class Pred, class ...Args>
    struct func_validation : func_validation_impl<Pred, pack<Args...>> {};

    template <class Pred, class ...Args>
    struct func_exception {
        static constexpr bool value = noexcept(
            std::declval<Pred>()(std::declval<Args...>)
        );
    };

    template <class Pred, class ...Args>
    struct func_decltype {
        using type = decltype(std::declval<Pred>()(std::declval<Args>...));
    };

    virtual ~func_traits_utils() = 0;
protected:
    template <std::size_t, class>
    struct Args_N {};

    template <std::size_t N, class Head, class ...Tail>
    struct Args_N<N, pack<Head, Tail...>>
        : public Args_N<N - 1, pack<Tail...>> {};

    template <class Head, class ...Tail>
    struct Args_N<0, pack<Head, Tail...>> {
        using type = Head;
    };

    template <std::size_t, class>
    struct length;

    template <std::size_t N, class Head, class ...Tail>
    struct length<N, pack<Head, Tail...>>
        : public length<N + 1, pack<Tail...>> {};

    template <std::size_t N>
    struct length<N, pack<>> {
        static constexpr std::size_t value = N;
    };
/*
    template <class, class, class = std::void_t<>>
    struct func_validation_impl : std::false_type {};

    template <class Func_type, class ...Argsvalid>
    struct func_validation_impl<Func_type, pack<Argsvalid...>, std::void_t<
        decltype(std::declval<Func_type *>()(std::declval<Argsvalid>()...)
    )>> : std::true_type {};

    template <class Func_type>
    struct func_validation
        : func_validation_impl<Func_type, typename func_traits<Func_type>::args_pack> {};
*/
};

template <class Ret, class ...Args>
struct func_traits_nonclosure : public func_traits_utils {
    using ret = Ret;
    static constexpr std::size_t len = length<0, pack<Args...>>::value;
    static constexpr std::size_t len1 = sizeof...(Args);
    template <std::size_t N>
    using args = typename Args_N<N, pack<Args...>>::type;
    using args_pack = pack<Args...>;
    // static constexpr bool validation = func_validation<Ret(*)(Args...)>::value;
    static constexpr bool exception = func_exception<Ret(*)(Args...), Args...>::value;
    virtual ~func_traits_nonclosure() = 0;
};

template <class Ret, class C, class ...Args>
struct func_traits_nonclosure<Ret(C::*)(Args...) const>
    : public func_traits_nonclosure<Ret, Args...> {};

template <class Pred, class ...Args>
struct func_traits_closure : func_traits_utils {
    static constexpr bool validation = func_validation<Pred, Args...>::value;
    static constexpr bool exception = func_exception<Pred, Args...>::value;
    using ret = typename func_decltype<Pred, Args...>::type;
};

template <class Pred, class = std::void_t<>>
struct is_closure : std::true_type {
    using type = Pred;
};

template <class Pred>
struct is_closure<Pred, std::void_t<
    decltype(&Pred::operator())>> : std::false_type {
    using type = decltype(&Pred::operator());
};

template <bool, class>
struct is_closure_func_traits_impl;

template <class Pred>
struct is_closure_func_traits_impl<true, Pred> {
    using type = func_traits_closure<typename is_closure<Pred>::type>;
};

template <class Pred>
struct is_closure_func_traits_impl<false, Pred> {
    using type = func_traits_nonclosure<typename is_closure<Pred>::type>;
};

template <class Pred>
struct is_closure_func_traits
    : is_closure_func_traits_impl<is_closure<Pred>::value, Pred> {};

template <class Pred, class ...Args>
struct func_traits : func_traits_closure<Pred, Args...> {};

template <class Pred>
struct func_traits<Pred> : is_closure_func_traits<Pred>::type {};

template <class Ret, class ...Args>
struct func_traits<Ret(Args...)> : func_traits_nonclosure<Ret, Args...> {};

template <class Ret, class C, class ...Args>
struct func_traits<Ret(C::*)(Args...) const> : func_traits_nonclosure<Ret, Args...> {};

func_traits_utils::~func_traits_utils() {}

template <class ...Ts>
func_traits_utils::pack<Ts...>::~pack() {}

template <class Ret, class ...Args>
func_traits_nonclosure<Ret, Args...>::~func_traits_nonclosure() {}

