# **Live2D on STM32**

## 简介
### 在STM32上运行Live2D  
---------------------------
## 硬件
### 本项目使用开源的硬件进行开发
### [开发板和LCD模块的作者的立创开源硬件平台的个人主页](https://oshwhub.com/micespring)
### [开发板的项目主页](https://oshwhub.com/micespring/stm32h743ii-coreboard)
### [LCD模块的项目主页](https://oshwhub.com/micespring/IPS-035-16B-320x480-CTP-A_copy)
---------------------------
## 环境搭建与clone项目到本地
### 1. 下载并安装 [Git](https://git-scm.com/downloads) 、 [RT-Thread Studio](https://www.rt-thread.org/page/studio.html) 、 [STM32CubeMX](https://www.st.com/zh/development-tools/stm32cubemx.html) 和 [STM32CubeProg](https://www.st.com/zh/development-tools/stm32cubeprog.html) （如果已经安装可以跳过此步骤）
### 2. 进入 [RT-Thread 嵌入式开源社区](https://club.rt-thread.org/) 注册账号用于第一次启动RT-Thread Studio时登录（如果已有账号可以跳过此步骤）
### 3. 打开Windows PowerShell，切换到你想存放项目工程的目录
### 4. 执行 `git clone https://github.com/QPLover/live2d_on_stm32.git` 从github上下载项目到本地
### 5. 执行 `cd live2d_on_stm32` 切换到项目工程的目录，在项目工程的目录下执行 `git submodule init` 后再执行 `git submodule update` 以下载项目里的子模块
---------------------------
## 编译
### 1. 启动RT-Thread Studio，进入SDK Manager（RT-Thread Studio界面上方的抽屉形状的图标） > ToolChain_Support_Packages > NU_Tools_for_ARM_Embedded_Processors下安装10.2.1版本的工具链
### 2. 右键单击项目资源管理器空白页，在弹出的“导入”选项中导入刚才clone好的工程
### 3. 右键点击工作空间里刚刚导入的工程，在弹出菜单中点击更新软件包
### 4. 点击编译按钮（RT-Thread Studio界面上方的锤子形状的图标）即可完成工程的编译，点击下载按钮（RT-Thread Studio界面上方的带有红色向下箭头的磁盘形状的图标）即可将编译生成的二进制文件下载进目标板执行