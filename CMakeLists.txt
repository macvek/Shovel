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

set(sources main.cxx
  console.cxx
  timer.cxx
  log.cxx
  input.cxx
  inputdecoder.cxx
  terminal.cxx
  editor.cxx
)

add_executable(Shovel ${sources})

target_link_libraries(Shovel rt)
target_link_options(Shovel PUBLIC 
  "$<${gcc_like_cxx}:-rdynamic>"  
)

enable_testing()

add_executable(InputDecoderDemo inputdecoder_demo.cxx log.cxx console.cxx inputdecoder.cxx )
add_executable(InputDecoderTest inputdecoder_test.cxx inputdecoder.cxx )
add_test(NAME InputDecoderTest COMMAND InputDecoderTest)

add_executable(NameEnumTest nameenum_test.cxx)
add_test(NAME NameEnumTest COMMAND NameEnumTest)

add_executable(TerminalDemo terminal_demo.cxx terminal.cxx)

add_executable(EditorDemo editor_demo.cxx editor.cxx terminal.cxx log.cxx console.cxx inputdecoder.cxx)
add_executable(EditorTest editor_test.cxx editor.cxx inputdecoder.cxx)
add_test(NAME EditorTest COMMAND EditorTest)

add_executable(RenderBufferDemo renderbuffer_demo.cxx renderbuffer.cxx terminal.cxx log.cxx console.cxx inputdecoder.cxx )
add_executable(RenderBufferTest renderbuffer_test.cxx renderbuffer.cxx terminal.cxx )
add_test(NAME RenderBufferTest COMMAND RenderBufferTest)