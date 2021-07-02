#ifndef VERSION1_0_TRANSDATA_H
#define VERSION1_0_TRANSDATA_H

#include <iostream>
extern "C"
{
#include "libavformat/avformat.h"
#include <libavutil/mathematics.h>
#include <libavutil/time.h>
#include <libavutil/samplefmt.h>
#include <libavcodec/avcodec.h>
#include <libavfilter/buffersink.h>
#include <libavfilter/buffersrc.h>
#include "libavutil/avconfig.h"
#include <libavutil/imgutils.h>
#include "libswscale/swscale.h"
};
#include "opencv2/core.hpp"
#include<opencv2/opencv.hpp>
//#include "LogUtils.h"
using namespace std;
using namespace cv;

class Transdata
{
public:
    Transdata();
    ~Transdata();
    AVFormatContext *ifmt_ctx = NULL;
    AVPacket pkt;
    AVFrame *pframe = NULL;
    int ret, i;
    int videoindex=-1;
    AVCodecContext  *pCodecCtx;
    AVCodec         *pCodec;
    const AVBitStreamFilter *buffersrc  =  NULL;
    AVBSFContext *bsf_ctx;
    AVCodecParameters *codecpar = NULL;
    std::string str2= "rtmp://localhost:1935/rtmplive/test";
    //std::string str2= "rtmp://47.100.110.164:1935/live/test";
    //const char *in_filename  = "rtmp://localhost:1935/rtmplive";   //rtmp地址
    //const char *in_filename  = "rtmp://58.200.131.2:1935/livetv/hunantv";   //芒果台rtmp地址
    cv::Mat image_test;
    int Transdata_init(int num);
    int Transdata_Recdata();
    int Transdata_free();
    void avframeToCvmat(const AVFrame * frame);
    int User_ID;
    string Simg_index;
    pthread_t thread_id;
};


#endif //VERSION1_0_TRANSDATA_H
#ifndef VERSION1_0_TRANSDATA_H
#define VERSION1_0_TRANSDATA_H

#include <iostream>
extern "C"
{
#include "libavformat/avformat.h"
#include <libavutil/mathematics.h>
#include <libavutil/time.h>
#include <libavutil/samplefmt.h>
#include <libavcodec/avcodec.h>
#include <libavfilter/buffersink.h>
#include <libavfilter/buffersrc.h>
#include "libavutil/avconfig.h"
#include <libavutil/imgutils.h>
#include "libswscale/swscale.h"
};
#include "opencv2/core.hpp"
#include<opencv2/opencv.hpp>
using namespace std;
using namespace cv;

class Transdata
{
public:
    Transdata();
    ~Transdata();
    AVFormatContext *ifmt_ctx = NULL;
    AVPacket pkt;
    AVFrame *pframe = NULL;
    int ret, i;
    int videoindex=-1;
    AVCodecContext  *pCodecCtx;
    AVCodec         *pCodec;
    const AVBitStreamFilter *buffersrc  =  NULL;
    AVBSFContext *bsf_ctx;
    AVCodecParameters *codecpar = NULL;
    std::string str2= "rtmp://localhost:1935/rtmplive/test";
    int Transdata_init(int num);
    int Transdata_Recdata();
    int Transdata_free();
    void avframeToCvmat(const AVFrame * frame);
    int User_ID;
    string Simg_index;
    pthread_t thread_id;
};

#endif //VERSION1_0_TRANSDATA_H
