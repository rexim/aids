@echo off
rem launch this from msvc-enabled console

set CXXFLAGS=/std:c++17 /O2 /FC /Zi /W4 /WX /J /wd4458 /wd4996 /nologo
set INCLUDES=/I ..\

cl.exe %CXXFLAGS% %INCLUDES% utf8_test.cpp

cl.exe %CXXFLAGS% %INCLUDES% hash_map_test.cpp

cl.exe %CXXFLAGS% %INCLUDES% string_view_test.cpp
