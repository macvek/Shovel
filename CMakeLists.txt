cmake_minimum_required(VERSION 3.15)

project(Shovel VERSION 1.0)

set(gcc_like_cxx "$<COMPILE_LANG_AND_ID:CXX,ARMClang,AppleClang,Clang,GNU,LCC>")
set(msvc_cxx "$<COMPILE_LANG_AND_ID:CXX,MSVC>")

add_library(shared_compiler_flags INTERFACE)
target_compile_features(shared_compiler_flags INTERFACE cxx_std_11)
target_compile_options(shared_compiler_flags INTERFACE
  "$<${gcc_like_cxx}:$<BUILD_INTERFACE:-Wall;-Wextra;-Wshadow;-Wformat=2;-Wunused>>"
  "$<${msvc_cxx}:$<BUILD_INTERFACE:-W3>>"
)

add_executable(Shovel 
    Main.cxx
    console.cxx
)

target_link_libraries(Shovel rt)