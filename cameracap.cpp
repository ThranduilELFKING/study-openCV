/**
 * @file cameracap.cpp
 * @brief 捕获摄像头图像并展示视频流
 * @version 0.1
 * @date 2023-01-26
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include <iostream>

#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/types_c.h>

using namespace cv;
using namespace std;

int main() {
    //VideoCapture参数为0，表示打开笔记本内置摄像头；
    //VideoCapture参数是视频文件路径，则表示打开指定视频文件；
    VideoCapture capture(0);

    //没有打开摄像头的话，就返回
    if (!capture.isOpened()) {
        return 1;
    }

    Mat img1;
    while (true) {
        Mat frame;
        capture >> frame;

        //进行滤波
        blur(frame, frame, Size(3, 3));
        //可以实现图像反转，（可以控制图像成y轴反转，x轴反转，x,y都反转的效果）
        //如果不进行反转的话，可以看到展示的内容与实际的内容是成左右相反的
        flip(frame, frame, 1);

        imshow("视频", frame);
        //延迟30毫秒
        waitKey(30);

        //按ESC键退出
        if (cv::waitKey(50) == 27) {
            //保存当前屏幕上的图像到本地
            //SaveImage("LocalImage.png", frame, 0);
            break;
        }
    }

    //释放所有分配的资源，避免后续出现问题
    capture.release();
    return 0;
}