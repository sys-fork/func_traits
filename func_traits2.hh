#include <utility>
#include <functional>

struct func_traits {
    struct utility {
        template <class ...>
        struct pack {};
    
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
    };

    template <class, class ...>
    struct function_impl;

    template <class Ret, class ...Args>
    struct function_impl<std::function<Ret(Args...)>> : public utility {
        using ret = Ret;
        static constexpr std::size_t len = sizeof...(Args);
        template <std::size_t N>
        using args = typename Args_N<N, pack<Args...>>::type;
        using args_pack = pack<Args...>;
    };

    template <class Pred>
    struct function : public function_impl<
        decltype(std::function(std::declval<Pred>()))
    > {};
};

