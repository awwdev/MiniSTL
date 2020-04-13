#pragma once
#include <numeric>
#include <cstdint>

namespace mini
{
    //shorthand fixed sizes

    using u64 = std::uint64_t;
    using i64 = std::int64_t;
    using u32 = std::uint32_t;
    using i32 = std::int32_t;
    using u16 = std::uint16_t;
    using i16 = std::int16_t;
    using u8  = std::uint8_t;
    using i8  = std::int8_t;

    using f64 = double;
    using f32 = float;

    constexpr auto u16max = std::numeric_limits<u16>::max();
}