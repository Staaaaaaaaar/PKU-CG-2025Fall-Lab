# Hand

北京大学图形学课程第一次小作业代码。

> 注：Assimp 是经过裁剪的版本，只支持 FBX 文件的导入

## 作业要求

- **基本要求**:

  1. 利用示例程序或自行编程实现手的运动，并绘制运动结果。
  2. 可以通过键盘/鼠标输入交互地修改或变换手势。

- **考查知识点**: 图形变换和基本图形编程知识。

---

### 评分标准 (共 5 分)

- **1 分**: 代码可编译运行。
- **1 分**: 手的模型能正常显示。
- **1 分**: 五根手指都能动。
- **1 分**: 完成 3 个动作（有负面意义的手势不得分）。
  - _示例_: 抓握, 挥手, OK, 点赞, “手枪”, 数字 1-9。
- **1 分**: 有键盘、鼠标交互事件。

### 加分项

- 设计丰富的手势和交互方法。

## 环境配置

本项目使用 CMake 构建，并依赖于以下第三方库：

- **GLEW**: 用于管理 OpenGL 扩展。
- **GLFW**: 用于创建窗口、上下文和处理输入。
- **GLM**: 用于进行数学（向量、矩阵）运算。
- **Assimp**: 用于加载 3D 模型。

所有依赖项已包含在 `third_party` 目录中，并通过 CMake 自动编译和链接，无需手动配置。

您只需要一个支持 C++11 或更高版本的 C++ 编译器（如 GCC, Clang, MSVC）以及 CMake。

## 快速开始

### Windows (使用 VS Code)

1.  **环境准备**:

    - 安装 [Visual Studio Community Edition](https://visualstudio.microsoft.com/vs/community/)，并确保在安装时勾选 **“使用 C++ 的桌面开发”** 工作负载。
    - 安装 [CMake](https://cmake.org/download/) 并确保将其添加到系统的 `PATH` 环境变量中。
    - 在 VS Code 中，安装 [C/C++](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools) 和 [CMake Tools](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cmake-tools) 扩展。

2.  **克隆仓库并打开项目**:

    ```bash
    git clone https://github.com/Staaaaaaaaar/PKU-CG-2025Fall-Lab.git
    cd PKU-CG-2025Fall-Lab
    git checkout hand
    code .
    ```

3.  **配置和编译**:

    - 用 VS Code 打开项目文件夹后，CMake Tools 扩展会自动开始配置项目。
    - 选择一个检测到的 Visual Studio 编译器 Kit。
    - 配置完成后，点击 VS Code 底部状态栏的 **`Build`** 按钮（或按 `F7`）来编译项目。

4.  **运行程序**:
    - 编译成功后，在状态栏中选择 `Hand` 作为启动目标。
    - 点击状态栏中的 **`Launch`** 按钮（或按 `Shift+F5`）来启动程序。

### macOS / Linux

1.  **克隆仓库**:

    ```bash
    git clone https://github.com/Staaaaaaaaar/PKU-CG-2025Fall-Lab.git
    cd PKU-CG-2025Fall-Lab
    git checkout hand
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
    ./src/Hand
    ```

## 使用说明

- **鼠标交互**:

  - 按住鼠标 **左键** 并拖动，可以自由旋转手部模型。

- **键盘交互**:
  - **`ESC`**: 关闭窗口。
  - **`SPACE`**: 暂停或继续手势之间的过渡动画。
  - **数字键 `0`-`9`**: 切换不同的手势。
    - `0`: OK 手势
    - `1`: “手枪”手势
    - `2`: 数字 2 / “耶”
    - `3`: 数字 3
    - `4`: 数字 4
    - `6`: 数字 6
    - `7`: 点赞
    - `8`: 数字 8
    - `9`: 数字 9
  - **字母键**:
    - `R`: 石头
    - `S`: 剪刀
    - `P`: 布
