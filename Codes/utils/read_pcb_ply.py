import open3d as o3d
import numpy as np
# import pyvista as pv
import matplotlib.pyplot as plt
from plyfile import PlyData

##reference urls:
# 1) Python讀取pcd點雲文件: https://www.twblogs.net/a/5f0352409644181341a1cb3b
# 2) docs of open3d: http://www.open3d.org/docs/release/getting_started.html
# 3) Generating a Mesh from a Point Cloud in Python: A Data Scientist's Guide:
# 	https://saturncloud.io/blog/generating-a-mesh-from-a-point-cloud-in-python-a-data-scientists-guide/
# 4) PLY - Polygon File Format: https://paulbourke.net/dataformats/ply/
# 5) python：读取PCD、PLY格式点云数据，转换为numpy格式，保存为txt - https://blog.csdn.net/qq_35591253/article/details/127910980
# 6) How to install GLIBCXX_3.4.29 on Ubuntu 20.04? - https://askubuntu.com/questions/1393285/how-to-install-glibcxx-3-4-29-on-ubuntu-20-04
# 7) reading-a-ply-file : plyfile库 - https://python-plyfile.readthedocs.io/en/latest/usage.html#reading-a-ply-file



def read_pcd(file_path, return_xy_minmax=True):
	pcd = o3d.io.read_point_cloud(file_path)
	# print(np.asarray(pcd.points).shape)
	colors = np.asarray(pcd.colors) * 255
	points = np.asarray(pcd.points)
	# print(points.shape, colors.shape)
	# return np.concatenate([points, colors], axis=-1)

	if return_xy_minmax:
		x_min = np.min(points[:, 0])
		x_max = np.max(points[:, 0])
		y_min = np.min(points[:, 1])
		y_max = np.max(points[:, 1])
		# print(x_min, x_max, y_min, y_max)
		return np.concatenate((points, colors), axis=-1),  (x_min, x_max, y_min, y_max)
	return np.concatenate((points, colors), axis=-1)	

def read_ply(filename, return_xy_minmax=True):

	print("read ply data...")
	ply_data = PlyData.read(filename)
	# print(ply_data)
	# points = ply_data['vertex'].data
	# print(ply_data.elements)
	# (PlyElement('vertex', 
	# 	(PlyProperty('x', 'float'), PlyProperty('y', 'float'), 
	# 	PlyProperty('z', 'float'), PlyProperty('red', 'uchar'), 
	# 	PlyProperty('green', 'uchar'), PlyProperty('blue', 'uchar'), 
	# 	PlyProperty('alpha', 'uchar')), count=5502336, comments=[]), 
	# PlyElement('face', 
	# 	(PlyListProperty('vertex_indices', 'uchar', 'int'),), count=10852871, comments=[]))
	
	print("search vertex element...")
	# 找到vertex的属性
	index = 0
	for ele in ply_data.elements:
		if(ele.name == "vertex"):
			break

	print("search vertex element properties...")
	# 查看Vertex属性
	prop_names = []
	for prop in ply_data.elements[index].properties:
		prop_names.append(prop.name)

	if return_xy_minmax:
		print("search vertex xy-minmax index...")
		# 获取xy坐标最大最小值
		points = ply_data['vertex'].data
		x_min = np.min(points['x'])
		x_max = np.max(points['x'])
		y_min = np.min(points['y'])
		y_max = np.max(points['y'])

	print("get pt coulds")
	pt_coulds = np.empty((points.shape[0], len(prop_names)))
	for i in range(points.shape[0]):
		for j, name in enumerate(prop_names):
			pt_coulds[i][j] = np.array(points[name][i])
		# if(i == 0):
		# 	print(pt_coulds[i])
	if return_xy_minmax:
		print("finished get pt coulds (include xy_minmax).")
		return pt_coulds, (x_min, x_max, y_min, y_max)
	print("finished get pt coulds.")
	return pt_coulds


if __name__ == "__main__":

	print("hello, world!")

	# 读取pcd文件
	# pcb_file_path = "/home/bingqiangzhou/r3live_output/rgb_pt.pcd"
	# points, colors = read_pcd(pcb_file_path)
	
	ply_file_path = "/home/bingqiangzhou/r3live_output/textured_mesh.ply"
	points, xy_minmax = read_ply(ply_file_path)
	print(xy_minmax)
	# print("Create a pyvista point cloud object")
	# cloud = pv.PolyData(points[:10000])

	# print("Generate the mesh")
	# mesh = cloud.delaunay_2d()
	# # mesh = cloud.delaunay_3d()


	# print("Plot the mesh")
	# plotter = pv.Plotter()
	# plotter.add_mesh(mesh, color='white')
	# plotter.show()

	# # Save the mesh to a file
	# mesh.save('mesh.vtk')

	# # Visualize the mesh with matplotlib
	# pv.plot(mesh)
	# plt.show()

