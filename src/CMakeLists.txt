set(CORE_COMPONENTS)

add_subdirectory(lexer)

add_library(core SHARED
    ${CORE_COMPONENTS}
)

target_include_directories(core
    PUBLIC 
        ${CMAKE_SOURCE_DIR}/include                     
)

target_compile_definitions(core PRIVATE BUILDING_CORE_DLL)

set_target_properties(core PROPERTIES
    OUTPUT_NAME "minicomp"                
    VERSION 1.0.0                     
    SOVERSION 1                       
)