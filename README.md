## 関数の型から戻り値や引数の型，引数の個数などを取得するためのメタ関数

C++17 に対応している．

### 定義
```cpp
template <class>
struct func_traits;

template <class Ret, class ...Args>
struct func_traits<Ret(Args...)> { /* implementation */ };
```

特性を調べたい関数が
```cpp
fn(a, b, ...)
```
であるとする．なお，`fn` は通常の関数か静的メンバ関数でなければならない．ラムダ式やテンプレート関数には対応していない．

### 戻り値の型を取得する
```cpp
typename func_traits<decltype(fn)>::ret
```

### 引数の個数を取得する
```cpp
func_traits<decltype(fn)>::len
```

### N 番目の引数の型を取得する
```cpp
typename func_traits<decltype(fn)>::template args<N>
```
なお，0番目の引数とは a のことを指す．

### 関数のバリデーションを取得する
```cpp
func_traits<decltype(fn)>::validation
```

### 引数型のリストを取得する
```cpp
typename func_traits<decltype(fn)>::args_pack
```
ここで `decltype(fn)` が `Ret(Args...)` であるとき，上の式の型は
```cpp
func_traits_utils::pack<Args...>
```
となる．ここで pack は
```cpp
template <class ...>
struct pack {};
```
のように定義されたクラスで，任意個の型テンプレートパラメータを
受け取る．

例えばこれは関数の引数の型をテンプレートパラメータリストとして
取得したいときに便利である．すなわち，ある関数に関する
何らかのメタ関数 `metafunc` を実装するにあたって，関数の引数の型をテンプレートパラメータとして取得したいときは，次のようにすればよい．
```cpp
template <class>
struct metafunc_impl;

template <class ...Args>
struct metafunc_impl<func_traits_utils::pack<Args...>> {
    /* do something like this: std::declval<Args>... */
};

template <class Fn>
struct metafunc
    : metafunc_impl<typename func_traits<Fn>::args_pack> {};
```
