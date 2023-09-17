
import numpy as np
import cv2 as cv


from LearningSLAM_ROS.Codes.utils.read_pcb_ply import *

def rgb_pcb2dom(pcd_file_path, resolution=0.05):
    # 读取pcd文件
    points, xy_minmax = read_pcd(pcd_file_path)
    x_min, x_max, y_min, y_max = xy_minmax
    print(x_min, x_max, y_min, y_max)

    # 网格分辨率
    # resolution = 0.05

    # 计算网格数量
    w = int(abs(x_max - x_min) / resolution) + 1
    h = int(abs(y_max - y_min) / resolution) + 1
    print(h, w)

    dom = np.empty((w, h, 3))
    dem = np.empty((w, h, 1))

    for i in range(points.shape[0]):
        # 计算图像所在行列
        x = int(abs(points[i][0] - x_min) / resolution)
        y = int(abs(points[i][1] - y_min) / resolution)
        z = points[i][2]
        if z > dem[x][y] :
            dem[x][y] = z
            dom[x][y] = (points[i][3], points[i][4], points[i][5]) # rgb

    return dom, dem


if __name__ == "__main__":
    # ply_file_path = "/home/bingqiangzhou/r3live_output/textured_mesh.ply"
    # points, xy_minmax = read_ply_cloud(ply_file_path)
    # x_min, x_max, y_min, y_max = xy_minmax
    
    pcd_file_path = "/home/bingqiangzhou/r3live_output/rgb_pt.pcd"
    dom, dem = rgb_pcb2dom(pcd_file_path, resolution=0.05)
    cv.imwrite("dom.png", dom)
    cv.imwrite("dem.png", dem)

    

