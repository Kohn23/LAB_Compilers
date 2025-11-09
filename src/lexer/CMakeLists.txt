# 词法分析器库
add_library(mini_lexer
    Lexer.cpp
    Token.cpp
)

target_include_directories(mini_lexer
    PUBLIC ${CMAKE_SOURCE_DIR}/include
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
)

# 设置属性
set_target_properties(mini_lexer PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)