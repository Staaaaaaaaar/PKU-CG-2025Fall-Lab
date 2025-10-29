# Camera

北京大学图形学课程小作业代码。

> 注：Assimp是经过裁剪的版本，只支持FBX文件的导入

## 作业要求

本项目在第一次小作业（实现手的运动）的基础之上，新增了关于相机控制的以下要求：

- **视点自由控制**：可以通过键盘和鼠标输入，交互式地、平滑地控制相机（视点），实现类似飞行模拟的效果。
- **A-B点相机动画**：
  - 支持在场景中任意指定 A、B 两个点作为相机的起始和终止位置。
  - 能够设定相机在 A、B 两点的朝向等参数。
  - 基于四元数机制，实现相机从 A 点到 B 点的连续、平滑的过渡动画。
  - 支持交换 A、B 两点，实现从 B 到 A 的反向平滑过渡。

---

此前的基础要求如下：
- 代码可编译运行
- 手的模型能正常显示
- 五根手指都能动
- 完成3个动作（有负面意义的手势不得分）
  - 举例：抓握，挥手，OK，点赞，“手枪”，数字1-9
- 有键盘、鼠标交互事件

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
    *   安装 [Visual Studio Community Edition](https://visualstudio.microsoft.com/vs/community/)，并确保在安装时勾选 **“使用 C++ 的桌面开发”** 工作负载。这将安装所需的 MSVC 编译器。
    *   安装 [CMake](https://cmake.org/download/) 并确保将其添加到系统的 `PATH` 环境变量中。
    *   在 VS Code 中，从扩展市场安装以下两个扩展：
        *   [C/C++](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools)
        *   [CMake Tools](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cmake-tools)

2.  **克隆仓库并打开项目**:
    ```bash
    git clone https://github.com/Staaaaaaaaar/PKU-CG-2025Fall-Lab.git
    cd PKU-CG-2025Fall-Lab
    code . 
    ```

3.  **配置和编译**:
    *   用 VS Code 打开项目文件夹后，CMake Tools 扩展会自动开始配置项目。
    *   您可能会在 VS Code 底部看到一个提示，要求选择一个 "Kit"（编译器）。选择一个检测到的 Visual Studio 编译器，例如 `Visual Studio Community 2022 Release - amd64`。
    *   配置完成后，点击 VS Code 底部状态栏的 **`Build`** 按钮，或者按快捷键 `F7` 来编译整个项目。

4.  **运行程序**:
    *   编译成功后，点击状态栏中的 **`Launch`** 按钮（通常在 `Build` 按钮旁边），或者按 `Shift+F5` 来启动程序。
    *   CMake Tools 会自动选择 `Hand` 作为启动目标。如果不是，您可以在状态栏点击目标名称进行切换。

### macOS / Linux

1.  **克隆仓库**:
    ```bash
    git clone https://github.com/Staaaaaaaaar/PKU-CG-2025Fall-Lab.git
    cd PKU-CG-2025Fall-Lab
    ```

2.  **创建 build 目录并构建**:
    ```bash
    mkdir build
    cd build
    cmake ..
    ```

3.  **编译**:
    在 `build` 目录下执行 `make` 命令。
    ```bash
    make
    ```

4.  **运行程序**:
    编译成功后，可执行文件将位于 `build/bin` 或 `build/src` 目录下。
    ```bash
    # 在 build 目录下执行
    ./src/Hand
    ```

## 使用说明

- **相机交互**:
  - **旋转视角**: 按住鼠标 **左键** 并拖动，可以自由旋转相机视角（改变俯仰角和偏航角）。
  - **平移相机**: 按住鼠标 **右键** 并拖动，可以在相机的本地平面上进行平移。
  - **缩放视角**: 滚动鼠标 **滚轮**，可以向前或向后移动相机，实现缩放效果。

- **手势控制 (键盘)**:
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

- **相机 A-B 点过渡 (GUI)**:
  程序右侧的 "Control Panel" 提供了相机动画控制功能。
  - **设置航点**:
    - **`Set A`**: 将当前相机的位姿（位置和朝向）保存为航点 A。
    - **`Set B`**: 将当前相机的位姿保存为航点 B。
    - 你可以通过 `InputFloat3` 控件手动修改 A、B 两点的具体参数。
  - **执行过渡**:
    - **`Go A->B`**: 触发相机从航点 A 到航点 B 的平滑过渡动画。
    - **`Go B->A`**: 触发相机从航点 B 到航点 A 的平滑过渡动画。
  - **调整动画**:
    - **`Anim Duration (s)`**: 通过滑块调整 A-B 点过渡动画的持续时间。
  - **重置相机**:
    - **`Reset Camera`**: 将相机恢复到默认的初始位置和朝向。