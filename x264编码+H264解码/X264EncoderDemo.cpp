//
// Created by liheng on 19-12-9.
//

#include "x264_encoder.h"
#include "h264decoder.h"
#include <iostream>
#include <opencv2/opencv.hpp>

int main()
{
    cv::Mat frame;
    

    cv::VideoCapture videoCapture("/home/liheng/ADAS_Video/1120/ADAS_Video-20191120-151849.mp4");
    for (int i = 0; i < 10; ++i)
    {
        videoCapture >> frame;
    }

    //x264 encode
    x264Encoder m_x264Encoder;
    m_x264Encoder.Create(1280,720,3,30);
    cv::Mat x264_dst;

    //h264 decode
    CH264Decoder m_h264Decoder;
    m_h264Decoder.initial();

    //cv encode
    int jpeg_quality = 75;
    std::vector<int> params;
    params.push_back(cv::IMWRITE_JPEG_QUALITY);
    params.push_back(jpeg_quality);
    cv::Mat cv_dst;
    
    int nWaitTime =1;
    while (1)
    {
        videoCapture >>frame;
        if( frame.empty() )
            break;

        cv::imshow("src",frame);
        cv::Mat _frame;
        cv::resize(frame,_frame,cv::Size(),0.5,0.5);

        //cv ecode + cv decode
        double timePoint1 = cv::getTickCount();

        std::vector<uchar> jpgSize;
        cv::imencode(".jpg", _frame, jpgSize, params);
        cv_dst = cv::imdecode(jpgSize,cv::IMREAD_COLOR);

        cv::imshow("cv_dst",cv_dst);

        //x264 ecode + h264 decode
        double timePoint2 = cv::getTickCount();

        int size = m_x264Encoder.EncodeOneFrame(frame);
        uchar* data = nullptr;
        data = m_x264Encoder.GetEncodedFrame();

        m_h264Decoder.decode(data,size,x264_dst);
        cv::imshow("x264_dst",x264_dst);
        double timePoint3 = cv::getTickCount();
        //cv::Mat diff = x264_dst - frame;
        //cv::imshow("diff",diff);//查看编解码前后图像是否有差异

        printf("cv::encode size:%d Fps:%.2f, x264 encode size:%d,Fps:%.2f\n",
                jpgSize.size(),cv::getTickFrequency()/(timePoint2-timePoint1),
                size,cv::getTickFrequency()/(timePoint3-timePoint2));





        char chKey = cv::waitKey(nWaitTime);
        //ESC
        if (27 == chKey)break;
        else if (' ' == chKey) nWaitTime = !nWaitTime;
    }


    return 0;
}
