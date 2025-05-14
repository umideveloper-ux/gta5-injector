cmake_minimum_required(VERSION 3.10)
project(Injector)
add_executable(Injector main.cpp pe.cpp mapper.cpp crypto.cpp stealth.cpp antidebug.cpp antidump.cpp)
