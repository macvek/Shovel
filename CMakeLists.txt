cmake_minimum_required(VERSION 3.15)

project(Shovel VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)
set(gcc_like_cxx "$<COMPILE_LANG_AND_ID:CXX,ARMClang,AppleClang,Clang,GNU,LCC>")
set(msvc_cxx "$<COMPILE_LANG_AND_ID:CXX,MSVC>")

IF (WIN32)
  add_definitions(-DBUILDONWINDOWS)
ENDIF()

add_library(shared_compiler_flags INTERFACE)
target_compile_features(shared_compiler_flags INTERFACE cxx_std_11)
target_compile_options(shared_compiler_flags INTERFACE
  "$<${gcc_like_cxx}:$<BUILD_INTERFACE:-Wall;-Wextra;-Wshadow;-Wformat=2;-Wunused>>"
  "$<${msvc_cxx}:$<BUILD_INTERFACE:-W3>>"
)

set (liblog log.cxx)

set (libspecialcharsmap specialcharsmap.cxx)
set (libinputdecoder inputdecoder.cxx)
set (libterminal terminal.cxx)

set (libconsole console.cxx ${liblog})

set (libeditor editor.cxx ${libinputdecoder})
set (librenderbuffer renderbuffer.cxx ${libspecialcharsmap} ${libterminal})

set (libframe frame.cxx ${librenderbuffer})

set (libtimer timer.cxx ${liblog} )
set (libinput input.cxx ${libinputdecoder} ${liblog})

set (libsources ${liblog}
  ${libconsole}
  ${libtimer}
  ${libinput}
  ${libterminal}
  ${librenderbuffer}
  ${libframe}
)

set(sources ${libsources}
  main.cxx
)

IF (NOT WIN32)
  link_libraries(rt)
ENDIF()

add_executable(Shovel ${sources})
target_link_options(Shovel PUBLIC 
  "$<${gcc_like_cxx}:-rdynamic>"  
)

enable_testing()

add_executable(InputDecoderDemo inputdecoder_demo.cxx ${libconsole} ${libinputdecoder} )
add_executable(InputDecoderTest inputdecoder_test.cxx ${libinputdecoder} )
add_test(NAME InputDecoderTest COMMAND InputDecoderTest)

add_executable(NameEnumTest nameenum_test.cxx)
add_test(NAME NameEnumTest COMMAND NameEnumTest)

add_executable(TerminalDemo terminal_demo.cxx ${libterminal})

add_executable(EditorDemo editor_demo.cxx ${libeditor} ${libterminal} ${libinput} ${libconsole})
add_executable(EditorTest editor_test.cxx ${libeditor})
add_test(NAME EditorTest COMMAND EditorTest)

add_executable(RenderBufferDemo renderbuffer_demo.cxx ${librenderbuffer} ${libconsole} ${libinput} )
add_executable(RenderBufferTest renderbuffer_test.cxx ${librenderbuffer} )
add_test(NAME RenderBufferTest COMMAND RenderBufferTest)

add_executable(TimerDemo timer_demo.cxx ${libtimer} ${libinput} ${libconsole})
add_executable(FrameDemo frame_demo.cxx ${libframe})

add_executable(SnakeDemo snake_demo.cxx ${libsources})
add_executable(TetrisDemo tetris_demo.cxx ${libsources})

add_executable(EventsTest events_test.cxx)
add_test(NAME EventsTest COMMAND EventsTest)
