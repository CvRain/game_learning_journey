# OpenGL Sandbox Project

这是一个用于学习和实验OpenGL的沙盒项目。包含标准的C++项目结构，适用于构建和测试新的图形功能。

## 目录结构说明

- `src/`: 存放所有的源代码文件 (.cpp, .ixx)
- `include/opengl_sandbox/`: 存放公共头文件
- `res/`: 资源目录
  - `shaders/`: GLSL着色器代码
  - `textures/`: 图片纹理资源
- `tests/`: 测试代码
- `doc/`: 项目文档

## 构建说明

本项目使用 CMake 构建，依赖于 vcpkg 管理的库（SDL3, GLM, Glad）。

1. 确保已安装 CMake 和 vcpkg。
2. 配置项目:
   ```bash
   cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=[path/to/vcpkg.cmake]
   ```
3. 构建项目:
   ```bash
   cmake --build build
   ```

## 已添加的依赖

- **SDL3**: 窗口管理与输入处理
- **GLM**: 数学库
- **Glad**: OpenGL加载器

## 待办事项

- [ ] 迁移基础OpenGL代码
- [ ] 添加摄像机类
- [ ] 实现基础渲染循环
