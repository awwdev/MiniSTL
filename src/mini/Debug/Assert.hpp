#pragma once
#include "mini/Debug/Logger.hpp"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#undef max

//todo: make a define #ifdef DEBUG otherwise the function is just empty

namespace mini 
{
    inline void Assert(const bool b, const char* msg = "") 
    { 
        if (!b)
        {
            BringWindowToTop(GetConsoleWindow()); //win only
            ERR(msg);
            system("pause"); //win only
        }
    } 

}//ns
