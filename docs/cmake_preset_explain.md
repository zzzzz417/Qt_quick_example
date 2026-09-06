# CMake Preset 内置宏说明

本项目的公共和用户预设使用 schema 6，需要 CMake 3.25 或更新版本。`CMakeUserPresets.json` 自动包含公共预设，同类预设名称必须在两个文件之间保持唯一；本机配置统一使用 `local-` 前缀。

| 宏名称 | 含义 / 展开值 | 示例 / 说明 |
| :--- | :--- | :--- |
| `${sourceDir}` | 项目源码根目录（顶层 `CMakeLists.txt` 所在路径） | `C:/Users/.../innoStudio` |
| `${sourceParentDir}` | 源码根目录的上一级目录 | `C:/Users/...` |
| `${sourceDirName}` | 源码根目录的文件夹名称 | `innoStudio` |
| `${presetName}` | 当前正在执行的 Preset 名称 | `default-make` |
| `${generator}` | 当前配置使用的 Generator 生成器 | `Unix Makefiles` / `Ninja` |
| `${hostSystemName}` | 当前运行系统名称 | `Windows` / `Linux` / `Darwin` |
| `${fileDir}` | 当前 `CMakePresets.json` 所在的绝对目录 | `C:/Users/.../innoStudio` |
| `${dollar}` | 字面量字符 `$`（用于转义） | `$` |
| `$env{VAR}` | 环境变量 `VAR` 的值 | `$env{HOME}` / `$env{USERPROFILE}` |
| `$penv{VAR}` | 启动 CMake 进程时的父进程环境变量 | 防止被 Preset 内部修改的原始环境变量 |

## buildPresets 核心字段说明

`buildPresets` 用于控制实际编译阶段的行为（对应 `cmake --build --preset <name>`），必须通过 `configurePreset` 字段关联对应的配置预设。

| 字段 | 类型 | 是否必填 | 说明 | 示例 |
| :--- | :--- | :--- | :--- | :--- |
| `name` | string | **是** | Build Preset 的唯一标识名称，供命令行调用 | `"debug"` |
| `configurePreset` | string | **是** | 绑定的 `configurePresets` 的 `name` | `"windows-mingw-debug"` |
| `displayName` | string | 否 | 在 IDE / UI 中显示的友好名称 | `"MinGW Debug Build"` |
| `description` | string | 否 | 当前构建预设的描述信息 | `"使用 8 线程并行全量编译"` |
| `jobs` | int | 否 | 并行构建任务数（相当于 `cmake --build --parallel 8`） | `8` |
| `targets` | string / array | 否 | 指定只编译某些目标 target（缺省时为全量编译） | `["InnoStudio"]` |
| `cleanFirst` | bool | 否 | 编译前是否先执行清理（clean） | `true` / `false` |
| `verbose` | bool | 否 | 是否输出详细编译命令日志（相当于 `make VERBOSE=1`） | `true` / `false` |
| `configuration` | string | 否 | 针对多配置生成器（如 VS/Xcode）指定配置类型 | `"Debug"` / `"Release"` |
| `inherits` | string / array | 否 | 继承其他 build preset 的配置项 | `"base-build"` |

## 命令行调用规则

| 命令形式 | 阶段与作用 | `--preset` 参数填什么 | 对应 JSON 区块 |
| :--- | :--- | :--- | :--- |
| `cmake --preset <name>` | **配置 / 生成阶段**（生成 Makefile/Ninja） | 填 `configurePresets` 的 `name` | `"configurePresets"` |
| `cmake --build --preset <name>` | **编译 / 链接阶段**（执行实际编译） | 填 `buildPresets` 的 `name` | `"buildPresets"` |
| `cmake --workflow --preset <name>` | **顺序执行配置、构建等步骤** | 填 `workflowPresets` 的 `name` | `"workflowPresets"` |

公共 `release` 构建预设指定 `configuration: Release` 和 `targets: ["install"]`，因此多配置生成器也会安装 Release。`local-release` 继承这些设置，并关联本机的配置预设。
