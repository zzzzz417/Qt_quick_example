cmake_minimum_required(VERSION 3.25)
project(InnoStudio VERSION 1.0.0 LANGUAGES C CXX)

# 固定 Qt LTS 系列，允许该系列的补丁更新。
set(INNOSTUDIO_QT_SERIES 6.5)
string(REPLACE "." ";" qt_version_parts "${INNOSTUDIO_QT_SERIES}")
list(GET qt_version_parts 0 qt_major_version)
list(GET qt_version_parts 1 qt_minor_version)
math(EXPR qt_next_minor_version "${qt_minor_version} + 1")
set(INNOSTUDIO_QT_VERSION_RANGE
        "${INNOSTUDIO_QT_SERIES}...<${qt_major_version}.${qt_next_minor_version}")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB_RECURSE PROJECT_SOURCE_FILES CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cxx"
)
file(GLOB_RECURSE PROJECT_HEADER_FILES CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hh"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hxx"
)

file(GLOB_RECURSE PROJECT_QML_FILES CONFIGURE_DEPENDS
        RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
        "${CMAKE_CURRENT_SOURCE_DIR}/qml/*.qml"
        "${CMAKE_CURRENT_SOURCE_DIR}/qml/*.js"
        "${CMAKE_CURRENT_SOURCE_DIR}/qml/*.mjs"
)

if(PROJECT_SOURCE_FILES STREQUAL "")
    message(FATAL_ERROR "未在 src 目录下检测到源码文件，请至少添加一个 .cpp/.c 文件。")
endif()

find_package(Qt6 ${INNOSTUDIO_QT_VERSION_RANGE} REQUIRED COMPONENTS
        Core
        Gui
        Qml
        Quick
        QuickControls2
)

# 使用所选 Qt LTS 系列的标准设置与 QML 策略。
qt_standard_project_setup(REQUIRES ${INNOSTUDIO_QT_SERIES})

# 可执行文件与 QML 模块目录分别输出，避免名称冲突。
qt_add_executable(${PROJECT_NAME})
set_target_properties(${PROJECT_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin"
        WIN32_EXECUTABLE TRUE
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_GUI_IDENTIFIER "com.innostudio.app"
        MACOSX_BUNDLE_BUNDLE_NAME "${PROJECT_NAME}"
        MACOSX_BUNDLE_BUNDLE_VERSION "${PROJECT_VERSION}"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION}"
)
target_compile_definitions(${PROJECT_NAME} PRIVATE
        INNOSTUDIO_APP_VERSION="${PROJECT_VERSION}"
)

target_sources(${PROJECT_NAME} PRIVATE
        ${PROJECT_SOURCE_FILES}
        ${PROJECT_HEADER_FILES}
)

target_include_directories(${PROJECT_NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

qt_add_qml_module(${PROJECT_NAME}
        URI InnoStudio
        VERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
        RESOURCE_PREFIX /qt/qml
        QML_FILES
        ${PROJECT_QML_FILES}
)

# 链接 Qt6 库
target_link_libraries(${PROJECT_NAME} PRIVATE
        Qt6::Core
        Qt6::Gui
        Qt6::Qml
        Qt6::Quick
        Qt6::QuickControls2
)

# Windows UTF-8 支持
if(WIN32)
    if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(${PROJECT_NAME} PRIVATE -finput-charset=UTF-8 -fexec-charset=UTF-8)
    endif()
endif()

# 安装规则
install(TARGETS ${PROJECT_NAME}
        BUNDLE DESTINATION .
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
)

# 为 macOS 应用包和 Windows 程序部署 Qt / QML 依赖。
if(APPLE OR WIN32)
    qt_generate_deploy_qml_app_script(
        TARGET ${PROJECT_NAME}
        OUTPUT_SCRIPT deploy_script
        NO_UNSUPPORTED_PLATFORM_ERROR
    )
    install(SCRIPT ${deploy_script})
endif()
