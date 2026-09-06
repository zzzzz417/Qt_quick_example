# innoStudio

C++20 / Qt Quick 模板。

## 版本约定

- Qt 使用 **6.5 LTS 系列**，由 `CMakeLists.txt` 中的 `INNOSTUDIO_QT_SERIES` 定义。允许 6.5.x 补丁更新，配置时拒绝其他次版本系列。
- CMake **3.25 或更新版本**；公共和用户预设都使用 schema 6。顶层构建脚本和公共预设声明相同的最低要求。
- 应用版本只在 `project(InnoStudio VERSION ...)` 中定义。C++ 的应用版本和窗口标题使用完整版本；QML 模块使用其中的主、次版本。

## 本地构建

1. 安装 Qt 6.5.x 桌面 Kit、CMake、Ninja 和匹配的 C++ 编译器。
2. 将 `CMakeUserPresets.json.example` 复制为 `CMakeUserPresets.json`。
3. 将 `local-base` 的 `CMAKE_PREFIX_PATH` 改为实际 Qt Kit 目录。若 Ninja 未加入 `PATH`，在该预设的 `cacheVariables` 中增加 `CMAKE_MAKE_PROGRAM`，填入 Ninja 可执行文件的完整路径。
4. 执行：

```sh
cmake --list-presets=all
cmake --workflow --preset local-debug
cmake --workflow --preset local-release
```

用户预设使用 `local-` 前缀，并继承公共预设。它们不能与公共配置预设重名。`CMakeUserPresets.json` 已被 Git 忽略，仅保存本机配置。

默认示例使用单配置 Ninja。macOS 的 Debug 应用为 `build/local-debug/bin/InnoStudio.app`，Release 工作流会安装到 `dist/InnoStudio.app` 并收集 Qt / QML 依赖；可以在 Finder 中双击应用包，或执行：

```sh
open build/local-debug/bin/InnoStudio.app
# 或打开已安装的 Release 应用
open dist/InnoStudio.app
```

Windows 产物为 GUI 子系统的 `InnoStudio.exe`，启动时不弹出控制台；Release 安装到 `dist/bin` 并收集 Qt / QML 依赖。Linux 可执行文件名为 `InnoStudio`，安装到 `dist/bin`。

如果 Qt 与生成器已可被 CMake 自动发现，也可以直接使用公共 `debug` / `release` 工作流。

## 多配置生成器

可将用户预设的 `generator` 改为 `Ninja Multi-Config`、Visual Studio 或 Xcode。切换生成器时使用新构建目录，或确认旧产物不再需要后清理对应目录。Windows 编译器需与 Qt Kit 匹配。

构建预设通过 `configuration` 显式选择 Debug / Release。多配置产物位于 `bin/Debug` 或 `bin/Release`，其中 macOS 产物为 `.app` 应用包。安装位置与单配置构建相同。`CMAKE_BUILD_TYPE` 只用于单配置生成器。

预设字段与命令说明见 [docs/cmake_preset_explain.md](docs/cmake_preset_explain.md)。
