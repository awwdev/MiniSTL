//https://github.com/awwdev

#pragma once
#include "com/Utils.hpp"
#include "dbg/Assert.hpp"
#include "dbg/Logger.hpp"
#include "com/mem/Allocator.hpp"

#include <cstddef>
#include <fstream>

namespace rpg::com {

constexpr bool USE_ARRAY_ASSERTS = true;

#define FOR_ARRAY(arr, i) \
for(idx_t i = 0; i < arr.Count(); ++i)

template<typename T, auto N>
struct Array
{
    static constexpr auto COUNT_MAX = static_cast<idx_t>(N);
    static constexpr auto TOTAL_BYTE_SIZE = COUNT_MAX * sizeof(T);
    using TYPE = T;

    //? constructors

    constexpr
    Array() 
        : count { 0 }  
        //leave bytes array uninit
    {
        if (std::is_constant_evaluated())
        {
            //this is needed for constexpr while leaving bytes uninit at runtime
            FOR_C_ARRAY(bytes, i)
                bytes[i] = {};
        }            
    } 

    explicit constexpr     
    Array(std::convertible_to<T> auto&&... elements) : Array()
    {
        auto const emplaceFn = [this](auto&& element) 
        {
            ArrayAssert(count + 1 <= COUNT_MAX, "[Array::Ctor] array exhausted");
            PlacementNew(std::forward<decltype(element)>(element));
        };
        (emplaceFn(std::forward<decltype(elements)>(elements)), ...);
    }

    
    constexpr 
    ~Array()
    {
        Clear();
    }

    //? methods

    constexpr
    void Clear(idx_t beginIdx = 0)
    {
        ArrayAssert(beginIdx >= 0 && beginIdx <= count, "[Array::Clear] beginIdx out of bounds");

        if constexpr (!std::is_trivially_destructible_v<T>)
        {
            while(count > beginIdx)
            {
                this->operator[](count - 1).~T();
                --count;
            }
        }
        else count = beginIdx;        
    }

    //? append

    template<typename... Ts> constexpr
    T& AppendElement(Ts&&... args) requires is_constructible_with<T, Ts...>
    {
        ArrayAssert(count + 1 <= COUNT_MAX, "[Array::AppendElement] array exhausted");
        return *PlacementNew(std::forward<decltype(args)>(args)...);
    }

    template<typename OTHER_T, auto OTHER_N> constexpr
    void AppendArray(Array<OTHER_T, OTHER_N> const& other)
    {
        ArrayAssert(count + other.Count() <= COUNT_MAX, "[Array::AppendArray] array exhausted");
        FOR_ARRAY(other, i)
            PlacementNew(static_cast<T>(other[i]));
    }

    template<typename OTHER_T, auto OTHER_N> constexpr
    void AppendArray(OTHER_T const (&arr)[OTHER_N])
    {
        ArrayAssert(count + OTHER_N <= COUNT_MAX, "[Array::AppendArray] array exhausted");
        FOR_C_ARRAY(arr, i)
            PlacementNew(static_cast<T>(arr[i]));
    }

    template<typename OTHER_T> constexpr
    void AppendArray(OTHER_T const* ptr, idx_t const pCount)
    {
        ArrayAssert(count + pCount <= COUNT_MAX, "[Array::AppendArray] array exhausted");
        for(idx_t i = 0; i < pCount; ++i)
            PlacementNew(static_cast<T>(ptr[i]));
    }

    //? access

    constexpr
    T& operator[](idx_t const idx) 
    { 
        ArrayAssert(idx >= 0 && idx < count, "[Array::[]] array access out of bounds");
        return reinterpret_cast<T*> (bytes)[idx]; 
    } 

    constexpr
    T const& operator[](idx_t const idx) const 
    { 
        ArrayAssert(idx >= 0 && idx < count, "[Array::[]] array access out of bounds");
        return reinterpret_cast<T const*>(bytes)[idx];
    }

    //? helper

    constexpr auto Count() const -> auto     { return count; }
    constexpr auto Empty() const -> bool     { return count == 0; }
    constexpr auto Data()        -> T*       { return &this->operator[](0); }
    constexpr auto Data()  const -> T const* { return &this->operator[](0); }
    constexpr auto Last()        -> T&       { return  this->operator[](count - 1); }
    constexpr auto Last()  const -> T const& { return  this->operator[](count - 1); }

    constexpr auto CurrentByteSize() const { return sizeof(T) * count; }

    constexpr
    bool Contains(auto const& element) //allows for custom operator==
    {
        FOR_ARRAY((*this), i) {
            if (this->operator[](i) == element)
                return true;
        }
        return false;
    }

    void Print() const
    {
        FOR_ARRAY((*this), i)
            dbg::LogInfo(this->operator[](i));
    }

private:

    //? internal helper

    constexpr
    auto PlacementNew(auto&&... args) -> T*
    {
        auto address = &this->operator[](count++);
        //TODO: new placement is not constexpr yet
        return new(address) T { std::forward<decltype(args)>(args)... };
    }

    constexpr
    void ArrayAssert(bool const expr, chars_t msg) const 
    {
        if constexpr (!USE_ARRAY_ASSERTS) return;
        dbg::Assert(expr, msg);
    }

    //? DATA

    alignas(alignof(T)) std::byte bytes [TOTAL_BYTE_SIZE]; //uninit
    idx_t count;

public:

    //? serialization

    void WriteBinaryFile(chars_t path) const
    {
        std::ofstream file { path, std::ios::binary };
        dbg::Assert(file.is_open(), "[IO] cannot open file"); //not an array assert
        file << count;
        file.write(reinterpret_cast<char const*>(bytes), sizeof(T) * count);
    }

    void ReadBinaryFile(chars_t path)
    {
        std::ifstream file { path, std::ios::binary };
        dbg::Assert(file.is_open(), "[IO] cannot open file"); //not an array assert
        file >> count;
        file.read(reinterpret_cast<char*>(bytes), sizeof(T) * count);
    }

};

//? CTAD

template<class T, class... Ts>
Array(T, Ts...) -> Array<T, sizeof...(Ts) + 1>;

}//ns