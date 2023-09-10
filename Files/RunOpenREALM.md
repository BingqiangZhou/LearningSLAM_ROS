


# OpenREALM部署过程记录-230910

该文档记录了在`Ubuntu 18.04`部署[OpenREALM](https://github.com/laxnpander/OpenREALM)的主要过程。在部署过程中主要参考了博客-[基于Arm64的open_realm docker环境搭建-CSDN](https://blog.csdn.net/IT_forlearn/article/details/130852649)，大致部署步骤如下：

<!-- TOC -->

- [OpenREALM部署过程记录-230910](#openrealm部署过程记录-230910)
  - [一、进行准备工作](#一进行准备工作)
    - [1.1、切换软件源](#11切换软件源)
    - [1.2、安装需要用到的工具](#12安装需要用到的工具)
    - [1.3、升级CMake](#13升级cmake)
  - [二、下载安装OpenREALM](#二下载安装openrealm)
    - [2.1、下载OpenREALM代码并执行部署脚本](#21下载openrealm代码并执行部署脚本)
    - [2.2、编译安装opencv 3.3.1](#22编译安装opencv-331)
    - [2.3、安装Pangolin](#23安装pangolin)
    - [2.4、编译安装G2O](#24编译安装g2o)
    - [2.5、编译安装OpenVSLAM](#25编译安装openvslam)
    - [2.6、编译安装OpenREALM](#26编译安装openrealm)
  - [三、安装ROS环境](#三安装ros环境)
    - [3.1、设置安装源](#31设置安装源)
    - [3.2、设置密钥与公钥](#32设置密钥与公钥)
    - [3.3、安装ROS](#33安装ros)
    - [3.4、将ROS添加到环境变量中](#34将ros添加到环境变量中)
    - [3.4、初始化rosdep](#34初始化rosdep)
  - [四、编译安装OpenREALM\_ROS1\_Bridge](#四编译安装openrealm_ros1_bridge)
    - [4.1 安装ROS相关的依赖包](#41-安装ros相关的依赖包)
    - [4.2、安装OpenREALM\_ROS1\_Bridge](#42安装openrealm_ros1_bridge)
    - [4.3、将ROS工作环境加入到环境变量中](#43将ros工作环境加入到环境变量中)
  - [五、运行OpenREALM](#五运行openrealm)
    - [5.1、下载并解压测试数据](#51下载并解压测试数据)
    - [5.2、修改roslaunch文件中输入数据路径](#52修改roslaunch文件中输入数据路径)
    - [5.3、启动roslaunch](#53启动roslaunch)
    - [5.4、安装CUDA](#54安装cuda)

<!-- /TOC -->

## 一、进行准备工作

### 1.1、切换软件源

软件源建议切换为`中科院Ubuntu源-mirrors.ustc.edu.cn`，设置过程参考：[Ubuntu 源使用帮助 - USTC Mirror Help](https://mirrors.ustc.edu.cn/help/ubuntu.html)

建议科学上网，安装`Clash.for.Windows-x64-linux`（注意：需要有clash格式的订阅），可参考博客：[ubuntu 20.04 下安装使用clash for windows （最简单版本）](https://www.codeprj.com/blog/ed9a301.html)

### 1.2、安装需要用到的工具

```bash
sudo apt update
sudo apt-get install sudo vim git g++ gcc libssl-dev cmake
```

### 1.3、升级CMake

CMake 需要3.15及以上版本，安装过程如下，参考博客：[Ubuntu升级Cmake的正确方式](https://blog.csdn.net/qq_27350133/article/details/121994229)

```bash
wget https://cmake.org/files/v3.22/cmake-3.22.1.tar.gz

tar -xvzf cmake-3.22.1.tar.gz

cd cmake-3.22.1/

chmod 777 ./configure

./configure

sudo make

sudo make install

sudo update-alternatives --install /usr/bin/cmake cmake /usr/local/bin/cmake 1 --force

cmake --version
```

## 二、下载安装OpenREALM


### 2.1、下载OpenREALM代码并执行部署脚本

下载OpenREALM代码，初始化子模块，执行部署脚本`install_deps.sh`，这个脚本运行到检测opencv版本的时候会停下来，需要自己手动安装`OpenCV 3.3.1`，并把剩下的`G2O`、`OpenVSLAM`也安装上。执行的命令参考：[install_deps.sh](https://github.com/laxnpander/OpenREALM/blob/master/tools/install_deps.sh)

```bash
git clone https://github.com/laxnpander/OpenREALM.git

git submodule init

git submodule update --recursive

cd OpenREALM/tools

sudo chmod u+x install_deps.sh

./install_deps.sh
```

### 2.2、编译安装opencv 3.3.1

```bash
cd ~ 

git clone https://github.com/opencv/opencv.git

cd ~/opencv

git init ## 有时候git checkout不成功，需要初始化一下，参考博客： fatal: Not a git repository (or any of the parent directories): .git - https://blog.csdn.net/wenb1bai/article/details/89363588

git checkout 3.3.1

mkdir build && cd build

cmake -D CMAKE_BUILD_TYPE=RELEASE -D BUILD_EXAMPLES=OFF -D BUILD_opencv_apps=OFF -D BUILD_DOCS=OFF -D BUILD_PERF_TESTS=OFF -D      BUILD_TESTS=OFF -D CMAKE_INSTALL_PREFIX=/usr/local ..

make

sudo make install
```

### 2.3、安装Pangolin

OpenVSLAM依赖于Pangolin

```BASH
cd ~

git clone https://github.com/stevenlovegrove/Pangolin.git

git init ## 有时候git checkout不成功，需要初始化一下，参考博客： fatal: Not a git repository (or any of the parent directories): .git - https://blog.csdn.net/wenb1bai/article/details/89363588

git checkout v0.6

cd Pangolin && mkdir build && cd build

cmake .. && make && sudo make install
```


### 2.4、编译安装G2O

```bash
cd ~ && mkdir g2o && cd g2o

git clone https://github.com/RainerKuemmerle/g2o.git

cd g2o

git init

git checkout 9b41a4ea5ade8e1250b9c1b279f3a9c098811b5a

mkdir build && cd build

cmake \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX=/usr/local \
    -DCMAKE_CXX_FLAGS=-std=c++11 \
    -DBUILD_SHARED_LIBS=ON \
    -DBUILD_UNITTESTS=OFF \
    -DG2O_USE_CHOLMOD=OFF \
    -DG2O_USE_CSPARSE=ON \
    -DG2O_USE_OPENGL=OFF \
    -DG2O_USE_OPENMP=ON \
    ..

make -j4

sudo make install
```

### 2.5、编译安装OpenVSLAM

```bash
cd ~ && mkdir openvslam && cd openvslam

git clone https://github.com/laxnpander/openvslam.git

cd openvslam

git submodule init && git submodule update

mkdir build && cd build

cmake \
    -DUSE_PANGOLIN_VIEWER=ON \
    -DINSTALL_PANGOLIN_VIEWER=ON \
    -DUSE_SOCKET_PUBLISHER=OFF \
    -DUSE_STACK_TRACE_LOGGER=ON \
    -DBUILD_TESTS=ON \
    -DBUILD_EXAMPLES=ON \
    ..

make -j4

sudo make install
```

<!-- 报错：c++ error: ‘decay_t’ is not a member of ‘std’

[error: ‘decay_t’ is not a member of ‘std’_orbslam2出现error: ‘decay_t’ is not a member of ‘std_荆黎明的博客-CSDN博客](https://blog.csdn.net/jlm7689235/article/details/122287743) -->


### 2.6、编译安装OpenREALM

在编译OpenREALM的过程中，可能会报找不到`fbow`包的问题（`Target ‘realm_stages’ links to target 'fbow::fobw' but the target was not found.`），下面的命令已改为在编译之前，修改`CMakeLists.txt`，加入查找`fbow`包来避免报错。

```bash
cd ~/OpenREALM/

vim ../CMakeLists.txt

## 在CMakeLists.txt大约25行的地方添加下面这一行，即可编译通过  
find_package(fbow REQUIRED)

mkdir build && cd build

cmake ..

make all

sudo make install
```

## 三、安装ROS环境

ROS安装过程参考官方文档 [Ubuntu install of ROS Melodic](https://wiki.ros.org/melodic/Installation/Ubuntu)。

### 3.1、设置安装源

官方ROS源下载较慢，下面的命令已改为中科大ROS源镜像

```bash
sudo sh -c '. /etc/lsb-release && echo "deb http://mirrors.ustc.edu.cn/ros/ubuntu/ $DISTRIB_CODENAME main" > /etc/apt/sources.list.d/ros-latest.list'
```

### 3.2、设置密钥与公钥 

```bash
# 设置密钥
sudo apt-key adv --keyserver hkp://keyserver.ubuntu.com:80 --recv-key 421C365BD9FF1F717815A3895523BAEEB01FA116
 
# 在更新之前，需要安装公钥，否则无法验证签名
sudo apt-key adv --keyserver keyserver.ubuntu.com --recv-keys F42ED6FBAB17C654
```
 
### 3.3、安装ROS

```bash
sudo apt update

# 推荐安装桌面完全版
sudo apt install ros-melodic-desktop-full
```
 
### 3.4、将ROS添加到环境变量中

```bash
echo "source /opt/ros/noetic/setup.bash" >> ~/.bashrc

source ~/.bashrc
```

### 3.4、初始化rosdep

```bash
# 安装依赖包
sudo apt install python-rosdep python-rosinstall python-rosinstall-generator python-wstool build-essential

# 初始化
sudo rosdep init

rosdep update
```

## 四、编译安装OpenREALM_ROS1_Bridge

### 4.1 安装ROS相关的依赖包

```bash
sudo apt install -y -q ros-$ROS_DISTRO-geographic-msgs ros-$ROS_DISTRO-geodesy ros-$ROS_DISTRO-cv-bridge ros-$ROS_DISTRO-rviz ros-$ROS_DISTRO-pcl-ros

sudo apt install -y -q python-catkin-tools build-essential git wget
```

### 4.2、安装OpenREALM_ROS1_Bridge 

在编译OpenREALM_ROS1_Bridge的过程中，可能会报找不到`fbow`包的问题（`Target ‘realm_stages’ links to target 'fbow::fobw' but the target was not found.`），下面的命令已改为在编译之前，修改`CMakeLists.txt`，加入查找`fbow`包来避免报错。

```bash
cd ~

# 创建工作目录
mkdir -p ~/catkin_ws/src && cd ~/catkin_ws/src

git clone https://github.com/laxnpander/OpenREALM_ROS1_Bridge.git

vim ~/catkin_ws/src/OpenREALM_ROS1_Bridge/realm_ros/CMakeLists.txt
 
# 报错：找不到fbow库导致的错误，在CMakeLists.txt大概75行的位置添加下面这行
find_package(fbow REQUIRED)

cd ..

catkin_make -DCMAKE_BUILD_TYPE=Release
```

### 4.3、将ROS工作环境加入到环境变量中

```bash
echo "source ~/catkin_ws/devel/setup.bash" >> ~/.bashrc

source ~/.bashrc
```

## 五、运行OpenREALM

### 5.1、下载并解压测试数据

下载地址：https://drive.google.com/open?id=1-2h0tasI4wzxZKLBbOz3XbJ7f5xlxlMe

```bash
tar -xvzf open_realm_edm_dataset.tar.gz
```

### 5.2、修改roslaunch文件中输入数据路径

`OpenREALM_ROS1_Bridge/realm_ros/launch`路径下有三个`launch`文件，分别对应三种模式：
- `alexa_gnss.launch`：不需要GPS数据，只做建图（GNSS only mapping）
- `alexa_noreco.launch`：定位与建图（2D mapping with visual SLAM）
- `alexa_reco.launch`：定位建图并重建（2.5D mapping with visual SLAM and surface reconstruction）

在运行前需要修改`launch`文件中`config/input`参数的`value`为测试数据的路径，修改`launch`文件命令如下。

```bash
cd ~/catkin_ws/src/OpenREALM_ROS1_Bridge/realm_ros/launch

vim alexa_gnss.launch
# vim alexa_noreco.launch
# vim alexa_reco.launch
```

如下`launch`文件中部分内容所示，修改`config/input`参数的`value`为测试数据的路径。

```XML
<node pkg="realm_ros" type="realm_exiv2_grabber" name="realm_exiv2_grabber" output="screen">
    <param name="config/id" type="string" value="$(arg camera_id)"/>
    <param name="config/input" type="string" value="PUT THE TEST DATASET'S ABSOLUTE PATH HERE"/>
    <param name="config/rate" type="double" value="10.0"/>
    <param name="config/profile" type="string" value="alexa"/>
</node>
```

### 5.3、启动roslaunch

```bash
# GNSS only mapping:
roslaunch realm_ros alexa_gnss.launch
```

```bash
# 2D mapping with visual SLAM:
roslaunch realm_ros alexa_noreco.launch
```

需要注意的是，运行`alexa_noreco.launch`涉及到重建，OpenREALM库中说到重建依赖于`CUDA`库（[OpenREALM#optional-dependencies](https://github.com/laxnpander/OpenREALM#optional-dependencies)：CUDA is optional but will be mandatory for stereo reconstruction with plane sweep lib）（这个暂时未加入配置CUDA的过程，此次部署是在虚拟机中进行）。

```bash
# 2.5D mapping with visual SLAM and surface reconstruction:
roslaunch realm_ros alexa_reco.launch
```

运行的结果输出在`OpenREALM_ROS1_Bridge/realm_ros/output`文件夹中相应时间目录下的`mosaicing`文件夹中，包括构建的地图、高程图（俯视图）等。


### 5.4、安装CUDA

在NVIDIA官网的[CUDA历史版本](https://developer.nvidia.com/cuda-toolkit-archive)中找到[CUDA 9.0](https://developer.nvidia.com/cuda-90-download-archive)