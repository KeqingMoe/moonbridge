#pragma once
#ifndef TERU_MOONBRIDGE_HPP
#define TERU_MOONBRIDGE_HPP

#if __cplusplus < 202302L
#error "需要 C++23 或更高版本"
#endif

#include <concepts>
#include <memory>
#include <type_traits>

#include <moonbit.h>

// Moonbit Binding
namespace mbt
{


// Moonbit 基本 C FFI
namespace ffi
{

// 非公共 API，不稳定，直接来自于 Moonbit 编译结果
namespace unsupported
{

extern "C"
{
    MOONBIT_EXPORT _Noreturn void moonbit_panic(void);


    inline constexpr auto make_regular_object_header(std::uint32_t ptr_field_offset,
                                                     std::uint32_t ptr_field_count,
                                                     std::uint32_t tag) -> std::uint32_t
    {
        return (((uint32_t)moonbit_BLOCK_KIND_REGULAR << 30)
                | (((uint32_t)(ptr_field_offset) & (((uint32_t)1 << 11) - 1)) << 19)
                | (((uint32_t)(ptr_field_count) & (((uint32_t)1 << 11) - 1)) << 8) | ((tag) & 0xFF));
    }
}

}

[[noreturn]] inline constexpr auto panic() -> void
{
    unsupported::moonbit_panic();
}

using object               = moonbit_object;
using valtype_array_header = moonbit_valtype_array_header;
using string               = char16_t*;
using bytes                = std::byte*;
using finalizer            = void (*)(void*);

inline constexpr auto object_tag(void* obj) -> std::uint32_t
{
    return Moonbit_object_tag(obj);
}

inline constexpr auto object_header(void* obj) -> object*
{
    return Moonbit_object_header(obj);
}

inline constexpr auto array_length(void* obj) -> std::uint32_t
{
    return Moonbit_array_length(obj);
}

inline constexpr auto malloc(std::size_t size) -> void*
{
    return moonbit_malloc(size);
}

inline constexpr auto incref(void* obj) -> void
{
    moonbit_incref(obj);
}

inline constexpr auto decref(void* obj) -> void
{
    moonbit_decref(obj);
}

inline constexpr auto make_string(std::int32_t size, char16_t value) -> string
{
    auto s = moonbit_make_string(size, value);
    return static_cast<string>(static_cast<void*>(s));
}

inline constexpr auto make_string_raw(std::int32_t size) -> string
{
    auto s = moonbit_make_string_raw(size);
    return static_cast<string>(static_cast<void*>(s));
}

inline constexpr auto make_external_object(finalizer finalize, std::uint32_t payload_size) -> void*
{
    return moonbit_make_external_object(finalize, payload_size);
}

static const object constant_constructor_0 = {
    .rc   = -1,
    .meta = unsupported::make_regular_object_header(2, 0, 0),
};

}

// ABI
namespace abi
{

template <typename T>
concept c = std::is_arithmetic_v<T> || (std::is_aggregate_v<T> && std::is_trivially_assignable_v<T, T>);

template <typename T>
concept cpp = !c<T>;

constexpr auto pointer_size = sizeof(void*);

template <typename T>
concept moonbit = abi::c<T> && sizeof(T) <= abi::pointer_size;

}

// moonbridge 类型绑定
inline namespace types
{

// Moonbit Option ABI
enum struct option_kind
{
    // 不到 32bits
    i32,
    // 不到 64bits 并且底层表示是整数
    i64,
    // >= 64bits、浮点数或 opaque 类型
    ref,
    // 仅 Moonbit 侧类型
    ptr,
};

template <abi::moonbit T>
struct moonbit_trait
    // {
    //     using repr_type = _;
    //     OPTION_KIND(_);
    //
    //     auto retain () noexcept -> void;
    //     auto release() noexcept -> void;
    // }
    ;

template <typename T>
concept moonbit = requires(T t) {
    typename moonbit_trait<T>::repr_type;
    { moonbit_trait<T>::option_kind } -> std::convertible_to<option_kind>;
    { moonbit_trait<T>::trait_boxed } -> std::convertible_to<bool>;

    { moonbit_trait<T>::retain(t) } -> std::same_as<void>;
    { moonbit_trait<T>::release(t) } -> std::same_as<void>;
};

#define impl_moonbit(P, T) template P struct moonbit_trait<T>
#define OPTION_KIND(tag) static constexpr auto option_kind = option_kind::tag
#define TRAIT_BOXED(flag) static constexpr auto trait_boxed = flag
#define NO_RC(Self)                                                                                                    \
    static constexpr auto retain(Self) noexcept -> void {}                                                             \
    static constexpr auto release(Self) noexcept -> void {}
#define RC(Self)                                                                                                       \
    static constexpr auto retain(Self self) noexcept -> void                                                           \
    {                                                                                                                  \
        ffi::incref(self.repr);                                                                                        \
    }                                                                                                                  \
    static constexpr auto release(Self self) noexcept -> void                                                          \
    {                                                                                                                  \
        ffi::decref(self.repr);                                                                                        \
    }

#define NEWTYPE(Self, Repr)                                                                                            \
    using repr_type = Repr;                                                                                            \
    repr_type repr;

#define NEWTYPE_WITH_CTOR(Self, Repr, From)                                                                            \
    NEWTYPE(Self, Repr)                                                                                                \
    static constexpr auto make(From r) noexcept -> Self                                                                \
    {                                                                                                                  \
        return {.repr = r};                                                                                            \
    }                                                                                                                  \
    // explicit constexpr operator From(this Self self) noexcept \
    // { \
    //     return self.repr; \
    // }

#define NEWTYPE_OF(Self, Repr) NEWTYPE_WITH_CTOR(Self, Repr, Repr)

#define CONTAINER_OF(T)                                                                                                \
    using value_type      = T;                                                                                         \
    using size_type       = Int;                                                                                       \
    using difference_type = Int;                                                                                       \
    using pointer         = value_type*;                                                                               \
    using iterator        = pointer;                                                                                   \
    using reference       = value_type&;                                                                               \
    using const_pointer   = const value_type*;                                                                         \
    using const_reference = const value_type&;                                                                         \
    using const_iterator  = const_pointer;                                                                             \
    auto begin(this Self self) -> iterator                                                                             \
    {                                                                                                                  \
        return self.data();                                                                                            \
    }                                                                                                                  \
    auto end(this Self self) -> iterator                                                                               \
    {                                                                                                                  \
        return self.begin() + self.size();                                                                             \
    }                                                                                                                  \
    auto operator[](this Self self, Int index) noexcept -> reference                                                   \
    {                                                                                                                  \
        if (index < 0 || index > self.size()) {                                                                        \
            ffi::panic();                                                                                              \
        }                                                                                                              \
        return self.repr[index];                                                                                       \
    }

// 基本类型和外部类型引用
inline namespace basic
{

struct Unit
{
    NEWTYPE(Unit, std::int32_t);
};

struct Bool
{
    NEWTYPE_WITH_CTOR(Bool, std::int32_t, bool);
};

inline constexpr auto False = Bool::make(false);
inline constexpr auto True  = Bool::make(true);

using Byte   = std::uint8_t;
using Int    = std::int32_t;
using UInt   = std::uint32_t;
using Int64  = std::int64_t;
using UInt64 = std::uint64_t;
using Float  = float;
using Double = double;

struct String
{
    using Self = String;
    CONTAINER_OF(char16_t)
    using repr_type = pointer;
    repr_type repr;

    static auto from_raw(const_pointer s, size_type count) -> Self
    {
        auto str = pointer{};
        if (s) {
            str = ffi::make_string_raw(count);
            for (auto i = size_type{}; i < count; ++i) {
                str[i] = s[i];
            }
        } else {
            str = ffi::make_string(0, char16_t{});
        }
        return {.repr = str};
    }

    template <size_type N>
    static auto from(const char16_t (&s)[N]) -> Self
    {
        return from_raw(s, N - 1);
    }

    static auto from_str(const char16_t* s) -> Self
    {
        auto n = size_type{};
        for (auto p = s; *p != u'\0'; ++p, ++n);
        return from_raw(s, n);
    }

    auto size(this Self self) -> size_type
    {
        return ffi::array_length(self.repr);
    }

    auto data(this Self self) -> pointer
    {
        return self.repr;
    }
};

template <typename T>
inline constexpr auto finalizer_of(void* obj)
{
    auto ptr = static_cast<T*>(obj);
    std::destroy_at(ptr);
}

// 用于将外部类型传递给 Moonbit，底层表示为 void*
template <typename T>
struct box
{
    using Self      = box;
    using repr_type = T*;
    repr_type repr;

    constexpr auto operator->(this Self self) noexcept -> T*
    {
        return self.repr;
    }

    constexpr auto operator*(this Self self) noexcept -> T&
    {
        return *self.repr;
    }

    template <typename... Args>
        requires std::constructible_from<T, Args&&...>
    static auto with_finalizer(ffi::finalizer finalize, Args&&... args) noexcept -> Self
    {
        auto memory = ffi::make_external_object(finalize, sizeof(T));
        auto ptr    = std::construct_at(static_cast<T*>(memory), std::forward<Args>(args)...);
        return {.repr = ptr};
    }

    template <typename... Args>
        requires std::constructible_from<T, Args&&...>
    static auto make(Args&&... args) noexcept -> Self
    {
        auto memory = ffi::make_external_object(finalizer_of<T>, sizeof(T));
        auto ptr    = std::construct_at(static_cast<T*>(memory), std::forward<Args>(args)...);
        return {.repr = ptr};
    }

    static auto from_raw(T* ptr) noexcept -> Self
    {
        return {.repr = ptr};
    }
};

}

impl_moonbit(<>, Unit)
{
    using repr_type = Unit::repr_type;
    OPTION_KIND(i32);
    TRAIT_BOXED(true);
    NO_RC(Unit);
};

impl_moonbit(<>, Bool)
{
    using repr_type = Bool::repr_type;
    OPTION_KIND(i32);
    TRAIT_BOXED(true);
    NO_RC(Bool)
};

impl_moonbit(<>, Byte)
{
    using repr_type = Byte;
    OPTION_KIND(i32);
    TRAIT_BOXED(true);
    NO_RC(Byte);
};

impl_moonbit(<>, Int)
{
    using repr_type = Int;
    OPTION_KIND(i64);
    TRAIT_BOXED(true);
    NO_RC(Int);
};

impl_moonbit(<>, UInt)
{
    using repr_type = UInt;
    OPTION_KIND(i64);
    TRAIT_BOXED(true);
    NO_RC(UInt);
};

impl_moonbit(<>, Int64)
{
    using repr_type = Int64;
    OPTION_KIND(ref);
    TRAIT_BOXED(true);
    NO_RC(Int64);
};

impl_moonbit(<>, UInt64)
{
    using repr_type = UInt64;
    OPTION_KIND(ref);
    TRAIT_BOXED(true);
    NO_RC(UInt64);
};

impl_moonbit(<>, Float)
{
    using repr_type = Float;
    OPTION_KIND(ref);
    TRAIT_BOXED(true);
    NO_RC(Float);
};

impl_moonbit(<>, Double)
{
    using repr_type = Double;
    OPTION_KIND(ref);
    TRAIT_BOXED(true);
    NO_RC(Double);
};

impl_moonbit(<>, String)
{
    using repr_type = String::repr_type;
    OPTION_KIND(ptr);
    TRAIT_BOXED(false);
    NO_RC(String);
};

impl_moonbit(<typename T>, box<T>)
{
    using repr_type = box<T>::repr_type;
    OPTION_KIND(ref);
    TRAIT_BOXED(true);
    RC(box<T>);
};

inline namespace compound
{

template <moonbit T>
struct Ref
{
    using Self      = Ref;
    using repr_type = T*;
    repr_type repr;

    constexpr auto operator->(this Self self) noexcept -> T*
    {
        return self.repr;
    }

    constexpr auto operator*(this Self self) noexcept -> T&
    {
        return *self.repr;
    }

    static auto make(T x) noexcept -> Self
    {
        auto size    = sizeof(Self) / 4;
        auto mem     = ffi::malloc(size);
        auto header  = ffi::object_header(mem);
        header->meta = ffi::unsupported::make_regular_object_header(size, 0, 0);
        auto obj     = std::construct_at(static_cast<T*>(mem), x);
        return {.repr = obj};
    }
};

template <moonbit T>
struct FixedArray
{
    using Self = FixedArray;
    CONTAINER_OF(T)
    using repr_type = pointer;
    repr_type repr;

    auto size(this Self self) -> size_type
    {
        return ffi::array_length(self.repr);
    }

    auto data(this Self self) -> pointer
    {
        return self.repr;
    }
};

template <moonbit T>
struct Array
{
    using Self = Array;
    CONTAINER_OF(T)
    struct underlying
    {
        size_type size;
        pointer data;
    };
    using repr_type = underlying*;
    repr_type repr;

    auto size(this Self self) -> size_type
    {
        return self.repr->size;
    }

    auto data(this Self self) -> pointer
    {
        return self.repr->data;
    }
};

// template <moonbit T>
// using option_repr =
//     std::conditional_t<moonbit_trait<T>::option_kind == option_kind::i32,
//                        Int,
//                        std::conditional_t<moonbit_trait<T>::option_kind == option_kind::i64, Int64, void*>>;

template <moonbit T>
struct Option
{
    using Self  = Option;
    using trait = moonbit_trait<T>;
    // using repr_type = option_repr<T>;
    using repr_type =
        std::conditional_t<moonbit_trait<T>::option_kind == option_kind::i32,
                           Int,
                           std::conditional_t<moonbit_trait<T>::option_kind == option_kind::i64, Int64, void*>>;
    repr_type repr;

    static constexpr auto none() -> Self
    {
        if constexpr (trait::option_kind == option_kind::i32) {
            return {.repr = -1};
        } else if constexpr (trait::option_kind == option_kind::i64) {
            return {.repr = 4294967296ll};
        } else if constexpr (trait::option_kind == option_kind::ref) {
            return {.repr = const_cast<ffi::object*>(&ffi::constant_constructor_0) + 1};
        } else {
            return {.repr = nullptr};
        }
    }

    static constexpr auto some(T x) -> Self
    {
        if constexpr (trait::option_kind == option_kind::ptr) {
            return {.repr = x.repr};
        } else if constexpr (trait::option_kind == option_kind::ref) {
            auto mem     = ffi::malloc(sizeof(T));
            auto header  = ffi::object_header(mem);
            header->meta = ffi::unsupported::make_regular_object_header(sizeof(T) / 4, 0, 1);
            auto obj     = std::construct_at(static_cast<T*>(mem), x);
            return {.repr = obj};
        } else {
            return {.repr = x};
        }
    }

    constexpr auto is_none(this Self self) noexcept -> bool
    {
        if constexpr (trait::option_kind == option_kind::i32) {
            return self.repr == -1;
        } else if constexpr (trait::option_kind == option_kind::i64) {
            return self.repr == 4294967296ll;
        } else if constexpr (trait::option_kind == option_kind::ref) {
            return ffi::object_tag(self.repr) == 0;
        } else {
            return self.repr == 0;
        }
    }

    constexpr auto is_some(this Self self) noexcept -> bool
    {
        return !self.is_none();
    }
};

template <typename...>
struct fn;

template <moonbit R, moonbit... Args>
struct fn<R(Args...)>
{
    using Self = fn;

    struct underlying
    {
        using Self      = underlying;
        using context   = underlying*;
        using code_type = R (*)(Self*, Args...) noexcept;
        code_type code;
    };

    using code_type = underlying::code_type;
    using context   = underlying::context;
    using repr_type = underlying*;
    repr_type repr;

    auto operator()(this Self self, Args... args) noexcept -> R // 猗露说这里不用 &&
    {
        moonbit_trait<Self>::retain(self);
        (..., moonbit_trait<Args>::retain(args));
        code_type code = self.repr->code;
        context ctx    = self.repr;
        return code(ctx, args...);
    }
};

// Hack MoonBit Trait Object Fat Pointer 的设施
// 仅用于 FFI 边界（私有 API）上的类型擦除技巧
// 需要确保 T 就是 MoonBit 侧公共 API 中相同的类型
template <moonbit T>
struct fat
{
    using Self                        = fat;
    static constexpr auto trait_boxed = moonbit_trait<T>::trait_boxed;
    struct underlying
    {
        using object_type = std::conditional_t<trait_boxed, T*, T>;
        void* vtable;
        object_type object;
    };
    using repr_type = underlying;
    repr_type repr;

    constexpr auto operator->(this Self self) noexcept -> T*
    {
        if constexpr (trait_boxed) {
            return self.repr.object;
        } else {
            return &self.repr.object;
        }
    }

    constexpr auto operator*(this Self self) noexcept -> T
    {
        if constexpr (trait_boxed) {
            return *self.repr.object;
        } else {
            return self.repr.object;
        }
    }
};

}

impl_moonbit(<moonbit T>, Ref<T>)
{
    using repr_type = Ref<T>::repr_type;
    OPTION_KIND(ptr);
    TRAIT_BOXED(false);
    RC(Ref<T>);
};

impl_moonbit(<moonbit T>, FixedArray<T>)
{
    using repr_type = FixedArray<T>::repr_type;
    OPTION_KIND(ptr);
    TRAIT_BOXED(false);
    RC(FixedArray<T>);
};

impl_moonbit(<moonbit T>, Array<T>)
{
    using repr_type = Array<T>::repr_type;
    OPTION_KIND(ref);
    TRAIT_BOXED(false);
    RC(Array<T>);
};

impl_moonbit(<moonbit T>, Option<T>)
{
    using repr_type = Option<T>::repr_type;
    OPTION_KIND(ref);
    TRAIT_BOXED((moonbit_trait<T>::option_kind == option_kind::i32)
                || moonbit_trait<T>::option_kind == option_kind::i64);
    RC(Option<T>);
};

impl_moonbit(<typename F>, fn<F>)
{
    using repr_type = fn<F>::repr_type;
    OPTION_KIND(ref);
    TRAIT_BOXED(false);
    RC(fn<F>);
};

// template <moonbit R, moonbit... Args>
// struct moonbit_trait<fn<R(Args...)>>
// {
//     using repr_type = fn<R(Args...)>::repr_type;
//     OPTION_KIND(ref);
//     RC(fn<R(Args...)>);
// };

inline namespace persistent
{

// 它用于持久化存储，不对应任何 Moonbit 类型
// 没有移动语义，我不确定它是否与 Moonbit 对象模型相容，并且害怕空壳被误用
template <moonbit T>
struct own
{
    using Self      = own<T>;
    using trait     = moonbit_trait<T>;
    using repr_type = T;
    repr_type repr;

    explicit own(T r) noexcept : repr(r)
    {
        trait::retain(this->repr);
    }

    own(Self const& other) noexcept
    {
        this->repr = other.repr;
        trait::retain(this->repr);
    }

    ~own() noexcept
    {
        trait::release(this->repr);
    }

    auto operator=(Self const& other) noexcept -> Self&
    {
        if (this == &other) {
            return *this;
        }
        trait::release(this->repr);
        this->repr = other.repr;
        trait::retain(this->repr);
        return *this;
    }
};

}

#undef CONTAINER_OF

#undef NEWTYPE_OF
#undef NEWTYPE_WITH_CTOR
#undef NEWTYPE

#undef RC
#undef NO_RC
#undef OPTION_KIND
#undef impl_moonbit

}

}

#endif
