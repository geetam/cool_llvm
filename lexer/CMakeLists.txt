find_package(FLEX)
set(cool_support "${CMAKE_CURRENT_SOURCE_DIR}/../cool_support_code/lexing_parsing")
include_directories(${cool_support})
set(SRC "${cool_support}/handle_flags.cc" "${cool_support}/stringtab.cc" "${cool_support}/utilities.cc" ${cool_support}/lextest.cc) 
FLEX_TARGET(lexer_t_name cool.flex ${CMAKE_CURRENT_BINARY_DIR}/lexer.cpp)
add_executable(lexer ${FLEX_lexer_t_name_OUTPUTS} ${SRC})

