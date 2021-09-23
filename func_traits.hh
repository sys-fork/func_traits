#include <type_traits>
#include <functional>
#include <tuple>

template <class T>
struct func_traits {};

template <class Ret, class ...Args>
struct func_traits<Ret(Args...)> {
private:
    template <class ...>
    struct pack {};

    template <std::size_t, class>
    struct Args_N {};

    template <std::size_t N, class Head, class ...Tail>
    struct Args_N<N, pack<Head, Tail...>> : public Args_N<N - 1, pack<Tail...>> {};

    template <class Head, class ...Tail>
    struct Args_N<0, pack<Head, Tail...>> {
        using type = Head;
    };

    template <std::size_t, class>
    struct length;

    template <std::size_t N, class Head, class ...Tail>
    struct length<N, pack<Head, Tail...>> : public length<N + 1, pack<Tail...>> {};

    template <std::size_t N>
    struct length<N, pack<>> {
        static constexpr std::size_t value = N;
    };
/*
    template <class, template <class ...> class, class = std::void_t<>>
    struct func_validation_impl : std::false_type {};

    template <class Func_type, template <class ...Argsvalid> class pack>
    struct func_validation_impl<Func_type, std::void_t<
        decltype(std::declval<Func_type *>()(std::declval<Argsvalid>()...)
    ))>> : std::true_type {};

    template <class Func_type>
    struct func_validation
        : func_validation_impl<Func_type, func_traits<Func_type>::args_pack> {}; */
public:
    using ret = Ret;
    static constexpr std::size_t len = length<0, pack<Args...>>::value;
    static constexpr std::size_t len1 = sizeof...(Args);
    template <std::size_t N>
    using args = typename Args_N<N, pack<Args...>>::type;
    using args_pack = pack<Args...>;
    // static constexpr bool validation = func_validation<Func_type>::value;
};

