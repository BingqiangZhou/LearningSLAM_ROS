
# 记录平时写的一些SLAM相关的代码

## 一、文件读取

### 1.1、读取pcb文件与ply文件

[utils/read_pcb_file.py](./utils/read_pcb_file.py)，包含`read_pcd`、`read_ply`函数。

## 二、算法

### 2.1、根据pcb生成dom与dem

实现过程：

- 根据rgb点云(`xyzrgb`)中xy坐标与设置的网格分辨率，计算图像大小，并初始化`dom`（`w, h, 3`）、`dem`（`w, h`）。
- 遍历所有点云，通过当前点`xy`坐标，计算所在图像行和列，如果当前点`z`大于`dem`中对于行列的中保存的值，则更新为当前点`z`，同时更新对应行列`dom`的`rgb`值。

[pcd2dom.py](./pcd2dom.py)