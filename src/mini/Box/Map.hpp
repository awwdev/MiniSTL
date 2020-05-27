//https://github.com/awwdev

/* 
## mini::box::Map

- index based (any enum can be used, no cast needed)(no bucket system)
- fixed size (capacity-based with internal count)
- no ctor calls when initializing the map (underlying byte array)
- bounds checking is toggleable via macro, no exceptions are used
- overall more readable and extendible than the STL

*/

#pragma once
#include "mini/Utils/Types.hpp"
#include "mini/Debug/Assert.hpp"
#include "mini/Debug/Logger.hpp"
#include "mini/Box/Bitset.hpp"

//TODO: bound checks

namespace mini::box
{
    //use for passing around
    template<class T>
    struct IMap
    {
        template<typename IDX, typename = IsIntegralOrEnum<IDX>>
        bool Contains(const IDX i)
        {
            return bitset.Test(i);
        }

        template<typename IDX, class... CtorArgs, typename = IsIntegralOrEnum<IDX>>
        void Set(const IDX idx, CtorArgs&&... args)
        {
            if (bitset.Test(idx))
                dataPtr[idx].~T();

            bitset.Set<true>(idx);
            
            if constexpr (std::is_aggregate_v<T>)
            {
                new(&dataPtr[idx]) T{ std::forward<CtorArgs>(args)... };
            }
            else
            {
                new(&dataPtr[idx]) T(args...);
            }
        }

        template<typename IDX, class... CtorArgs, typename = IsIntegralOrEnum<IDX>>
        T& Get(const IDX idx)
        {
            return dataPtr[idx];
        }

        void Clear()
        {
            for(u32 i = 0; i < CAPACITY; ++i)
            {
                if (bitset.Test(i))
                    dataPtr[i].~T();
            }
            bitset.Clear();
        }

    protected:
        IMap(T* const data, const u32 capacity) : dataPtr { data }, CAPACITY { capacity } {;}
        T* const dataPtr;
        const u32 CAPACITY;
        box::Bitset<100> bitset; //!issue !!! 

        ~IMap() { Clear(); }
    };

    template<class T, auto CAPACITY_T, typename = IsArrayIndex<CAPACITY_T>>
    struct Map : IMap<T>
    {
        using BASE = IMap<T>;

        constexpr static auto CAPACITY  = CAPACITY_T;
        constexpr static auto BYTE_SIZE = CAPACITY * sizeof(T);

        Map() : BASE(reinterpret_cast<T*>(data), CAPACITY) {;}

    private:
        alignas(T) u8 data[BYTE_SIZE];
    };

}//ns