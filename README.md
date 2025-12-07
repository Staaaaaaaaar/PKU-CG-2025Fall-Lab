# Texture

北京大学图形学课程第四次小作业代码。

## 作业要求

- **基本要求**:
  1. 采用着色器编程，实现基于法向贴图的光照和纹理细节效果。
  2. 可以通过键盘/鼠标输入交互地修改各种参数。
- **考查知识点**: 纹理映射和光照明。

---

### 评分标准 (共5分)
- **1分**: 代码可编译运行，成功加载各项纹理图片数据。
- **1分**: 光源的任意位置和方向的交互设置。
- **1分**: 视点/模型的任意位置和方向的交互设置（模型和相机有一个能动就行）。
- **1分**: 有光照效果的实现（随着光源变化有变化）。
- **1分**: 使用法向贴图进行光照计算。

### 加分项
- 实现 parallax mapping (1 分) 或者 relief mapping (1 分)。
- 其它相关的更酷炫的效果。

## 环境配置

本项目使用 CMake 构建，并依赖于以下第三方库：

- **GLEW**: 用于管理 OpenGL 扩展。
- **GLFW**: 用于创建窗口、上下文和处理输入。
- **GLM**: 用于进行数学（向量、矩阵）运算。
- **ImGui**: 用于创建图形用户界面（GUI）。

所有依赖项已包含在 `third_party` 目录中，并通过 CMake 自动编译和链接，无需手动配置。

您只需要一个支持 C++17 或更高版本的 C++ 编译器（如 GCC, Clang, MSVC）以及 CMake。

## 快速开始

### Windows (使用 VS Code)

1.  **环境准备**:
    *   安装 [Visual Studio Community Edition](https://visualstudio.microsoft.com/vs/community/)，并确保在安装时勾选 **“使用 C++ 的桌面开发”** 工作负载。
    *   安装 [CMake](https://cmake.org/download/) 并确保将其添加到系统的 `PATH` 环境变量中。
    *   在 VS Code 中，安装 [C/C++](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools) 和 [CMake Tools](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cmake-tools) 扩展。

2.  **克隆仓库并打开项目**:
    ```bash
    git clone https://github.com/Staaaaaaaaar/PKU-CG-2025Fall-Lab.git
    cd PKU-CG-2025Fall-Lab
    git checkout texture
    code . 
    ```

3.  **配置和编译**:
    *   用 VS Code 打开项目文件夹后，CMake Tools 扩展会自动开始配置项目。
    *   选择一个检测到的 Visual Studio 编译器 Kit。
    *   配置完成后，点击 VS Code 底部状态栏的 **`Build`** 按钮（或按 `F7`）来编译项目。

4.  **运行程序**:
    *   编译成功后，在状态栏中选择 `texture` 作为启动目标。
    *   点击状态栏中的 **`Launch`** 按钮（或按 `Shift+F5`）来启动程序。

### macOS / Linux

1.  **克隆仓库**:
    ```bash
    git clone https://github.com/Staaaaaaaaar/PKU-CG-2025Fall-Lab.git
    cd PKU-CG-2025Fall-Lab
    ```

2.  **创建 build 目录并构建**:
    ```bash
    mkdir -p build
    cd build
    cmake ..
    ```

3.  **编译**:
    ```bash
    make
    ```

4.  **运行程序**:
    编译成功后，可执行文件将位于 `build/src` 目录下。
    ```bash
    # 在 build 目录下执行
    ./src/texture
    ```

## 使用说明

- **相机交互**:
  - **旋转视角 (Orbit)**: 按住鼠标 **左键** 并拖动，可以围绕场景中心旋转相机。
  - **缩放视角 (Zoom)**: 滚动鼠标 **滚轮**，可以拉近或推远相机。

- **GUI 控制面板**:
  程序运行时会显示一个 "Settings" 窗口，提供以下交互功能：
  - **`Use Normal Map`**: 勾选以启用法线贴图着色，取消勾选时恢复到普通纹理光照。
  - **`Light Position`**: 通过滑块调整点光源在世界空间中的位置。
  - **Camera**:
    - 展开此项可查看并微调相机的详细参数。