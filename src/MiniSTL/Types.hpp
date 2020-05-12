#pragma once
#include <numeric>
#include <cstdint>
#include <type_traits>
#undef max

namespace mini
{
    //shorthand fixed sizes

    using u64 = std::uint64_t;
    using s64 = std::int64_t;
    using u32 = std::uint32_t;
    using s32 = std::int32_t;
    using u16 = std::uint16_t;
    using s16 = std::int16_t;
    using u8  = std::uint8_t;
    using s8  = std::int8_t;

    using f64 = double;
    using f32 = float;

    constexpr auto u16max = std::numeric_limits<u16>::max();
    constexpr auto u32max = std::numeric_limits<u32>::max();

    //type traits

    template<class T>
    using IsArithmetic = std::enable_if_t<std::is_arithmetic_v<T>>;

    template<class T>
    using IsFloating = std::enable_if_t<std::is_floating_point_v<T>>;

    template<class T>
    using IsIntegralOrEnum = std::enable_if_t<std::is_integral_v<T> || std::is_enum_v<T>>;

    template<class T>
    constexpr auto GetIntegralFromEnum() //transform enum to integral
    {
        if      constexpr (std::is_integral_v<T>) return T{};
        else if constexpr (std::is_enum_v<T>)     return std::underlying_type_t<T>{};
        else    static_assert(0, "STATIC ASSERT type must be integral or enum");
    }

    template<auto N>
    using IntegralTypeEnum = decltype(GetIntegralFromEnum<decltype(N)>()); //shorthand

    template<auto N> //allow signed but not minus 
    using IsArrayIndex = std::enable_if_t<( 
        (std::is_integral_v<decltype(N)> || std::is_enum_v<decltype(N)>) && N >= (decltype(N))0 
    )>;

    template<auto N> //allow signed but not minus or zero 
    using IsArraySize = std::enable_if_t<(
        (std::is_integral_v<decltype(N)> || std::is_enum_v<decltype(N)>) && N > (decltype(N))0 
    )>;

    template<class T>
    using IsPrintable = std::enable_if_t<(
        std::is_arithmetic_v<T> ||
        std::is_pointer_v<T>
    )>;

    template<typename T>
    using IsArray   = std::enable_if_t<std::is_array_v<T>>;

    template<typename T>
    using IsNoArray = std::enable_if_t<!std::is_array_v<T>>;

    template<typename T>
    using IsPointer = std::enable_if_t<std::is_pointer_v<T>>;

}//ns