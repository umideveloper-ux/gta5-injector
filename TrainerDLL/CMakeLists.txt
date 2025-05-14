cmake_minimum_required(VERSION 3.10)
project(TrainerDLL)
add_library(TrainerDLL SHARED dllmain.cpp)
set_target_properties(TrainerDLL PROPERTIES PREFIX "" SUFFIX ".dll")
