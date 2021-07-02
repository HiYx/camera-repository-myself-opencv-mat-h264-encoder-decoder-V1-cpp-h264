# camera-repository-myself-opencv-mat-h264-encoder-decoder-V1-cpp-h264
camera-repository-myself-opencv-mat-h264-encoder-decoder-V1-cpp-h264





客户端和程序的视频传输采用了cv::imencode和cv::imdecode函数实现编解码

最近偶然间发现可以利用H.264对视频进行编解码





针对x264编码，需要安装libx264库，安装命令为:`sudo apt install libx264-dev`,H264编解码需要安装ffmpeg库，同样采用apt命令进行安装即可。



- 将视频发送部分修改为采用x264进行编码后，能够在不缩放视频的情况下进行传输
- 不过，其实利用gstreamer-rtsp-sever也能够实现同样的效果，该方式采用rtsp视频流的形式进行视频传输，在接收端，可以直接利用cv::VideoCapture进行视频接收，不用编写额外的解码代码
- 也可以利用VLC media 进行视频接收，此时甚至不用专门编写视频接收代码了