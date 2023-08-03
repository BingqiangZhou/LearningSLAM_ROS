<!--
 * @Author       : Bingqiang Zhou
 * @Date         : 2023-08-03 20:42:54
 * @LastEditors  : Bingqiang Zhou
 * @LastEditTime : 2023-08-04 02:29:52
 * @Description  : 
-->
# 开源项目R3live的配置与运行

R3live是香港大学MARS实验室以Livox雷达为核心做的多传感器融合的SLAM方案，开源项目地址：[https://github.com/hku-mars/r3live](https://github.com/hku-mars/r3live)，以下是R3live项目的配置与运行的记录。

- [开源项目R3live的配置与运行](#开源项目r3live的配置与运行)
  - [一、安装ROS](#一安装ros)
    - [1.1、配置Ubuntu软件库](#11配置ubuntu软件库)
    - [1.2、设置软件源](#12设置软件源)
    - [1.3、添加软件包密钥](#13添加软件包密钥)
    - [1.4、安装ros-noetic](#14安装ros-noetic)
    - [1.5、配置ROS环境](#15配置ros环境)
    - [1.6、安装构建ROS包的依赖包](#16安装构建ros包的依赖包)
    - [1.7、安装R3live依赖的ROS包](#17安装r3live依赖的ros包)
  - [二、安装livox的ROS驱动程序](#二安装livox的ros驱动程序)
    - [2.1、编译安装livox-SDK驱动](#21编译安装livox-sdk驱动)
    - [2.2、验证livox-SDK驱动的安装](#22验证livox-sdk驱动的安装)
    - [2.3、创建ROS工作路径](#23创建ros工作路径)
    - [2.4、编译安装livox的ROS驱动程序](#24编译安装livox的ros驱动程序)
    - [2.5、配置ROS工作路径的运行设置到系统环境](#25配置ros工作路径的运行设置到系统环境)
  - [三、安装CGAL和pcl\_viewer](#三安装cgal和pcl_viewer)
  - [四、安装OpenCV](#四安装opencv)
    - [4.1、下载OpenCV源码](#41下载opencv源码)
    - [4.2、生成makefile文件](#42生成makefile文件)
    - [4.3、编译OpenCV](#43编译opencv)
    - [4.4、安装OpenCV](#44安装opencv)
    - [4.5、将OpenCV的库添加到系统路径](#45将opencv的库添加到系统路径)
    - [4.6、将OpenCV库配置到系统环境中](#46将opencv库配置到系统环境中)
    - [4.7、验证OpenCV 4.x库路径是否配置正确到系统配置中](#47验证opencv-4x库路径是否配置正确到系统配置中)
    - [4.8、确保OpenCV 4.x版本正确被R3live找到](#48确保opencv-4x版本正确被r3live找到)
    - [4.9、OpenCV版本太多配置冲突，卸载OpenCV重新安装（未尝试，可能存在风险）](#49opencv版本太多配置冲突卸载opencv重新安装未尝试可能存在风险)
  - [五、编译运行R3live](#五编译运行r3live)
    - [5.1、编译安装R3live](#51编译安装r3live)
    - [5.2、下载rosbag数据文件](#52下载rosbag数据文件)
    - [5.3、运行R3live](#53运行r3live)
    - [5.4、运行时，可能遇到的问题](#54运行时可能遇到的问题)
      - [5.4.1、Rviz进程终止运行](#541rviz进程终止运行)
      - [5.4.2、内存分配失败报错（内存不够）](#542内存分配失败报错内存不够)
  - [附页](#附页)
    - [个人电脑配置与环境](#个人电脑配置与环境)
    - [参考链接](#参考链接)

## 一、安装ROS

### 1.1、配置Ubuntu软件库

- 打开`Software & Updates`(紫色图标)，注意不是右边的`Software Update`(灰色图标)

![Software and updates](RunR3liveProject/Pictures/Ubuntu%20Software%20and%20Update.png)

- 将下载来源改为阿里云，如下图所示

![Configure your Ubuntu repositories](./RunR3liveProject/Pictures/Configure%20your%20Ubuntu%20repositories.png)

### 1.2、设置软件源

将`packages.ros.org`加入到软件源

```bash
sudo sh -c 'echo "deb http://packages.ros.org/ros/ubuntu $(lsb_release -sc) main" > /etc/apt/sources.list.d/ros-latest.list'
```

### 1.3、添加软件包密钥

这里没有安装`curl`软件包，也可以用`wget`下载(ubuntu自带)

```bash
sudo apt install curl

curl -s https://raw.githubusercontent.com/ros/rosdistro/master/ros.asc | sudo apt-key add -
```

如果用`curl`下载失败，可以改用`wget`从`http://packages.ros.org/ros.key`下载

```bash
wget http://packages.ros.org/ros.key
sudo apt-key add ros.key
```

上述处理方法来自：[Unable to locate package, no valid OpenPGP data](https://answers.ros.org/question/157766/unable-to-locate-package-no-valid-openpgp-data/)

### 1.4、安装ros-noetic

注意需要先更新一下软件包索引，另外，这里为了减少额外ROS包的安装，下载的内容最全的ROS版本`ros-noetic-desktop-full`。

其他只有部分内容的`ros-noetic-desktop`，`ros-noetic-ros-base`是否也可以，需要尝试，个人觉得`ros-noetic-desktop`应该可以，但`ros-noetic-ros-base`不行，r3live需要`RViz`可视化工具，当然自己安装缺少的ROS包一定是可以的。

```bash
sudo apt update 

sudo apt install ros-noetic-desktop-full
```

![ROS package](./RunR3liveProject/Pictures/ROS%20package.png)

### 1.5、配置ROS环境

将ROS环境设置到`~/.bashrc`中

```bash
echo "source /opt/ros/noetic/setup.bash" >> ~/.bashrc

source ~/.bashrc
```

### 1.6、安装构建ROS包的依赖包

安装在自己创建、(build)构建ROS包时依赖的包

```bash
sudo apt install python3-rosdep python3-rosinstall python3-rosinstall-generator python3-wstool build-essential
```

初始化rosdep，这一步在官方文档中存在，但是进行尝试后，由于网络原因会出错，而且跳过这一步好像也没有太影响（可跳过）。
  
```bash
sudo rosdep init

rosdep update
```

这里找到一个解决方案[rosdep init cannot download default sources list](https://github.com/ros-infrastructure/rosdep/issues/791)，但未进行尝试，有兴趣可以试试。

### 1.7、安装R3live依赖的ROS包

```bash
sudo apt-get install ros-noetic-cv-bridge ros-noetic-tf ros-noetic-message-filters ros-noetic-image-transport ros-noetic-image-transport*
```

如果是其他版本的ROS库，对应ROS版本替换`XXX`

```bash
sudo apt-get install ros-XXX-cv-bridge ros-XXX-tf ros-XXX-message-filters ros-XXX-image-transport ros-XXX-image-transport*
```

---

参考链接：

- [官方文档：Ubuntu install of ROS Noetic](https://wiki.ros.org/noetic/Installation/Ubuntu)
- [R3live开源Github库 - README](https://github.com/hku-mars/r3live)
- [Unable to locate package, no valid OpenPGP data](https://answers.ros.org/question/157766/unable-to-locate-package-no-valid-openpgp-data/)
- [rosdep init cannot download default sources list](https://github.com/ros-infrastructure/rosdep/issues/791)

## 二、安装livox的ROS驱动程序

这里不用livox激光雷达，只用数据集跑，还是需要安装livox的ROS驱动程序，如果不进行这一步，当编译R3live程序时，编译不会通过并且会明确告诉你缺少livox的ros驱动程序。

### 2.1、编译安装livox-SDK驱动

livox-SDK的地址：[https://github.com/Livox-SDK/Livox-SDK](https://github.com/Livox-SDK/Livox-SDK)

```bash
git clone https://github.com/Livox-SDK/Livox-SDK.git

cd Livox-SDK

cd build && cmake ..

make

sudo make install
```

### 2.2、验证livox-SDK驱动的安装

在编译安装完成后，可以进行简单的测试，以确定是否成功安装。

在build文件夹下，进入`sample/lidar`或者进入`sample/hub`文件夹下，运行相应的示例程序进行测试。

在虚拟机下运行或者是没有livox雷达，看到如下输出之后，认为SDK已正确安装，这个时候，可以`control + c`退出示例程序了

![livox SDK](./RunR3liveProject/Pictures/livox%20SDK.png)

### 2.3、创建ROS工作路径

在Home路径下，创建ROS工作路径`catkin_ws`(名称可以自定)，编译安装`catkin_make`命令要在`catkin_ws`路径下执行，另外，在ROS工作路径下创建`src`文件夹，将代码放在`src`文件夹内以便管理。

以下命令为循环创建`catkin_ws`与`src`文件夹，`mkdir`命令中的`-p`(`--parents`)参数，表示自动建立好路径中那些尚不存在的文件夹。

```bash
mkdir -p ~/catkin_ws/src
```

### 2.4、编译安装livox的ROS驱动程序

livox的ROS驱动程序：[https//github.com/Livox-SDK/livox_ros_driver](https://github.com/Livox-SDK/livox_ros_driver)

进入到ROS工作路径中`src`文件夹`~/catkin_ws/src`，下载ROS驱动程序代码

```bash
cd ~/catkin_ws/src

git clone https://github.com/Livox-SDK/livox_ros_driver.git
```

回到ROS工作路径中`~/catkin_ws`，编译安装livox的ROS驱动程序

```bash
cd ~/catkin_ws

catkin_make
```

### 2.5、配置ROS工作路径的运行设置到系统环境

```bash
echo "source ~/catkin_ws/devel/setup.sh" >> ~/.bashrc

source ~/.bashrc
```

---
参考链接：

- [香港大学R3live：手把手教你如何编译和运行](https://zhuanlan.zhihu.com/p/555225544)
- [livox-SDK驱动程序Github库 - README](https://github.com/Livox-SDK/Livox-SDK/tree/master)
- [livox的ROS驱动程序Github库 - README](https://github.com/Livox-SDK/livox_ros_driver)

## 三、安装CGAL和pcl_viewer

安装CGAL和pcl_viewer

```bash
sudo apt-get install libcgal-dev pcl-tools
```

如果后续编译R3live时出现报缺失CGAL的错误，则需要将CGAL的依赖全部安装一遍。命令如下：

```bash
sudo apt-get install build-essential
 
sudo apt-get install libpcl-dev pcl-tools    
 
sudo apt-get install libglu1-mesa-dev
 
sudo apt-get install libglut-dev
 
sudo apt-get install libopencv-dev python-opencv
 
sudo apt-get install qttools5-dev-tools
 
sudo apt-get install qt5-default libcgal11v5 libcgal-qt5-11 libcgal-qt5-dev libcgal-ipelets
 
sudo apt-get install geomview
 
sudo apt-get install libgmp-dev libmpfr-dev
 
sudo apt-get install libcgal-dev # CGAL库
```

---
参考链接：

- [R3live开源Github库 - README](https://github.com/hku-mars/r3live)
- [香港大学R3live：手把手教你如何编译和运行](https://zhuanlan.zhihu.com/p/555225544)

## 四、安装OpenCV

### 4.1、下载OpenCV源码

r3live作者提示OpenCV版本需要大于`3.3`，并且已经验证过了`3.3.1`、`3.4.16`、`4.2.1`、`4.5.3`的可行性，我们这里选择`4.5.3`进行安装。

下载OpenCV4.5.3源码并解压

```bash
cd ~

wget https://github.com/opencv/opencv/archive/4.5.3.zip

unzip opencv-4.5.3.zip
```

### 4.2、生成makefile文件

创建并进入到`build`文件夹

```bash
cd ~/opencv-4.5.3

mkdir -p build && cd build
```

使用cmake生成makefile文件

```bash
cmake -D CMAKE_BUILD_TYPE=RELEASE -D CMAKE_INSTALL_PREFIX=/usr/local -D WITH_GTK=ON -D OPENCV_GENERATE_PKGCONFIG=YES ..
```

`CMAKE_BUILD_TYPE=RELEASE`：表示编译发布版本
`CMAKE_INSTALL_PREFIX`：表示生成动态库的安装路径，可以自定义
`WITH_GTK=ON`：这个配置是为了防止GTK配置失败：即安装了libgtk2.0-dev依赖，还是报错未安装
`OPENCV_GENERATE_PKGCONFIG=YES`：表示自动生成OpenCV的pkgconfig文件，否则需要自己手动

### 4.3、编译OpenCV

使用所有系统内核编译OpenCV，提高编译速度。

```bash
make -j$(nproc)
```

这里`$(nproc)`表示系统内核数，如果知道自己的内核数或者想设置为其他内核数，命令格式为`make -j[内核数]`，例如`make -j8`使用8个内核，建议不要设置高于系统内核数，可能会出现系统卡死的情况，另外，如果编译时报错，可以尝试不使用多个内核编译，虽然需要更长的编译时间，但是可以避免一些奇怪的报错。

注意：如果需要重新cmake，请先将build目录下的文件清空，再重新cmake，以免发生错误。

### 4.4、安装OpenCV

```bash
sudo make install
```

### 4.5、将OpenCV的库添加到系统路径

- 方法一：
  
    创建opencv.conf文件

    ```bash
    sudo vim /etc/ld.so.conf.d/opencv.conf
    ```

    将`/usr/local/lib`写入到opencv.conf文件，需要注意的是：这里的`/usr/local/lib`是cmake编译时填的动态库安装路径加上`/lib`

    ```txt
    /usr/local/lib
    ```

    重新加载配置，使得配置的路径生效

    ```bash
    sudo ldconfig
    ```

- 方法二：

    配置ld.so.conf文件

    ```bash
    sudo vim /etc/ld.so.conf
    ```

    在文件中加上一行`include /usr/loacal/lib`，这个路径是cmake编译时填的动态库安装路径加上`/lib`

    ![OpenCV Config](./RunR3liveProject/Pictures/OpenCV%20Config.png)

### 4.6、将OpenCV库配置到系统环境中

在cmake生成makefile文件时，选择了自动生成OpenCV的pkgconfig文件(`OPENCV_GENERATE_PKGCONFIG=YES`)，在动态库安装路径`/usr/local/lib`下的`/pkgconfig`路径可以看到`opencv4.pc`文件。

将`/usr/local/lib/pkgconfig`配置到`/etc/bash.bashrc`配置文件中。

```bash
sudo vim /etc/bash.bashrc
```

在文件末尾加入

```bashrc
PKG_CONFIG_PATH=$PKG_CONFIG_PATH:/usr/local/lib/pkgconfig
export PKG_CONFIG_PATH
```

保存退出，然后执行如下命令使配置生效

```bash
source /etc/bash.bashrc
```

### 4.7、验证OpenCV 4.x库路径是否配置正确到系统配置中

执行以下命令，可看到输出OpenCV版本号`4.5.3`则为配置正确。

```bash
pkg-config --modversion opencv4
```

### 4.8、确保OpenCV 4.x版本正确被R3live找到

执行4.7小节中的命令，可以看到输出OpenCV版本号`4.5.3`，但是R3live中找的是`OpenCV`，因此需要输入以下命令验证

```bash
pkg-config --modversion opencv
```

这时会发现，找不到`opencv.pc`

![opencv pc](./RunR3liveProject/Pictures/opencv%20pc.png)

为了让系统找OpenCV包时，直接找到OpenCV 4.x包（拷贝`opencv4.pc`为`opencv.pc`，并配置`pkg-config`）

```bash
sudo cp /usr/local/lib/pkgconfig/opencv4.pc /usr/local/lib/pkgconfig/opencv.pc
```

让`pkg-config`找到opencv.pc文件

```bash
pkg-config --cflags --libs opencv
```

### 4.9、OpenCV版本太多配置冲突，卸载OpenCV重新安装（未尝试，可能存在风险）

输入以下命令卸载OpenCV相关库，这里可能存在的问题是不只是OpenCV库，其他相关的库也会被删掉，因此，经过这个操作之后，其他库可能出现问题，另外为了正确运行，需要对上面所有涉及到OpenCV库的步骤进行确认，可能会要重新安装库，该方法来自于`r3live`中的[r3live/issues/20#issuecomment-1006419883](https://github.com/hku-mars/r3live/issues/20#issuecomment-1006419883)，如下图所示

```bash
sudo apt-get purge '*opencv*'
```

![r3live issue 20](./RunR3liveProject/Pictures/r3live%20issue%2020.png)

---
参考链接：

- [香港大学R3live：手把手教你如何编译和运行](https://zhuanlan.zhihu.com/p/555225544)
- [Ubuntu 18.04安装配置OpenCV 4.4.0](https://www.jianshu.com/p/59608e83becb)
- [Installation in Linux - docs.opencv.org](https://docs.opencv.org/4.x/d7/d9f/tutorial_linux_install.html)
- [OpenCV configuration options reference - docs.opencv.org](https://docs.opencv.org/4.x/db/d05/tutorial_config_reference.html)
- [多版本opencv管理 --pkg-config](https://blog.csdn.net/weixin_44694952/article/details/121194740)
- [Package opencv was not found in the pkg-config search path. Perhaps you should add the directory containing `opencv.pc' to the PKG_CONFIG_PATH environment variable No package 'opencv' found pkg-config: exit status 1](https://github.com/hybridgroup/gocv/issues/363)
- [r3live/issues/20#issuecomment-1006419883](https://github.com/hku-mars/r3live/issues/20#issuecomment-1006419883)

## 五、编译运行R3live

### 5.1、编译安装R3live

```bash
cd ~/catkin_ws/src

git clone https://github.com/hku-mars/r3live.git

cd ../

catkin_make

# 下面这一句可以不需要了，已在安装livox的ROS驱动程序时添加到~/.bashrc了
# source ~/catkin_ws/devel/setup.bash
```

### 5.2、下载rosbag数据文件

r3live作者给出了一些rosbag数据文件，为了保证链接有效性，这里不放出链接，请直接查看[R3live开源Github库 - README](https://github.com/hku-mars/r3live)或者是[r3live_dataset](https://github.com/ziv-lin/r3live_dataset)

### 5.3、运行R3live

打开一个终端，输入以下命令，启动`r3live`

```bash
roslaunch r3live r3live_bag.launch
```

打开另一个终端，输入以下命令，使用`rosbag`提供数据

```bash
rosbag play YOUR_DOWNLOADED.bag
```

### 5.4、运行时，可能遇到的问题

#### 5.4.1、Rviz进程终止运行

错误信息如下：

```bash
[rviz-3] process has died [pid XXXXX, exit code -6 XXXX......]
```

该错误由OpenGL版本是mesa导致`RViz`启动不了，这时关闭硬件加速器即可。运行如下命令，并在此启动`RViz`即可。

```bash
export LIBGL_ALWAYS_SOFTWARE=1
```

解决方案来自：[ros中启动rviz显示段错误，核心以转储问题 rviz process has died](https://blog.csdn.net/CCCrunner/article/details/124826199)

#### 5.4.2、内存分配失败报错（内存不够）

修改`r3live/src/rgb_map`文件夹中，`pointcloud_rgbd.cpp`中的光流模块image的buffer的分配的大小。

![r3live issue 11](./RunR3liveProject/Pictures/r3live%20issue%2011.png)

上图中说的168行并不准确，因为代码不是最新的，作者已针对内存小于12GB的机器，改小了buffer大小(`1e8`)，大于12GB的设备还是`1e9`。在最新代码(日期20230803)中的177行[r3live/src/rgb_map/pointcloud_rgbd.cpp#L177](https://github.com/hku-mars/r3live/blob/6143a38537f28cb36eb24e9bbe2e39c8f7967157/r3live/src/rgb_map/pointcloud_rgbd.cpp#L177)

这里我改成了`1e7`，在虚拟机中8GB内存也可以正确跑出来结果来了。

需要注意的是：修改之后，需要编译安装`R3live`（[5.1、编译安装R3live](#51编译安装r3live)）。

解决方案来自：[r3live/issues/11#issuecomment-1003692169](https://github.com/hku-mars/r3live/issues/11#issuecomment-1003692169)

---
参考链接：

- [R3live开源Github库 - README](https://github.com/hku-mars/r3live)
- [r3live_dataset](https://github.com/ziv-lin/r3live_dataset)
- [ros中启动rviz显示段错误，核心以转储问题 rviz process has died](https://blog.csdn.net/CCCrunner/article/details/124826199)
- [r3live/issues/11#issuecomment-1003692169](https://github.com/hku-mars/r3live/issues/11#issuecomment-1003692169)

## 附页

### 个人电脑配置与环境

- 实体机： Macbook Pro 15(2015 mid)，MacOS 12.6.8版本
- VMware Fusion 虚拟机：Ubuntu 20.04.6版本，4核心，8GB内存
- ROS版本：Noetic（对应于Ubuntu 20.04）
- OpenCV：OpenCV 4.5.3

其他软件版本为Ubuntu系统自带默认版本。

### 参考链接

- [官方文档：Ubuntu install of ROS Noetic](https://wiki.ros.org/noetic/Installation/Ubuntu)
- [R3live开源Github库 - README](https://github.com/hku-mars/r3live)
- [Unable to locate package, no valid OpenPGP data](https://answers.ros.org/question/157766/unable-to-locate-package-no-valid-openpgp-data/)
- [rosdep init cannot download default sources list](https://github.com/ros-infrastructure/rosdep/issues/791)
- [香港大学R3live：手把手教你如何编译和运行](https://zhuanlan.zhihu.com/p/555225544)
- [livox-SDK驱动程序Github库 - README](https://github.com/Livox-SDK/Livox-SDK/tree/master)
- [livox的ROS驱动程序Github库 - README](https://github.com/Livox-SDK/livox_ros_driver)
- [Ubuntu 18.04安装配置OpenCV 4.4.0](https://www.jianshu.com/p/59608e83becb)
- [Installation in Linux - docs.opencv.org](https://docs.opencv.org/4.x/d7/d9f/tutorial_linux_install.html)
- [OpenCV configuration options reference - docs.opencv.org](https://docs.opencv.org/4.x/db/d05/tutorial_config_reference.html)
- [多版本opencv管理 --pkg-config](https://blog.csdn.net/weixin_44694952/article/details/121194740)
- [Package opencv was not found in the pkg-config search path. Perhaps you should add the directory containing `opencv.pc' to the PKG_CONFIG_PATH environment variable No package 'opencv' found pkg-config: exit status 1](https://github.com/hybridgroup/gocv/issues/363)
- [r3live/issues/20#issuecomment-1006419883](https://github.com/hku-mars/r3live/issues/20#issuecomment-1006419883)
- [r3live_dataset](https://github.com/ziv-lin/r3live_dataset)
- [ros中启动rviz显示段错误，核心以转储问题 rviz process has died](https://blog.csdn.net/CCCrunner/article/details/124826199)
- [r3live/issues/11#issuecomment-1003692169](https://github.com/hku-mars/r3live/issues/11#issuecomment-1003692169)
