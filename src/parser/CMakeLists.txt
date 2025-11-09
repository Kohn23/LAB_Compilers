# 语法分析器库
add_library(mini_parser
    Parser.cpp
    RecursiveDescentParser.cpp
    Grammar.cpp
)

target_include_directories(mini_parser
    PUBLIC ${CMAKE_SOURCE_DIR}/include
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
)

# 依赖关系
target_link_libraries(mini_parser
    PUBLIC mini_lexer
)