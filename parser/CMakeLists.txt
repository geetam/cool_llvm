find_package(BISON)
set(cool_support "${CMAKE_CURRENT_SOURCE_DIR}/../cool_support_code/lexing_parsing")
include_directories(${cool_support})
set(SRC ${cool_support}/handle_flags.cc ${cool_support}/stringtab.cc ${cool_support}/utilities.cc ${cool_support}/cool-tree.cc ${cool_support}/dumptype.cc ${cool_support}/tree.cc ${cool_support}/parser-phase.cc ${cool_support}/tokens-lex.cc) 
BISON_TARGET(parser_t_name cool.y ${CMAKE_CURRENT_BINARY_DIR}/parser.cpp COMPILE_FLAGS "-d -v -y -b cool --debug -p cool_yy")
add_executable(parser ${BISON_parser_t_name_OUTPUTS} ${SRC})
target_link_libraries(parser y) 
