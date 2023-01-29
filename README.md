cameracap.cpp 捕获摄像头视频流并展示（也可以稍作修改，变成打开指定mp4等视频流文件）

res子目录
存放测试过程使用的图片

makenoise子目录 
载入图像，并指定图像的噪声（标准偏差值），制作出带有噪声的图像
res/chess.jpg 源图像
res/noise.jpg 是对源图像的噪声值设置为50时，产生的噪声图像

measurenoise子目录
衡量噪声图像的值，估计噪声
```javascript
res/N20B.PGM 输入带噪声的图片
预期结果：
Image mean is   31.63629 Standard deviation is   19.52933
Noise mean is    0.00001 Standard deviation is   19.52933
res/N20W.PGM 输入带噪声的图片
预期结果：
Image mean is  188.60692 Standard deviation is   19.46295
Noise mean is   -0.00000 Standard deviation is   19.47054
```

gradient1子目录
通过导数算子方案一来进行边缘检测
gradient2子目录
通过导数算子方案二来进行边缘检测
```javascript
导数算子的两种方案，都是用以下图片进行输入测试
测试图像res/CHESS_3.PGM
测试图像res/CHESS_18.PGM
测试图像res/ET1_3.PGM
测试图像res/ET3_9.PGM
```

pratt子目录
评估边缘检测算法给出度量值
```javascript
res/ET1.EDG 对应res/ET1_3.PGM相关图像的真实边缘像素点数据
res/ET3.EDG 对应res/ET3_9.PGM相关图像的真实边缘像素点数据
```

kr子目录
评估边缘检测算法方案二