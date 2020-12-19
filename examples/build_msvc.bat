@echo off
rem launch this from msvc-enabled console

set CXXFLAGS=/std:c++17 /O2 /FC /Zi /W4 /WX /J /wd4458 /wd4996 /nologo
set INCLUDES=/I ..\

cl.exe %CXXFLAGS% %INCLUDES% cat.cpp

cl.exe %CXXFLAGS% %INCLUDES% gol.cpp

cl.exe %CXXFLAGS% %INCLUDES% sprintln.cpp

cl.exe %CXXFLAGS% %INCLUDES% utf8.cpp

cl.exe %CXXFLAGS% %INCLUDES% hashmap.cpp

cl.exe %CXXFLAGS% %INCLUDES% custom_struct_as_hashmap_key.cpp
