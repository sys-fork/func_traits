#include <type_traits>

namespace func_traits_impl {
    template <class ...>
    struct pack {};
}

template <class T>
struct func_traits {};

template <class Ret, class ...Args>
struct func_traits<Ret(Args...)> {
private:

    template <std::size_t, class>
    struct Args_N {};

    template <std::size_t N, class Head, class ...Tail>
    struct Args_N<N, func_traits_impl::pack<Head, Tail...>>
        : public Args_N<N - 1, func_traits_impl::pack<Tail...>> {};

    template <class Head, class ...Tail>
    struct Args_N<0, func_traits_impl::pack<Head, Tail...>> {
        using type = Head;
    };

    template <std::size_t, class>
    struct length;

    template <std::size_t N, class Head, class ...Tail>
    struct length<N, func_traits_impl::pack<Head, Tail...>>
        : public length<N + 1, func_traits_impl::pack<Tail...>> {};

    template <std::size_t N>
    struct length<N, func_traits_impl::pack<>> {
        static constexpr std::size_t value = N;
    };

    template <class, class, class = std::void_t<>>
    struct func_validation_impl : std::false_type {};

    template <class Func_type, class ...Argsvalid>
    struct func_validation_impl<Func_type, func_traits_impl::pack<Argsvalid...>, std::void_t<
        decltype(std::declval<Func_type *>()(std::declval<Argsvalid>()...)
    )>> : std::true_type {};

    template <class Func_type>
    struct func_validation
        : func_validation_impl<Func_type, typename func_traits<Func_type>::args_pack> {};
public:
    using ret = Ret;
    static constexpr std::size_t len = length<0, func_traits_impl::pack<Args...>>::value;
    static constexpr std::size_t len1 = sizeof...(Args);
    template <std::size_t N>
    using args = typename Args_N<N, func_traits_impl::pack<Args...>>::type;
    using args_pack = func_traits_impl::pack<Args...>;
    static constexpr bool validation = func_validation<Ret(Args...)>::value;
};

