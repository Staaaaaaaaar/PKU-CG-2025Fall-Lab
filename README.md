# Particle

北京大学图形学课程第三次小作业代码。

## 作业要求

- **基本要求**:
  1. 编写一个粒子系统的实现程序。
  2. 可以通过键盘/鼠标输入交互地控制视点。
  3. 可以交互地设置光源的位置（甚至设置光源的朝向，即方向光）。
- **考查知识点**: 粒子系统，光照明和真实感绘制。

---

### 评分标准 (共5分)
- **2分**: 自主设计并实现至少一种动态粒子特效（本项目实现了土星环）。
- **1分**: 粒子特效达到实时的效果，粒子数目不少于10000粒子。
- **1分**: 交互地调整光源，不同光源产生效果不同的光照。
- **1分**: 粒子特效渲染的外观具有较高的真实感（通过粒子的运动形态和粒子的渲染效果体现）。

### 加分项
- 设计更丰富种类的粒子特效。
- 配置丰富的背景场景与粒子特效相配合形成更加逼真的效果。
- 通过图形交互界面灵活调整和控制粒子数目、粒子运动形态等各种粒子相关参数。
- 提供由多个脚本形式来描述多种粒子特效且可以解析脚本并生成可供选择的特效。
- 设计和实现更复杂的粒子光照效果。

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
    git checkout particle
    code . 
    ```

3.  **配置和编译**:
    *   用 VS Code 打开项目文件夹后，CMake Tools 扩展会自动开始配置项目。
    *   选择一个检测到的 Visual Studio 编译器 Kit。
    *   配置完成后，点击 VS Code 底部状态栏的 **`Build`** 按钮（或按 `F7`）来编译项目。

4.  **运行程序**:
    *   编译成功后，在状态栏中选择 `particle` 作为启动目标。
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
    ./src/particle
    ```

## 使用说明

- **相机交互**:
  - **旋转视角 (Orbit)**: 按住鼠标 **左键** 并拖动，可以围绕场景中心旋转相机。
  - **缩放视角 (Zoom)**: 滚动鼠标 **滚轮**，可以拉近或推远相机。

- **GUI 控制面板**:
  程序运行时会显示一个 "Settings" 窗口，提供以下交互功能：
  - **Light Settings**:
    - **`Directional Light`**: 勾选此框可在 **方向光** 和 **点光源** 之间切换。
    - **`Light Direction`**: 当使用方向光时，通过滑块调整光照方向。
    - **`Light Position`**: 当使用点光源时，通过滑块调整光源在世界空间中的位置。
  - **Particle Settings**:
    - **`Particle Count`**: 通过滑块实时调整星环中粒子的数量（从 10,000 到 1,000,000）。
  - **Planet Settings**:
    - **`Planet Rotation (deg/s)`**: 通过滑块调整行星的自转速度。
  - **Camera**:
    - 展开此项可查看和微调相机的详细参数。