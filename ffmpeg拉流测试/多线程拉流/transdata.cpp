#include "transdata.h"

Transdata::Transdata(){}
Transdata::~Transdata(){}


int Transdata::Transdata_free()
{
    av_bsf_free(&bsf_ctx);
    avformat_close_input(&ifmt_ctx);
    av_frame_free(&pframe);
    if (ret < 0 && ret != AVERROR_EOF)
    {
        printf( "Error occurred.\n");
        return -1;
    }
    return 0;
}


int Transdata::Transdata_Recdata()
{
	//可以自己增加LOG函数
    //LOGD("Transdata_Recdata entry %d",User_ID);
    int count = 0;
    while(av_read_frame(ifmt_ctx, &pkt)>=0)
    {
        //LOGD("av_read_frame test %d",User_ID);
        if (pkt.stream_index == videoindex) {
            // H.264 Filter
            if (av_bsf_send_packet(bsf_ctx, &pkt) < 0){
                cout << " bsg_send_packet is error! " << endl;
                av_packet_unref(&pkt);
                continue;
                //return -1;
            }
           // LOGD("av_bsf_send_packet test %d",User_ID);
            if (av_bsf_receive_packet(bsf_ctx, &pkt) < 0) {
                cout << " bsg_receive_packet is error! " << endl;
                av_packet_unref(&pkt);
                continue;
                //return -1;
            }
           // LOGD("av_bsf_receive_packet test %d",User_ID);
            count ++;
            if(count == 10) {
                printf("My id is %d,Write Video Packet. size:%d\tpts:%ld\n",User_ID, pkt.size, pkt.pts);
                count =0;
            }

            // Decode AVPacket
           // LOGD("Decode AVPacket ,ID is  %d",User_ID);
            if (pkt.size) {
                ret = avcodec_send_packet(pCodecCtx, &pkt);
                if (ret < 0 || ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
                    std::cout << "avcodec_send_packet: " << ret << std::endl;
                    av_packet_unref(&pkt);

                    continue;
                    //return -1;
                }
            //    LOGD("avcodec_send_packet test %d",User_ID);
                //Get AVframe
                ret = avcodec_receive_frame(pCodecCtx, pframe);
                if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
                    std::cout << "avcodec_receive_frame: " << ret << std::endl;
                    av_packet_unref(&pkt);
                    av_frame_unref(pframe);
                    continue;
                    // return -1;
                }
                //转成rgb
                avframeToCvmat(pframe);
            }
        }
        //Free AvPacket
        av_packet_unref(&pkt);
     //   av_free(pframe->data[0]);
        av_frame_unref(pframe); //后来才增加的 !! 每次重用之前应调用将frame复位到原始干净可用状态
        //https://www.cnblogs.com/leisure_chn/p/10404502.html

    }
        return 0;

}
//AVFrame 转 cv::mat
void Transdata::avframeToCvmat(const AVFrame * frame)
{
   // LOGD("avframeToCvmat  imshow1 , ID is  %d",User_ID);
    int width = frame->width;
    int height = frame->height;
    cv::Mat image(height, width, CV_8UC3);
  //  LOGD("avframeToCvmat  imshow2 , ID is  %d",User_ID);
    int cvLinesizes[1];
    cvLinesizes[0] = image.step1();
    SwsContext* conversion = sws_getContext(width, height, (AVPixelFormat) frame->format, width, height, AVPixelFormat::AV_PIX_FMT_BGR24, SWS_FAST_BILINEAR, NULL, NULL, NULL);
    sws_scale(conversion, frame->data, frame->linesize, 0, height, &image.data, cvLinesizes);
  //  LOGD("avframeToCvmat  imshow3 , ID is  %d",User_ID);
    sws_freeContext(conversion);
  //  LOGD("avframeToCvmat  imshow4 , ID is  %d",User_ID);
    imshow(Simg_index,image);
    startWindowThread();//开启显示线程，专门用于显示
    waitKey(1);
    image.release();
  //  LOGD("avframeToCvmat  imshow5 , ID is  %d",User_ID);
}

int Transdata::Transdata_init(int num) {

    User_ID = num;  //用户ID
    Simg_index = to_string(num);
    cout << "Simg_index is : "<< Simg_index << endl;
    string str3 = to_string(num);
    cout << str3.size()<< endl;
    std::string video_name=str2+str3;
    const char *video_filename = video_name.c_str();//string转const char*

    cout << video_filename << endl;

    //新增
    ifmt_ctx = avformat_alloc_context();
    //pkt = (AVPacket *)av_malloc(sizeof(AVPacket));

    //Register
    av_register_all();
    //Network
    avformat_network_init();
    //Input
    if ((ret = avformat_open_input(&ifmt_ctx, video_filename, 0, 0)) < 0) {
        printf("Could not open input file.");
        return -1;
    }
    if ((ret = avformat_find_stream_info(ifmt_ctx, 0)) < 0) {
        printf("Failed to retrieve input stream information");
        return -1;
    }

    videoindex = -1;
    for (i = 0; i < ifmt_ctx->nb_streams; i++) {
        if (ifmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoindex = i;
            codecpar = ifmt_ctx->streams[i]->codecpar;
        }
    }

    //Find H.264 Decoder
    pCodec = avcodec_find_decoder(AV_CODEC_ID_H264);
    if (pCodec == NULL) {
        printf("Couldn't find Codec.\n");
        return -1;
    }
    pCodecCtx = avcodec_alloc_context3(pCodec);
    if (!pCodecCtx) {
        fprintf(stderr, "Could not allocate video codec context\n");
        return -1;
    }
    if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0) {
        printf("Couldn't open codec.\n");
        return -1;
    }
    pframe = av_frame_alloc();
    if (!pframe) {
        printf("Could not allocate video frame\n");
        return -1;
    }
    //find filter
    buffersrc = av_bsf_get_by_name("h264_mp4toannexb");
    //
    if(av_bsf_alloc(buffersrc, &bsf_ctx) < 0) {
        printf("av_bsf_alloc is error");
        return -1;
    }
    if(codecpar != NULL) {
        if (avcodec_parameters_copy(bsf_ctx->par_in, codecpar) < 0) {
            printf("avcodec_parameters_copy is error");
            return -1;
        }
        if (av_bsf_init(bsf_ctx) < 0) {
            printf("av_bsf_init is error");
            return -1;
        }
    }
    else {
        printf("codecpar is NULL\n");
        return -1;
    }
        return 0;
}
