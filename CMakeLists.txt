cmake_minimum_required(VERSION 3.30)
project(BalokerOnline VERSION 0.0.1 DESCRIPTION 63 LANGUAGES C)

set(CMAKE_C_STANDARD 17)

# Sources
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/Baloker Online/src/*.c"
)

# Executable
add_executable(${PROJECT_NAME} ${SOURCES})

target_link_options(${PROJECT_NAME} PRIVATE
    $<$<CONFIG:Release>:/SUBSYSTEM:WINDOWS>
    $<$<CONFIG:Debug>:/SUBSYSTEM:CONSOLE>
)

# Includes
target_include_directories(${PROJECT_NAME} PRIVATE
    "${CMAKE_SOURCE_DIR}/Baloker Online/include"
)

# SDL3
target_link_directories(${PROJECT_NAME} PRIVATE
    "${CMAKE_SOURCE_DIR}/Baloker Online"
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    SDL3
)

# Output
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/Debug"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/Release"
)

# Copy SDL3
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_SOURCE_DIR}/Baloker Online/SDL3.dll"
        $<TARGET_FILE_DIR:${PROJECT_NAME}>
)
