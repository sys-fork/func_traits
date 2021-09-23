#include <type_traits>

template <class T>
struct func_traits;

namespace func_traits_impl {
    template <class ...>
    struct pack {};
    
    template <std::size_t, class>
    struct Args_N;

    template <std::size_t N, class Head, class ...Tail>
    struct Args_N<N, pack<Head, Tail...>>
        : public Args_N<N - 1, pack<Tail...>> {};

    template <class Head, class ...Tail>
    struct Args_N<0,pack<Head, Tail...>> {
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

    template <class, class, class = std::void_t<>>
    struct func_validation_impl : std::false_type {};

    template <class Func_type, class ...Argsvalid>
    struct func_validation_impl<Func_type, pack<Argsvalid...>, std::void_t<
        decltype(std::declval<Func_type *>()(std::declval<Argsvalid>()...)
    )>> : std::true_type {};

    template <class Func_type>
    struct func_validation
        : func_validation_impl<Func_type, typename func_traits<Func_type>::args_pack> {};
}

template <class Ret, class ...Args>
struct func_traits<Ret(Args...)> {
public:
    using ret = Ret;
    static constexpr std::size_t len = func_traits_impl::length<0, func_traits_impl::pack<Args...>>::value;
    static constexpr std::size_t len1 = sizeof...(Args);
    template <std::size_t N>
    using args = typename func_traits_impl::Args_N<N, func_traits_impl::pack<Args...>>::type;
    using args_pack = func_traits_impl::pack<Args...>;
    static constexpr bool validation = func_traits_impl::func_validation<Ret(Args...)>::value;
};

