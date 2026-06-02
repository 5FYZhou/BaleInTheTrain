cmake_minimum_required(VERSION 3.16)

project(BaleInTheTrainWorkspace LANGUAGES CXX)

set(LOCAL_SFML_DIR "D:/code_soft/SFML/SFML-3.0.2/lib/cmake/SFML")
if(EXISTS "${LOCAL_SFML_DIR}/SFMLConfig.cmake")
    set(SFML_DIR "${LOCAL_SFML_DIR}" CACHE PATH "SFML 3 config directory")
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

add_subdirectory(code)
