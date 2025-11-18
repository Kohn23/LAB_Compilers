set(CORE_SOURCES "")

add_subdirectory(error)
add_subdirectory(tok)
add_subdirectory(lexer)


add_library(minicomp SHARED
    ${CORE_SOURCES}
)

target_include_directories(minicomp
    PUBLIC 
        ${CMAKE_SOURCE_DIR}/include
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}                   
)

target_compile_definitions(minicomp PRIVATE BUILDING_MINICOMP_DLL)

set_target_properties(minicomp PROPERTIES
    OUTPUT_NAME "minicomp"                
    VERSION 1.0.0                     
    SOVERSION 1                       
)