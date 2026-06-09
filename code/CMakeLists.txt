cmake_minimum_required(VERSION 3.16)

project(BaleInTheTrain LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SFML 3 COMPONENTS Graphics Window System Audio REQUIRED)

add_executable(BaleInTheTrain
    src/main.cpp
)

target_link_libraries(BaleInTheTrain
    SFML::Graphics
    SFML::Window
    SFML::System
    SFML::Audio
)

if(WIN32)
    add_custom_command(TARGET BaleInTheTrain POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_RUNTIME_DLLS:BaleInTheTrain>
            $<TARGET_FILE_DIR:BaleInTheTrain>
        COMMAND_EXPAND_LISTS
    )
endif()
