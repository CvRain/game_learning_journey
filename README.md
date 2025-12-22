# 游戏开发学习之旅
**学习游戏开发的冒险旅途**

## 项目结构

```
game_learning_journey/
├── hello_sdl/          # SDL 学习模块
│   └── first_window/   # 第一个窗口项目
├── hello_entt/         # EnTT (ECS) 学习模块（待添加）
├── hello_vulkan/       # Vulkan 学习模块（待添加）
└── vcpkg/              # vcpkg 包管理器
```

## 依赖管理

本项目使用 vcpkg 的 manifest 模式管理第三方库。依赖配置在根目录的 `vcpkg.json` 文件中。

## 构建说明

### 前提条件

1. 安装 CMake (>= 3.20)
2. 配置 vcpkg

### 初始化 vcpkg（首次使用）

```bash
cd vcpkg
./bootstrap-vcpkg.sh
```

### 构建项目

```bash
# 在项目根目录创建 build 目录
mkdir -p build && cd build

# 配置项目（使用 vcpkg）
cmake .. -DCMAKE_TOOLCHAIN_FILE=../vcpkg/scripts/buildsystems/vcpkg.cmake

# 编译
cmake --build .

# 运行
./bin/first_window
```

## 当前进度

- [x] hello_sdl/first_window - SDL 初始化和 Hello World

## 后续计划

- [ ] hello_sdl/draw_triangle - 绘制简单图形
- [ ] hello_entt - 学习 ECS 架构
- [ ] hello_vulkan - Vulkan 图形 API
