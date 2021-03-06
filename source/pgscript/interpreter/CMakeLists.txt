set(INTERPRETER_SOURCES
    "interpreter.cpp"
    "interpreter_visitor.cpp"
)

set(INTERPRETER_PUBLIC_HEADERS
    "interpreter.h"
    "interpreter_visitor.h"
)

add_library(pgscript_interpreter OBJECT ${INTERPRETER_SOURCES})

target_include_directories(
    pgscript_interpreter
    PRIVATE
        $<INSTALL_INTERFACE:pagoda>
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/source>
        ${Boost_INCLUDE_DIRS}
)

target_compile_features(
    pgscript_interpreter
    PRIVATE
        cxx_std_17
)

target_compile_options(
    pgscript_interpreter
    PRIVATE
        -Wall
        $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Werror>
        $<$<CXX_COMPILER_ID:GNU>:-Wno-gnu-zero-variadic-macro-arguments>
)

target_link_libraries(
    pgscript_interpreter
    PRIVATE
        common
)
