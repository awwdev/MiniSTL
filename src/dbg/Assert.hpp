#pragma once
#include "dbg/Logger.hpp"

//TODO: const std::source_location& location = std::source_location::current()
//#include <source_location>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#undef max

#include <fstream>

namespace rpg::dbg {

//DO NOT EXCLUDE THIS ASSERT VIA CONSTEXPR FLAG (OR MACRO) BUT DEFINE WRAPPER
template<class T>
void Assert(const bool expression, const T& msg) 
{ 
    if (!expression)  {
        dbg::LogError("[ASSERTION FAILED]", msg);
        BringWindowToTop(GetConsoleWindow()); 
        __debugbreak();
    }
} 

}//NS