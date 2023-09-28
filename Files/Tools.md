
# 记录一些工具代码

## 一、Mp4转gif动图

在[https://github.com/kwiats/mp42gif]代码上进行了修改。代码执行命令如：`python mp4togif.py C:\Users\12990\Desktop\Video_2023-09-27_103917.mp4 10`

```PYTHON
import cv2
import shutil
from PIL import Image

import glob
import sys
import os

def convert_mp4_to_jpgs(path, step, resize_ratio):
    if not os.path.exists('output'):
        os.mkdir('output')

    video_capture = cv2.VideoCapture(path)
    still_reading, image = video_capture.read()
    frame_count = 0
    while still_reading:
        file_path = f"output/{frame_count}.jpg"
        if(frame_count % step == 0):
            img = cv2.resize(image, dsize=(0, 0), fx=resize_ratio, fy=resize_ratio)
            cv2.imwrite(file_path, img)

        still_reading, image = video_capture.read()
        frame_count += 1
    print("Convert from mp4 to jpgs ended successfully!")

def sort_key(e):
    file_name = os.path.split(e)[1]
    file_id = os.path.splitext(file_name)[0]
    return int(file_id)

def convert_jpgs_to_gif(output_folder, step):
    images = glob.glob(f"{output_folder}/*.jpg")
    images.sort(key=sort_key)
    # print(images[0:-1:step])
    frames = [Image.open(image) for image in images[0:-1:step]]
    frames_one = frames[0]
    frames_one.save('converted_file.gif', format='GIF', append_images=frames,
                    save_all=True, duration=50, loop=0)
    print("Convert from jpgs to gif ended successfully!")


if __name__ == "__main__":
    # print(sys.argv)
    your_file = sys.argv[1]
    step = int(sys.argv[2])
    convert_mp4_to_jpgs(your_file, step, 0.5)
    convert_jpgs_to_gif('output', 1)

    shutil.rmtree('output') # 清理中间图像文件
    print("removed temp jpg files")
```