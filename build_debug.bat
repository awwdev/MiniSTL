@echo off ^
 & mkdir build_debug ^
 & mkdir build_debug\objs ^
 & mkdir build_debug\res ^
 & xcopy /s /y /d res\* build_debug\res\* ^
 & Pushd G:\\DEV2020\\buildtools\\msys2\\mingw64\\bin ^
 & gcc ^
-I %2 ^
-L %5 ^
-Wall -Wpedantic -Wextra ^
-g ^
-municode ^
-std=c++2a ^
-o %3 ^
%4 ^
-static ^
-lstdc++ ^
-lvulkan-1 ^
 & popd