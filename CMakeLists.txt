cmake_minimum_required(VERSION 3.25)
project(InnoStudio VERSION 1.0.0 LANGUAGES C CXX)

# Qt 6.5 是最低支持版本，允许使用更新的 Qt 6 版本构建。
set(INNOSTUDIO_QT_MIN_VERSION 6.5)

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

find_package(Qt6 ${INNOSTUDIO_QT_MIN_VERSION} REQUIRED COMPONENTS
        Core
        Gui
        Qml
        Quick
        QuickControls2
)

# 以最低支持的 Qt 版本启用标准设置与 QML 策略。
qt_standard_project_setup(REQUIRES ${INNOSTUDIO_QT_MIN_VERSION})

if(QT_KNOWN_POLICY_QTP0004)
    qt_policy(SET QTP0004 NEW)
endif()

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

# Linux 下配置 $ORIGIN RPATH，确保分发后优先使用打包自带的 lib 库
if(UNIX AND NOT APPLE)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        INSTALL_RPATH "$ORIGIN/../lib:$ORIGIN"
    )
endif()

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

# 修复 macOS-14 (Xcode 15+) 缺失 AGL.framework 导致的链接错误
if(APPLE)
    set(STUB_FRAMEWORK_DIR "${CMAKE_BINARY_DIR}/Frameworks")
    file(MAKE_DIRECTORY "${STUB_FRAMEWORK_DIR}/AGL.framework")

    # 编译一个合法的 Mach-O 动态库桩（避免 0 字节文件导致 ld 段错误崩溃）
    execute_process(
        COMMAND clang -dynamiclib -install_name @rpath/AGL.framework/AGL -o "${STUB_FRAMEWORK_DIR}/AGL.framework/AGL" -xc /dev/null
        OUTPUT_QUIET ERROR_QUIET
    )
    target_link_options(${PROJECT_NAME} PRIVATE "-F${STUB_FRAMEWORK_DIR}")
endif()

# 安装规则
install(TARGETS ${PROJECT_NAME}
        BUNDLE DESTINATION .
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
)

# 为所有系统（Windows, Linux, macOS）自动部署 Qt 动态库与 QML 模块依赖
qt_generate_deploy_qml_app_script(
    TARGET ${PROJECT_NAME}
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
