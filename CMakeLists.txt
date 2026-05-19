# 指定cmake最低版本要求
cmake_minimum_required(VERSION 3.28)
# 设置项目名称和使用的编程语言
project(CMakeProject LANGUAGES CXX)
# 设置可执行文件输出目录，使得main可执行文件生成在build/bin目录下
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# 选项：是否构建共享库，共享库设为 ON，静态库设为 OFF
option(BUILD_SHARED_LIBS "Build libraries as shared" OFF)

# 找 SFML（如果用系统安装）
find_package(SFML 3 REQUIRED COMPONENTS
    Graphics
    Window
    System
    Audio
)

add_library(GameLib
    src/Game.cpp
    src/Renderer.cpp
    src/Input.cpp
    src/Scene.cpp
)

# ⭐关键：链接 SFML
target_link_libraries(GameLib PUBLIC
    SFML::Graphics
    SFML::Window
    SFML::System
    SFML::Audio
)

# 定义一个名为main的可执行目标，源文件为src/main.cpp
add_executable(main src/main.cpp)

# 头文件目录 
target_include_directories(GameLib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_include_directories(main PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

#链接库 
target_link_libraries(main PRIVATE GameLib)

# 设置C++标准为C++17
target_compile_features(main PRIVATE cxx_std_17)

#在Windows系统中（使用MinGW-w64）构建时
#静态链接需要将1ibstdc++、libgcc编译进可执行文件中，避免运行时依赖外部DLL
if(WIN32 AND (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR
(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")))
    target_link_libraries(main PRIVATE
        -static-libgcc
        -static-libstdc++
    )
endif()

#复制data文件夹到可执行文件输出目录
add_custom_command(TARGET main POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/data # 源路径：项目根目录下的data文件夹
    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/data #目标路径：build/bin/所在目录下的 data 文件夹
    COMMENT "Copying data directory to output directory"
)

# 启动测试功能
enable_testing()
# 添加测试，运行可执行文件main.exe
add_test(NAME SmokeTest
        COMMAND main
        WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

# 打包配置
install(TARGETS main RUNTIME DESTINATION .)
# 安装data目录下的所有素材文件
install(DIRECTORY "data/"
        DESTINATION "data"
        FILES_MATCHING PATTERN "*"
)
# 设置CPack打包信息
set(CPACK_PACKAGE_NAME "HelloWorld")
set(CPACK_PACKAGE_VERSION "1.0.0")
set(CPACK_PACKAGE_VENDOR "HDU")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Hello-World")
set(CPACK_PACKAGE_CONTACT "contact@example.com")
include(CPack)