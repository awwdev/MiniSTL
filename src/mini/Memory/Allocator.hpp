//https://github.com/awwdev

#pragma once
#include "mini/Utils/Types.hpp"
#include "mini/Box/Bitset.hpp"
#include "mini/Debug/Logger.hpp"

#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#include "psapi.h"

//? note: do not claim a block per array element, instead claim one block for the whole array

namespace mini::mem {

struct BlockArray
{
    u32 size;
    u32 count;
};

//?-------------------------------------
//?DEFINE BLOCKS HERE (keep sorted for size!)
constexpr BlockArray BLOCK_ARRAYS [] {
    { .size =      1'000, .count = 5 },
    { .size =     10'000, .count = 5 },
    { .size =    100'000, .count = 5 },
    { .size =  1'000'000, .count = 5 },
    { .size = 10'000'000, .count = 5 },
};
//?-------------------------------------

constexpr auto BLOCK_ARRAY_COUNT = ArrayCount(BLOCK_ARRAYS);
constexpr auto BLOCK_COUNT_TOTAL = []() constexpr {
    std::size_t count = 0;
    FOR_CARRAY(BLOCK_ARRAYS, i)
        count += BLOCK_ARRAYS[i].count;
    return count;
}();

constexpr auto ALLOCATION_SIZE = []() constexpr {
    std::size_t size = 0;
    FOR_CARRAY(BLOCK_ARRAYS, i)
        size += BLOCK_ARRAYS[i].size * BLOCK_ARRAYS[i].count;
    return size;
}();

namespace priv 
{
    //all blocks are successive 
    inline box::Bitset<BLOCK_COUNT_TOTAL> blocksUsed;
    inline u8* blockArrayPtrs [BLOCK_ARRAY_COUNT]; //u8 for ptr arithmetics 

    template<u32 ARRAY_INDEX>
    u8* BlockAddress(const u32 blockIdx) 
    {
        constexpr auto blockSize = BLOCK_ARRAYS[ARRAY_INDEX].size;
        return priv::blockArrayPtrs[ARRAY_INDEX] + (blockIdx * blockSize);
    }

    inline void FreeBlock(const u32 blockId)
    {
        priv::blocksUsed.Flip(blockId);
        //LOG("FreeBlock", blockId);
        //dtor call of object is done in BlockPtr
    }
}

inline void GlobalAllocate()
{
    priv::blockArrayPtrs[0] = (u8*)VirtualAlloc(NULL, ALLOCATION_SIZE, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    LOG("Block array:", 0, BLOCK_ARRAYS[0].size * BLOCK_ARRAYS[0].count, (void*)priv::blockArrayPtrs[0]);

    for(auto i = 1; i < BLOCK_ARRAY_COUNT; ++i) {
        priv::blockArrayPtrs[i] = priv::blockArrayPtrs[i-1] + BLOCK_ARRAYS[i-1].size * BLOCK_ARRAYS[i-1].count; 
        LOG("Block array:", i, BLOCK_ARRAYS[i].size * BLOCK_ARRAYS[i].count, (void*)priv::blockArrayPtrs[i]);
    }
}

inline void GlobalDeallocate()
{
    VirtualFree(priv::blockArrayPtrs[0], 0, MEM_RELEASE);
}

template<class T>
struct BlockPtr
{
    using DATA_T = T;

    T* ptr = nullptr;
    u32 blockId = 0;

    T* operator->() { return  ptr; }
    T& operator* () { return *ptr; }

    T&       Get()       { return *ptr; }
    const T& Get() const { return *ptr; }

    //array access
    auto&       operator[](const std::size_t i)        { return (*ptr)[i]; };
    const auto& operator[](const std::size_t i) const  { return (*ptr)[i]; };

    BlockPtr(T* const pPtr, const u32 pBlockId)
        : ptr     { pPtr }
        , blockId { pBlockId } 
    {}

    BlockPtr(const BlockPtr&)            = delete;
    BlockPtr& operator=(const BlockPtr&) = delete;

    BlockPtr(BlockPtr&& other) 
        : ptr     { other.ptr }
        , blockId { other.blockId } 
    { 
        other.ptr = nullptr;
    }

    BlockPtr& operator=(BlockPtr&& other)           
    { 
        ptr         = other.ptr; 
        blockId     = other.blockId;
        other.ptr   = nullptr; 
        return *this; 
    }

    ~BlockPtr()
    { 
        if (ptr == nullptr) return;
        ptr->~T();
        priv::FreeBlock(blockId);
    }
};

template<class T, class... CtorArgs>
auto ClaimBlock(CtorArgs&&... args)
{
    struct FittingBlockSize { u32 arrayIdx; u32 blockId; }; //blockId == bitIdx

    constexpr auto FITTING_BLOCK_ARRAY = []() constexpr -> FittingBlockSize {
        u32 blockId = 0;
        for(u32 i = 0; i < BLOCK_ARRAY_COUNT; ++i) {
            if (BLOCK_ARRAYS[i].size >= (sizeof(T) + alignof(T))) //max
                return FittingBlockSize { i, blockId };
            blockId += BLOCK_ARRAYS[i].count;
        }
        //!no appropriate block size found (compile time error)
    }();

    const auto freeBlockId = priv::blocksUsed.FindFirstFreeBit(FITTING_BLOCK_ARRAY.blockId);
    priv::blocksUsed.Flip(freeBlockId);

    u8* blockAddress = priv::BlockAddress<FITTING_BLOCK_ARRAY.arrayIdx>(freeBlockId - FITTING_BLOCK_ARRAY.blockId);
    u8* aligned      = (u8*) (((std::uintptr_t)blockAddress + (alignof(T) - 1)) & ~(alignof(T) - 1));
    
    T* obj;
    if constexpr(std::is_array_v<T>) obj = new (aligned) T [ ArrayCount<T>() ];
    else                             obj = new (aligned) T { std::forward<CtorArgs>(args) ... };

    //LOG("ClaimBlock", FITTING_BLOCK_ARRAY.arrayIdx, freeBlockId - FITTING_BLOCK_ARRAY.blockId, freeBlockId);
    return BlockPtr<T> { obj, freeBlockId };
}

inline idx_t GetBlockArrayIdxFromBlockId(const idx_t blockId)
{
    idx_t blockCount = 0;
    FOR_CARRAY(BLOCK_ARRAYS, i){
        blockCount += BLOCK_ARRAYS[i].count;
        if (blockId < blockCount)
            return i;
    }
    ERR("cannot get block array idx from block id");
    return 0;
}

}//ns