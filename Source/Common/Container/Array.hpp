#pragma once
#include "Common/Types.hpp"
#include "Debug/Assert.hpp"
#include "Debug/Logger.hpp"

#include <cstring>

namespace rpg::com {

constexpr bool USE_ARRAY_ASSERTS = true;

constexpr void ArrayAssert(const bool condition, chars_t msg = "array assertion failed")
{
    if (std::is_constant_evaluated())
        return;

    if constexpr(USE_ARRAY_ASSERTS) {
        if (condition == false){
            dbg::LogError(msg);
            dbg::DebugBreak();
        }
    }
}

#define FOR_ARRAY(arr, i) for(idx_t i = 0; i < arr.count; ++i)

template<class T, auto N>
struct Array
{
    using TYPE = T;

    idx_t count = 0;

    static constexpr idx_t CAPACITY  = (idx_t)N;
    static constexpr idx_t BYTE_SIZE = sizeof(T) * CAPACITY;
    alignas(alignof(T)) u8 bytes[BYTE_SIZE]; //don't init

    Array() = default;
    Array(std::initializer_list<T> list) {
        for(auto&& element : list){
            Append(element);
        }
    }

    //? ACCESS

    template<class IDX> T&       operator[](const IDX idx)       { return reinterpret_cast<T&>      (bytes[sizeof(T) * (idx_t)idx]); }
    template<class IDX> const T& operator[](const IDX idx) const { return reinterpret_cast<const T&>(bytes[sizeof(T) * (idx_t)idx]); }

    T&       Last()       { ArrayAssert(count > 0); return this->operator[](count - 1); }
    const T& Last() const { ArrayAssert(count > 0); return this->operator[](count - 1); }
    T*       Data()       { return &(this->operator[](0)); }
    const T* Data() const { return &(this->operator[](0)); }

    //? COUNT MODIFICATION

    template<class... ARGS>
    T* Append(ARGS&&... args)
    {
        ArrayAssert(count < CAPACITY, "array capacity exhausted");
        auto* ptr = PlacementNew(std::forward<ARGS>(args)...);
        ++count;
        return ptr;
    }

    template<auto OTHER_N>
    void AppendArray(const Array<T, OTHER_N>& other)
    {
        ArrayAssert(count + other.count <= CAPACITY, "array capacity exhausted");
        FOR_ARRAY(other, i) {
            PlacementNew(other[i]);
            ++count;
        }
    }

    template<auto OTHER_N>
    void AppendArray(const T (&arr)[OTHER_N])
    {
        ArrayAssert(count + OTHER_N <= CAPACITY, "array capacity exhausted");
        FOR_CARRAY(arr, i) {
            PlacementNew(arr[i]);
            ++count;
        }
    }

    void Clear(const idx_t begin = 0)
    {
        ArrayAssert(begin <= count, "clear begin idx is bigger than array count");
        if constexpr (!std::is_trivial_v<T>) 
        {
            while(count > begin){
                --count;
                this->operator[](count).~T();
            }   
        }
        else count = begin;
    }

    //? HELPER

    template<class E>
    T* Contains(const E& element) //allows for custom operator==
    {
        FOR_ARRAY((*this), i) {
            if (this->operator[](i) == element)
                return &(this->operator[](i));
        }
        return nullptr;
    }

    template<class... ARGS>
    T* PlacementNew(ARGS&&... args)
    {
        return new(&bytes[sizeof(T) * count]) T { std::forward<ARGS>(args)... };
    }

    bool Empty()       const { return count == 0; }
    auto CurrentSize() const { return sizeof(T) * count; }
};

template<class T, auto N>
void PrintArray(const Array<T, N>& arr)
{
    FOR_ARRAY(arr, i){
        std::cout << arr[i] << ',';
    }
    std::cout << '\n';
}

}//ns