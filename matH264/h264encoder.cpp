#include "h264encoder.h"

h264Encoder::h264Encoder() {

    cdc_ = NULL;
    cdc_ctx_ = NULL;
    avf_ = NULL;
    avp_ = NULL;
}

h264Encoder::~h264Encoder()
{
    Destory();
}

int h264Encoder::Init(AvH264EncConfig h264_config) {

    pts_ = 0;
    cdc_ = avcodec_find_encoder(AV_CODEC_ID_H264);
    if (!cdc_) {

        return -1;
    }
    cdc_ctx_ = avcodec_alloc_context3(cdc_);
    if (!cdc_ctx_) {

        return -1;
    }
    //cdc_ctx_->bit_rate = h264_config.bit_rate;//导致画面模糊
    cdc_ctx_->width = h264_config.width;
    cdc_ctx_->height = h264_config.height;
    cdc_ctx_->time_base = { 1, h264_config.frame_rate };
    cdc_ctx_->framerate = { h264_config.frame_rate, 1 };
    cdc_ctx_->gop_size = h264_config.gop_size;
    cdc_ctx_->max_b_frames = h264_config.max_b_frames;
    cdc_ctx_->pix_fmt = AV_PIX_FMT_YUV420P;
    cdc_ctx_->codec_id = AV_CODEC_ID_H264;
    cdc_ctx_->codec_type = AVMEDIA_TYPE_VIDEO;
    //cdc_ctx_->qmin = 10;
    //cdc_ctx_->qmax = 51;
    //cdc_ctx_->qcompress = 0.6;
    AVDictionary *dict = nullptr;
    //av_dict_set(&dict, "preset", "slow", 0);
    av_dict_set(&dict, "preset", "veryfast", 0);
    av_dict_set(&dict, "tune", "zerolatency", 0);
    av_dict_set(&dict, "profile", "main", 0);
    avf_ = av_frame_alloc();
    avp_ = av_packet_alloc();
    if (!avf_ || !avp_) {

        return -1;
    }
    av_init_packet(avp_);

    frame_size_ = cdc_ctx_->width * cdc_ctx_->height;
    avf_->format = cdc_ctx_->pix_fmt;
    avf_->width = cdc_ctx_->width;
    avf_->height = cdc_ctx_->height;
    // alloc memory
    int r = av_frame_get_buffer(avf_, 0);
    if (r < 0) {

        return -1;
    }
    r = av_frame_make_writable(avf_);
    if (r < 0) {

        return -1;
    }
    return avcodec_open2(cdc_ctx_, cdc_, &dict);
}

void h264Encoder::Destory() {

    if(cdc_ctx_) avcodec_free_context(&cdc_ctx_);
    if (avf_) av_frame_free(&avf_);
    if (avp_) av_packet_free(&avp_);
}

AVPacket *h264Encoder::encode(const cv::Mat& mat) {

    if (mat.empty()) return NULL;
    cv::resize(mat, mat, cv::Size(cdc_ctx_->width, cdc_ctx_->height));
    cv::Mat yuv;
    cv::cvtColor(mat, yuv, cv::COLOR_BGR2YUV_I420);
    unsigned char *pdata = yuv.data;
    // fill yuv420
    // yyy yyy yyy yyy
    // uuu
    // vvv
    avf_->data[0] = pdata;
    avf_->data[1] = pdata + frame_size_;
    avf_->data[2] = pdata + frame_size_ * 5 / 4;
    avf_->pts = pts_++;
    int r = avcodec_send_frame(cdc_ctx_, avf_);
    if (r >= 0) {

        r = avcodec_receive_packet(cdc_ctx_, avp_);
        if (r == 0) {

            avp_->stream_index = avf_->pts;
            return avp_;
        }
        if (r == AVERROR(EAGAIN) || r == AVERROR_EOF) {

            return NULL;
        }
    }
    return NULL;
}
