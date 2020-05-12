#pragma once
#include "MiniSTL/Types.hpp"
#include "MiniSTL/Box/Bitset.hpp"
#include "MiniSTL/Debug/Logger.hpp"

#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#include "psapi.h"
#undef max


namespace mini::mem
{
    struct AllocInfo
    {
        u32 blockSize;
        u32 blockCount;
    };

    //!--------------------------------------
    //define blocks at compile time (size and count) 
    //KEEP IT SORTED BY SIZE
    constexpr AllocInfo allocs[] = {
        {  128, 100 }, //used for strings
        { 1000, 100 }
    };
    //!--------------------------------------


    constexpr auto ALLOC_COUNT = sizeof(allocs) / sizeof(allocs[0]);
    constexpr auto BLOCK_COUNT = []() constexpr { //total block count
        std::size_t count = 0;
        for(auto i  =0; i < ALLOC_COUNT; ++i) {
            count += allocs[i].blockCount;
        }
        return count;
    }();

    inline u8* allocPtrs [ALLOC_COUNT];         //index based
    inline box::Bitset<BLOCK_COUNT> blocksUsed; //all blocks


    inline void Allocate()
    {
        HANDLE const heap = GetProcessHeap();
        for(auto i = 0; i < ALLOC_COUNT; ++i) {
            allocPtrs[i] = (u8*)HeapAlloc(heap, 0, allocs[i].blockSize); 
        }
    }


    template<class T>
    struct MemPtr
    {
        T* ptr;
        std::size_t blockId;

        T* operator->() { return ptr; }
        T& operator* () { return *ptr;}


        MemPtr() = delete;
        MemPtr(T* const pPtr, const std::size_t pBlockId)
            : ptr     { pPtr }
            , blockId { pBlockId } 
        {;}

        MemPtr(const MemPtr&)            = delete;
        MemPtr& operator=(const MemPtr&) = delete;


        MemPtr(MemPtr&& other) 
            : ptr     { other.ptr }
            , blockId { other.blockId } 
        { 
            other.ptr = nullptr;
        }

        MemPtr& operator=(MemPtr&& other)           
        { 
            ptr         = other.ptr; 
            blockId     = other.blockId;
            other.ptr   = nullptr; 
            return *this; 
        }

        ~MemPtr()
        { 
            if (ptr != nullptr) 
            { 
                ptr->~T();
                blocksUsed.Flip(blockId); 
            } 
        }
    };


    template<class T, class... CtorArgs>
    auto ClaimBlock(CtorArgs&&... args)
    {
        struct FittingAlloc 
        { 
            std::size_t allocIdx; 
            std::size_t allocBitBegin;
        };

        constexpr auto FIT = []() constexpr -> FittingAlloc {
            std::size_t allocBitBegin = 0;
            for(std::size_t i=0; i<ALLOC_COUNT; ++i) 
            {
                if (allocs[i].blockSize >= (sizeof(T) + alignof(T))) //assumes sorted
                    return { i, allocBitBegin };
                allocBitBegin += allocs[i].blockCount;
            }
            //will not compile when hitting this point
        }();

        const auto freeBlock = blocksUsed.FindFirstFreeBit(FIT.allocBitBegin); //start search at the alloc
        blocksUsed.Flip(freeBlock); //mark used

        constexpr auto blockSize = allocs[FIT.allocIdx].blockSize;
        auto* ptr = allocPtrs[FIT.allocIdx] + ((freeBlock - FIT.allocBitBegin) * blockSize);
        auto* aligned = (u8*) (((std::uintptr_t)ptr + (alignof(T) - 1)) & ~(alignof(T) - 1)); //next aligned address
        auto* obj = new (aligned) T (std::forward<CtorArgs>(args) ...);

        return MemPtr<T> { obj, freeBlock };
    }

}//ns

//todo: print blocks