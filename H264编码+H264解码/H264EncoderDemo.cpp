//
// Created by liheng on 19-12-9.
//

#include "h264encoder.h"
#include "h264decoder.h"
#include <iostream>
#include <opencv2/opencv.hpp>

int main()
{
    cv::Mat frame;
    cv::Mat dst;

    cv::VideoCapture videoCapture("/home/liheng/ADAS_Video/1120/ADAS_Video-20191120-151849.mp4");
    for (int i = 0; i < 10; ++i)
    {
        videoCapture >> frame;
    }

    h264Encoder h264;
    AvH264EncConfig conf;
    conf.width = 1280;
    conf.height = 720;
    conf.gop_size = 10;
    conf.max_b_frames = 0;
    conf.frame_rate = 30;
    h264.Init(conf);


    CH264Decoder m_h264Decoder;
    m_h264Decoder.initial();

    int jpeg_quality = 75;
    std::vector<int> params;
    params.push_back(cv::IMWRITE_JPEG_QUALITY);
    params.push_back(jpeg_quality);

    cv::Mat cvDst;
    int nWaitTime =1;
    while (1)
    {
        videoCapture >>frame;
        if( frame.empty() )
            break;

        cv::imshow("src",frame);
        cv::Mat _frame;
        cv::resize(frame,_frame,cv::Size(),0.5,0.5);

        double timePoint1 = cv::getTickCount();

        std::vector<uchar> jpgSize;
        cv::imencode(".jpg", _frame, jpgSize, params);

        double timePoint2 = cv::getTickCount();

        cvDst = cv::imdecode(jpgSize,cv::IMREAD_COLOR);
        cv::imshow("cvdecode",cvDst);


        // do encode
        AVPacket *pkt = h264.encode(frame);
        int size = pkt->size;
        uchar* data = nullptr;
        data = pkt->data;

        m_h264Decoder.decode(data,size,dst);
        cv::imshow("decode",dst);

        double timePoint3 = cv::getTickCount();


        //cv::Mat diff = dst - frame;
        //cv::imshow("diff",diff);//查看编解码前后图像是否有差异



        printf("cv::encode size:%d Fps:%.2f, h264 encode size:%d,Fps:%.2f\n",
                jpgSize.size(),cv::getTickFrequency()/(timePoint2-timePoint1),
                size,cv::getTickFrequency()/(timePoint3-timePoint2));





        char chKey = cv::waitKey(nWaitTime);
        //ESC
        if (27 == chKey)break;
        else if (' ' == chKey) nWaitTime = !nWaitTime;
    }


    return 0;
}
