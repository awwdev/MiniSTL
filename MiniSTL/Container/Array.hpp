#pragma once
#include "MiniSTL/Types.hpp"
#include "MiniSTL/Debug/Assert.hpp"
#include "MiniSTL/Debug/Profiler.hpp"
#include <vector>

#define FOR_ARRAY(arr, i) for(std::decay_t<decltype(arr)>::COUNT_TYPE i=0; i<arr.Count(); ++i)

//may comment out on release mode:
#define CHECK_INDEX(i, c)       mini::Assert(i<c && i>=0);
#define CHECK_CAPACITY(c, cap)  mini::Assert(c<=cap);

#define ANY_SIZE template<typename A, typename = std::enable_if_t<std::is_same_v<T, typename A::DATA_TYPE>>>


namespace mini::container
{   
    //used for passing only
    template<class T, typename C = u16> class IArray
    {
    protected:
        IArray(T* const buffer, const C capacity)
            : bufferPtr { buffer }
            , CAPACITY  { capacity }
            , count     { 0 }
        {}
        ~IArray()
        {
            Clear();
        }
    public:
        [[nodiscard]] T&       operator[](const C i)        { CHECK_INDEX(i, count);       return bufferPtr[i]; } 
        [[nodiscard]] const T& operator[](const C i) const  { CHECK_INDEX(i, count);       return bufferPtr[i]; }
        [[nodiscard]] T&       Last()                       { CHECK_INDEX(count-1, count); return bufferPtr[count - 1]; }
        [[nodiscard]] const T& Last()  const                { CHECK_INDEX(count-1, count); return bufferPtr[count - 1]; }
        [[nodiscard]] C Count() const                       { return count; }
        [[nodiscard]] bool     Empty() const                { return count == 0; }

        void Clear() { while (count > 0) bufferPtr[--count].~T(); }

        template<class... Args> 
        void Append(Args&&... args)
        {
            CHECK_CAPACITY(count+1, CAPACITY);
            new(&bufferPtr[count++]) T{ std::forward<Args>(args)... };
        }

        template<class... Args>
        [[nodiscard]] T& AppendRtn(Args&&... args)
        {
            CHECK_CAPACITY(count + 1, CAPACITY);
            new(&bufferPtr[count++]) T{ std::forward<Args>(args)... };
            return bufferPtr[count - 1];
        }

        void Remove(const C i) //O(1)
        {   
            CHECK_INDEX(i, count);
            bufferPtr[i].~T();
            if (--count != i) { 
                new(&bufferPtr[i]) T{ std::move(bufferPtr[count]) }; 
                bufferPtr[count].~T();
            }
        } 

        void RemoveOrdered(C i) //O(n)
        {
            STORE_PROFILE_FUNCTION();
            CHECK_INDEX(i, count);
            bufferPtr[i].~T();
            for (; i < count - 1; ++i) {
                bufferPtr[i] = std::move(bufferPtr[i + 1]);
            }
            bufferPtr[--count].~T();
        }

        auto Contains(const T& t) -> T*
        {
            for (C i = 0; i < count; ++i) {
                if (bufferPtr[i] == t) return &bufferPtr[i];
            }
            return nullptr;
        }

        void Reverse()
        {
            for (C i = 0; i < (C)(count * 0.5f); ++i) { //swap
                const auto tmp = std::move(bufferPtr[i]);
                bufferPtr[i] = std::move(bufferPtr[count - 1 - i]);
                bufferPtr[count - 1 - i] = std::move(tmp);
            }
        }

    protected:
        T* const bufferPtr;
        C  count;
    public:
        const C CAPACITY;
        using DATA_TYPE  = T;
        using COUNT_TYPE = C;
    };


    template<class T, std::size_t CAPACITY, typename C = u16>
    class Array final : public IArray<T, C>
    {
    public:
        Array() : IArray<T, C> { reinterpret_cast<T* const>(buffer), CAPACITY } { ; }

        template<class... Args, typename = std::enable_if_t<std::is_same_v<T, std::common_type_t<Args...>>>>
        Array(Args&&... elements) : Array() //no forwarding ctor values but T elements (cause tmp creation)
        { 
            ((new(&buffer[this->count++ * sizeof(T)]) T{ std::forward<Args>(elements) }), ...);
        }

        Array(const Array& other) : Array() { this->operator=<Array>(other); }
        Array(Array&& other) : Array() { this->operator=<Array>(std::move(other)); }

        //can be used for resize
        ANY_SIZE Array(const A& other) : Array() { this->operator=<A>(other); }
        ANY_SIZE void operator=(const A& other)
        {
            this->count = (CAPACITY >= other.Count()) ? other.Count() : CAPACITY;
            for (C i = 0; i < this->count; ++i) ctor(i, other[i]);
        }

        ANY_SIZE Array(A&& other) : Array() { this->operator=<A>(std::move(other)); }
        ANY_SIZE void operator=(A&& other)
        {
            this->count = (CAPACITY >= other.Count()) ? other.Count() : CAPACITY;
            for (C i = 0; i < this->count; ++i) ctor(i, std::move(other[i]));
        }

    private:
        inline void ctor(const std::size_t& i, T&& t)       { new(&buffer[i * sizeof(T)]) T{ std::move(t) }; }
        inline void ctor(const std::size_t& i, const T& t)  { new(&buffer[i * sizeof(T)]) T{ t }; }

        alignas(T) std::byte buffer[sizeof(T[CAPACITY])]; //avoid ctor calls
   };

}//ns

#undef ANY_SIZE
#undef CHECK_INDEX(i, c)
#undef CHECK_CAPACITY(c, cap)