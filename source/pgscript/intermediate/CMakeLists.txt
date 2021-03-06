set(INTERMEDIATE_SOURCES
    "anonymous_method.cpp"
    "ast.cpp"
    "ast_printer.cpp"
    "class_declaration.cpp"
    "function_declaration.cpp"
    "get_expression.cpp"
    "parameter.cpp"
    "set_expression.cpp"
)

set(INTERMEDIATE_PUBLIC_HEADERS
    "anonymous_method.h"
    "ast.h"
    "ast_printer.h"
    "ast_visitor.h"
    "class_declaration.h"
    "function_declaration.h"
    "get_expression.h"
    "parameter.h"
    "set_expression.h"
)

add_library(pgscript_intermediate OBJECT ${INTERMEDIATE_SOURCES})

target_include_directories(
    pgscript_intermediate
    PRIVATE
        $<INSTALL_INTERFACE:pagoda>
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/source>
        ${Boost_INCLUDE_DIRS}
)

target_compile_features(
    pgscript_intermediate
    PRIVATE
        cxx_std_17
)

target_compile_options(
    pgscript_intermediate
    PRIVATE
        -Wall
        $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Werror>
        $<$<CXX_COMPILER_ID:GNU>:-Wno-gnu-zero-variadic-macro-arguments>
)

target_link_libraries(
    pgscript_intermediate
    PRIVATE
        common
)
