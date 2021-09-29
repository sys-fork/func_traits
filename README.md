## 関数や関数オブジェクトの特性を取得するメタ関数

C++17 に対応している．

### 定義
```cpp
template <class Func, class ...RequiredArgs>
struct fntraits::is_callable_with;

template <class Func, class ...RequiredArgs>
struct fntraits::noexception;
```

特性を調べたい関数，ラムダ式，ジェネリックラムダ式
がそれぞれ
```cpp
void f(int, int) {}
auto lambda = [](int, int){};
auto generic_lambda = [](auto, auto){};
```
であるとする．

### 指定の引数で呼び出し可能か調べる
```cpp
bool is_f_callable_with = fntraits::is_callable_with<decltype(f), int, int>::value;
bool is_lambda_callable_with
    = fntraits::is_callable_with<decltype(lambda), int, int>::value;
bool is_generic_lambda_callable_with
    = fntraits::is_callable_with<decltype(generic_lambda), int, int>::value;
```

### 例外を送出するかしらべる
```cpp
bool is_f_safe = fntraits::noexception<decltype(f)>::value;
bool is_lambda_safe_with
    = fntraits::noexception<decltype(lambda), int, int>::value;
bool is_generic_lambda_safe_with
    = fntraits::noexception<decltype(generic_lambda), int, int>::value;
```

### 使用例
今まで
```cpp
template <class Func, class ...Args>
decltype(auto) forward(Func func, Args&&... args)
    -> noexcept(noexcept(func(std::forward<Args>(args)...)))
{
    return func(std::forward<Args>(args)...);
}
```
と書いてきたコードは
```cpp
template <class Func, class ...Args>
decltype(auto) forward(Func func, Args&&... args)
    -> noexcept(fntraits::noexception_v<Func, Args...>)
{
    return func(std::forward<Args>(args)...);
}
```
と書くことが出来ます．

また，以下のような SFINAE を用いた複雑なコードを簡単に実現できます．
```cpp
template <class Func1, class Func2, class ...Args>
auto call_if(Func1 func1, Func2 func2, Args&&... args)
    -> typename std::enable_if<
        fntraits::is_callable_with_v<Func1, Args...>
        && !fntraits::is_callable_with_v<Func2, Args...>,
        fntraits::ret_type_t<Func1, Args...>
    >::type
{
    return func1(std::forward<Args>(args)...);
}

template <class Func1, class Func2, class ...Args>
auto call_if(Func1 func1, Func2 func2, Args&&... args)
    -> typename std::enable_if<
        !fntraits::is_callable_with_v<Func1, Args...>
        && fntraits::is_callable_with_v<Func2, Args...>,
        fntraits::ret_type_t<Func2, Args...>
    >::type
{
    return func2(std::forward<Args>(args)...);
}
```
