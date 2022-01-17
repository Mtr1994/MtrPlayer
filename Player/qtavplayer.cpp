#include "qtavplayer.h"
#include "Public/appsignal.h"

extern "C"
{
    #include <libavcodec/avcodec.h>
    #include <libavformat/avformat.h>
    #include <libavfilter/buffersink.h>
    #include <libavfilter/buffersrc.h>
    #include <libavutil/opt.h>
    #include "libswscale/swscale.h"
    #include "libswresample/swresample.h"
    #include "libavutil/opt.h"
}

#include <QAudioFormat>
#include <QAudioOutput>
#include <QIODevice>
#include <QPainter>
#include <thread>

QtAvPlayer::QtAvPlayer(QWidget *parent) : QOpenGLWidget(parent)
{
    connect(this, &QtAvPlayer::sgl_update_player, this, &QtAvPlayer::slot_update_player, Qt::QueuedConnection);
}

QtAvPlayer::~QtAvPlayer()
{
    disconnect(this, &QtAvPlayer::sgl_update_player, this, &QtAvPlayer::slot_update_player);
    clear();
}

void QtAvPlayer::play(const std::string &path)
{
    if (path.empty()) return;

    // 清理数据,回复状态
    clear();

    mPlayStatus = MediaPlaying;

    auto func1 = std::bind(&QtAvPlayer::parse, this, std::placeholders::_1);
    std::thread th1(func1, path);
    th1.detach();

    auto func2 = std::bind(&QtAvPlayer::playVideo, this);
    std::thread th2(func2);
    th2.detach();

    auto func3 = std::bind(&QtAvPlayer::playAudio, this);
    std::thread th3(func3);
    th3.detach();
}

void QtAvPlayer::pause()
{
    if (mPlayStatus != MediaPlaying) return;
    mPlayStatus = MediaPause;
    mPauseTime = getCurrentMillisecond();
}

void QtAvPlayer::start()
{
    if (mPlayStatus != MediaPause) return;
    mPlayStatus = MediaPlaying;
    mCvPause.notify_all();
}

void QtAvPlayer::seek(int32_t pts)
{
    if (mParseStop) return;

    mSeekPts = pts + (mVideoStartPts > 0 ? mVideoStartPts : 0);
    mCvPause.notify_all();
}

void QtAvPlayer::setVolume(float volume)
{
    if (volume < 0 || volume > 1) return;
    mVolume = volume;

    if (nullptr == mAudioOutput) return;
    mAudioOutput->setVolume(mVolume);
}

void QtAvPlayer::stop()
{
    // 关闭播放，并初始化参数
    clear();
}

void QtAvPlayer::initializeGL()
{
    initializeOpenGLFunctions();
    const char *vsrc =
            "attribute vec4 vertexIn; \
             attribute vec4 textureIn; \
             varying vec4 textureOut;  \
             void main(void)           \
             {                         \
                 gl_Position = vertexIn; \
                 textureOut = textureIn; \
             }";

    const char *fsrc =
            "varying mediump vec4 textureOut;\n"
            "uniform sampler2D textureY;\n"
            "uniform sampler2D textureU;\n"
            "uniform sampler2D textureV;\n"
            "void main(void)\n"
            "{\n"
            "vec3 yuv; \n"
            "vec3 rgb; \n"
            "yuv.x = texture2D(textureY, textureOut.st).r; \n"
            "yuv.y = texture2D(textureU, textureOut.st).r - 0.5; \n"
            "yuv.z = texture2D(textureV, textureOut.st).r - 0.5; \n"
            "rgb = mat3( 1,       1,         1, \n"
                        "0,       -0.39465,  2.03211, \n"
                        "1.13983, -0.58060,  0) * yuv; \n"
            "gl_FragColor = vec4(rgb, 1); \n"
            "}\n";

    m_program.addCacheableShaderFromSourceCode(QOpenGLShader::Vertex, vsrc);
    m_program.addCacheableShaderFromSourceCode(QOpenGLShader::Fragment, fsrc);
    m_program.link();

    GLfloat points[]{
        -1.0f, 1.0f,
         1.0f, 1.0f,
         1.0f, -1.0f,
        -1.0f, -1.0f,

        0.0f,0.0f,
        1.0f,0.0f,
        1.0f,1.0f,
        0.0f,1.0f
    };

    vbo.create();
    vbo.bind();
    vbo.allocate(points, sizeof(points));

    GLuint ids[3];
    glGenTextures(3, ids);
    idY = ids[0];
    idU = ids[1];
    idV = ids[2];
}

void QtAvPlayer::paintGL()
{
    std::lock_guard<std::mutex> lock(mMutexVideoPlay);
    if(!mDataY || !mDataU || !mDataV) return;

    glViewport((this->width() - mVideoWidth) / 2.0, (this->height() - mVideoHeight) / 2.0, mVideoWidth, mVideoHeight);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);

    m_program.bind();
    vbo.bind();
    m_program.enableAttributeArray("vertexIn");
    m_program.enableAttributeArray("textureIn");
    m_program.setAttributeBuffer("vertexIn", GL_FLOAT, 0, 2, 2 * sizeof(GLfloat));
    m_program.setAttributeBuffer("textureIn", GL_FLOAT, 2 * 4 * sizeof(GLfloat), 2, 2 * sizeof(GLfloat));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, idY);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, mVideoOriginWidth, mVideoOriginHeight, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, mDataY);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, idU);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, mVideoOriginWidth >> 1, mVideoOriginHeight >> 1, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, mDataU);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, idV);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, mVideoOriginWidth >> 1, mVideoOriginHeight >> 1, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, mDataV);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    m_program.setUniformValue("textureY", 0);
    m_program.setUniformValue("textureU", 1);
    m_program.setUniformValue("textureV", 2);
    glDrawArrays(GL_QUADS, 0, 4);
    m_program.disableAttributeArray("vertexIn");
    m_program.disableAttributeArray("textureIn");
    m_program.release();
}

void QtAvPlayer::resizeGL(int w, int h)
{
    if(h <= 0) h = 1;

    double rate = (double)mVideoOriginHeight / mVideoOriginWidth;
    double widthSize = w;
    double heightSize = w * rate;

    if (h < heightSize)
    {
        heightSize = h;
        widthSize = heightSize / rate;
    }

    mVideoWidth = widthSize;
    mVideoHeight = heightSize;
}

void QtAvPlayer::mouseDoubleClickEvent(QMouseEvent *event)
{
    event->accept();
    emit sgl_player_click();
}

void QtAvPlayer::slot_update_player()
{
    update();
}

void QtAvPlayer::clear()
{
    mPlayStatus = MediaNone;
    mCvPause.notify_all();

    mPlayTime = 0;
    mPauseTime = 0;
    mPauseTimeSpace = 0;
    mStartTime = 0;
    mSeekPts = 0;
    mVideoStartPts = -1;
    mAudioStartPts = -1;

    std::unique_lock<std::mutex> lock(mMutexClose);
    mCvClose.wait(lock, [this]{return mVideoPlayStop && mAudioPlayStop && mParseStop;});

    while (!mQueueVideo.empty())
    {
        VideoFrame frame;
        mQueueVideo.wait_and_pop(frame);
        delete [] frame.y;
        delete [] frame.u;
        delete [] frame.v;
        mQueueVideo.wait_and_pop();
    }

    while (!mQueueAudio.empty())
    {
        AudioFrame frame;
        mQueueAudio.wait_and_pop(frame);
        delete [] frame.data;
        mQueueAudio.wait_and_pop();
    }

    if (mDataY)
    {
        delete [] mDataY;
        mDataY = nullptr;
    }
    if (mDataU)
    {
        delete [] mDataU;
        mDataU = nullptr;
    }
    if (mDataV)
    {
        delete [] mDataV;
        mDataV = nullptr;
    }
}

void QtAvPlayer::parse(const std::string& path)
{
    //输入输出封装上下文
    AVFormatContext *formatCtx = nullptr;

    AVCodecContext* codeCtxVideo = nullptr;
    int videoStreamIndex = -1;

    AVCodecContext *codeCtxAudio = nullptr;
    int audioStreamIndex = -1;

    ///查找解码器
    const AVCodec* pCodecVideo = nullptr;
    const AVCodec* pCodecAudio = nullptr;
    int ret = 0;

    //打开文件，解封文件头
    ret = avformat_open_input(&formatCtx, path.c_str(), 0, 0);
    if (ret < 0)
    {
        printf("Could not open input file \n");
        return;
    }

    //获取音频视频流信息 ,h264 flv
    ret = avformat_find_stream_info(formatCtx, 0);
    if (ret < 0)
    {
        printf("Failed to retrieve input stream information\n");
        return;
    }

    if (formatCtx->nb_streams == 0)
    {
        printf("can not find stream\n");
        return;
    }

    for (unsigned i = 0; i < formatCtx->nb_streams; i++)
    {
        if (formatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
        {
            videoStreamIndex = i;
            AVCodecID codecid = formatCtx->streams[i]->codecpar->codec_id;
            if (codecid == AV_CODEC_ID_NONE) return;

            ///查找解码器
            pCodecVideo = avcodec_find_decoder(codecid);
            codeCtxVideo = avcodec_alloc_context3(pCodecVideo);
            avcodec_parameters_to_context(codeCtxVideo, formatCtx->streams[i]->codecpar);

            //没有此句会出现：Could not update timestamps for skipped samples
            codeCtxVideo->pkt_timebase = formatCtx->streams[i]->time_base;

            ///打开解码器
            if (avcodec_open2(codeCtxVideo, pCodecVideo, NULL) < 0)
            {
                printf("Could not open codec.");
                return;
            }

            double timebase = (double)codeCtxVideo->pkt_timebase.num / codeCtxVideo->pkt_timebase.den;
            int64_t duration = formatCtx->streams[i]->duration;
            if (duration < 0)
            {
                duration = formatCtx->duration * 1.0 / AV_TIME_BASE / timebase;
            }
            mVideoDuration = duration;
            // 发送给视频帧数量
            emit AppSignal::getInstance()->sgl_media_duration(duration, timebase);
        }
        else if (formatCtx->streams[i]->codecpar->codec_type== AVMEDIA_TYPE_AUDIO)
        {
            audioStreamIndex = i;
            AVCodecID codecid = formatCtx->streams[i]->codecpar->codec_id;
            if (codecid == AV_CODEC_ID_NONE) return;

            pCodecAudio = avcodec_find_decoder(formatCtx->streams[i]->codecpar->codec_id);
            codeCtxAudio = avcodec_alloc_context3(pCodecAudio);
            avcodec_parameters_to_context(codeCtxAudio, formatCtx->streams[i]->codecpar);

            //没有此句会出现：Could not update timestamps for skipped samples
            codeCtxAudio->pkt_timebase = formatCtx->streams[i]->time_base;

            ///打开解码器
            if (avcodec_open2(codeCtxAudio, pCodecAudio, NULL) < 0) {
                printf("Could not open codec.");
                return;
            }

            // 音频固定转换为 AV_SAMPLE_FMT_FLT 格式播放
            auto sampleSize = av_get_bytes_per_sample(AV_SAMPLE_FMT_FLT);
            initAudio(codeCtxAudio->sample_rate, codeCtxAudio->channels, sampleSize);
        }
    }

    auto sampleSize = av_get_bytes_per_sample(AV_SAMPLE_FMT_FLT);
    AVPacket *packet = av_packet_alloc(); // 分配一个packet
    AVFrame *frame = av_frame_alloc();
    int milliseconds = 0;
    mParseStop = false;
    // 记录是否获取到第一帧
    bool headFrame = false;
    while (true)
    {
        std::unique_lock<std::mutex> lockStatus(mMutexPause);
        mCvPause.wait(lockStatus, [this]{return (mPlayStatus == MediaPlaying ) || (mPlayStatus == MediaNone) || ((mSeekPts > 0) || mSeekVideo || mSeekAudio);});
        lockStatus.unlock();

        if (mPlayStatus == MediaNone) break;
        milliseconds = mSeekPts > 0 ? 0 : milliseconds;
        std::unique_lock<std::mutex> lock(mMutexParse);
        if(mCvParse.wait_for(lock, std::chrono::milliseconds(milliseconds)) == std::cv_status::timeout)
        {
            if (mSeekPts > 0)
            {
                // 重新开始播放前，清空已经读取的帧数据
                avcodec_flush_buffers(codeCtxVideo);
                avcodec_flush_buffers(codeCtxAudio);
                avformat_flush(formatCtx);

                av_seek_frame(formatCtx, videoStreamIndex, mSeekPts, AVSEEK_FLAG_BACKWARD);

                mSeekPts = 0;

                mSeekVideo = true;
                mSeekAudio = true;
            }

            ret = av_read_frame(formatCtx, packet);
            if (ret < 0) break;
            if (packet->stream_index == videoStreamIndex)
            {
                ret = avcodec_send_packet(codeCtxVideo, packet);
                if (ret < 0)
                {
                    av_packet_unref(packet);
                    continue;
                }

                SwsContext* pSwsCtx = nullptr;
                while (ret >= 0)
                {
                    ret = avcodec_receive_frame(codeCtxVideo, frame);
                    if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
                    {
                        av_frame_unref(frame);
                        break;
                    }
                    else if (ret < 0)
                    {
                        av_frame_unref(frame);
                        break;
                    }

                    frame->pts = frame->best_effort_timestamp;
                    // 记录第一帧图像的 PTS
                    if (mVideoStartPts < 0) mVideoStartPts = frame->pts;
                    if (!headFrame)
                    {
                        mPlayTime = getCurrentMillisecond();
                        headFrame = true;
                    }
                    if (nullptr == pSwsCtx) pSwsCtx = sws_getContext(codeCtxVideo->width, codeCtxVideo->height, (AVPixelFormat)frame->format, codeCtxVideo->width, codeCtxVideo->height, AV_PIX_FMT_YUV420P, SWS_BICUBIC, nullptr, nullptr, nullptr);
                    int pixWidth = codeCtxVideo->width;
                    int pixHeight = codeCtxVideo->height;
                    char *y = new char[pixWidth * pixHeight];
                    char *u = new char[pixWidth * pixHeight / 2];
                    char *v = new char[pixWidth * pixHeight / 2];
                    uint8_t *data[AV_NUM_DATA_POINTERS] = { 0 };
                    data[0] = (uint8_t *)y;
                    data[1] = (uint8_t *)u;
                    data[2] = (uint8_t *)v;
                    int lines[AV_NUM_DATA_POINTERS] = { 0 };
                    lines[0] = pixWidth;
                    lines[1] = pixWidth / 2;
                    lines[2] = pixWidth / 2;
                    int ret = sws_scale(pSwsCtx, (uint8_t const * const *) frame->data, frame->linesize, 0, frame->height, data, lines);
                    if (ret > 0)
                    {
                        VideoFrame video =
                        {
                            frame->pts - mVideoStartPts,
                            mSeekVideo,
                            pixWidth,
                            pixHeight,
                            pixWidth * pixHeight * sampleSize,
                            (double)codeCtxVideo->pkt_timebase.num / codeCtxVideo->pkt_timebase.den,
                            data[0],
                            data[1],
                            data[2]
                        };

                        if (mSeekVideo)
                        {
                            mStartTime = (frame->pts - mVideoStartPts) * video.timebase * 1000;
                            mPlayTime = getCurrentMillisecond();
                            if (mPlayStatus == MediaPlaying)
                            {
                                mPauseTime = 0;
                            }
                            else if (mPlayStatus == MediaPause)
                            {
                                mPauseTime = getCurrentMillisecond();
                            }
                            mPauseTimeSpace = 0;
                        }

                        // 根据实际数量，动态修改等待时间
                        size_t size = mQueueVideo.size();
                        milliseconds = (int)size * 100 * frame->pkt_duration * (double)codeCtxVideo->pkt_timebase.num / codeCtxVideo->pkt_timebase.den;

                        //qDebug() << "insert video " << mQueueVideo.size() << " sellp " << milliseconds;
                        mQueueVideo.push(video);

                        if (mPlayStatus == MediaPause && mSeekVideo)
                        {
                            mCvPause.notify_all();
                        }
                    }
                    av_frame_unref(frame);
                }
                av_packet_unref(packet);
                sws_freeContext(pSwsCtx);
            }
            else if (packet->stream_index == audioStreamIndex)
            {
                ret = avcodec_send_packet(codeCtxAudio, packet);
                if (ret < 0)
                {
                    av_packet_unref(packet);
                    continue;
                }
                AVFrame *frame = av_frame_alloc();
                while (ret >= 0)
                {
                    ret = avcodec_receive_frame(codeCtxAudio, frame);
                    if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
                    {
                        av_frame_unref(frame);
                        break;
                    }
                    else if (ret < 0)
                    {
                        av_frame_unref(frame);
                        break;
                    }

                    frame->pts = frame->best_effort_timestamp;
                    // 记录第一帧图像的 PTS
                    if (mAudioStartPts < 0) mAudioStartPts = frame->pts;
                    if (frame->pts == 0)
                    {
                        mPlayTime = getCurrentMillisecond();
                    }

                    auto swrCtx = swr_alloc_set_opts(nullptr, frame->channel_layout, AV_SAMPLE_FMT_FLT, codeCtxAudio->sample_rate, codeCtxAudio->channel_layout, codeCtxAudio->sample_fmt, codeCtxAudio->sample_rate, 0, nullptr);
                    swr_init(swrCtx);
                    int bufsize = av_samples_get_buffer_size(frame->linesize, frame->channels, frame->nb_samples, AV_SAMPLE_FMT_FLT, 0);
                    uint8_t *buf = new uint8_t[bufsize];
                    const int out_num_samples = av_rescale_rnd(swr_get_delay(swrCtx, frame->sample_rate) + frame->nb_samples, frame->sample_rate, frame->sample_rate, AV_ROUND_UP);
                    int tmpSize = swr_convert(swrCtx, &buf, out_num_samples, (const uint8_t**)(frame->data), frame->nb_samples);
                    if (tmpSize > 0)
                    {
                       AudioFrame audio =
                        {
                            frame->pts - mAudioStartPts,
                            mSeekAudio,
                            bufsize,
                            (double)codeCtxAudio->pkt_timebase.num / codeCtxAudio->pkt_timebase.den,
                            buf
                        };

                       mQueueAudio.push(audio);

                       if (mPlayStatus == MediaPause && mSeekAudio)
                       {
                           mCvPause.notify_all();
                       }
                    }
                    else
                    {
                        qDebug() << "audio  convert failed";
                    }

                    swr_free(&swrCtx);

                    milliseconds = 0;
                }
                av_packet_unref(packet);
            }
            else
            {
                printf("other stream\n");
            }
        }
    }

    av_frame_free(&frame);
    av_packet_unref(packet);
    av_packet_free(&packet);
    avcodec_free_context(&codeCtxVideo);
    avcodec_free_context(&codeCtxAudio);
    mPlayStatus = MediaClose;

    mParseStop = true;
    mCvClose.notify_one();
    qDebug() << "video parse over ~";
}

void QtAvPlayer::playVideo()
{
    mVideoPlayStop = false;
    while (true)
    {
        std::unique_lock<std::mutex> lockStatus(mMutexPause);
        mCvPause.wait(lockStatus, [this]{return mPlayStatus != MediaPause || mSeekVideo;});
        lockStatus.unlock();

        if ((mPlayStatus == MediaNone) || ((mPlayStatus == MediaClose) && mQueueVideo.empty())) break;

        VideoFrame frame;
        mQueueVideo.wait_and_pop(frame);

        int64_t currentTime = getCurrentMillisecond();
        int time = frame.pts * frame.timebase * 1000 - mStartTime;
        if (mPauseTime != 0 && mPlayStatus != MediaPause)
        {
            mPauseTimeSpace += currentTime - mPauseTime;
            mPauseTime = 0;
        }
        int space = currentTime - mPlayTime - mPauseTimeSpace;
        bool flag1 = space < time;
        bool flag2 = mSeekVideo == frame.seek;

        if (flag1 && flag2) continue;
        mQueueVideo.wait_and_pop();
        if (frame.seek) mSeekVideo = false;

        double rate = (double)frame.height / frame.width;
        double widthSize = this->width();
        double heightSize = this->width() * rate;

        if (this->height() < heightSize)
        {
            heightSize = this->height();
            widthSize = heightSize / rate;
        }

        std::unique_lock<std::mutex> lock(mMutexVideoPlay);
        if (nullptr != mDataY)
        {
            delete [] mDataY;
            mDataY = nullptr;
        }

        if (nullptr != mDataU)
        {
            delete [] mDataU;
            mDataU = nullptr;
        }

        if (nullptr != mDataV)
        {
            delete [] mDataV;
            mDataV = nullptr;
        }
        lock.unlock();

        mDataY = (uchar*)frame.y;
        mDataU = (uchar*)frame.u;
        mDataV = (uchar*)frame.v;

        mVideoWidth = widthSize;
        mVideoHeight = heightSize;
        mVideoOriginWidth = frame.width;
        mVideoOriginHeight = frame.height;

        // 只能调用 update， 使用 repaint 会导致 UI 压力增大，从而卡顿
        emit sgl_update_player();

        if (!frame.seek && !mSeekVideo) emit sgl_media_process(frame.pts);
    }
    mVideoPlayStop = true;
    mCvClose.notify_one();

    emit sgl_media_process(mVideoDuration);
    qDebug() << "play video over ";
}

void QtAvPlayer::playAudio()
{
    mAudioPlayStop = false;
    while (true)
    {
        std::unique_lock<std::mutex> lock(mMutexPause);
        mCvPause.wait(lock, [this]{return mPlayStatus != MediaPause || mSeekAudio;});
        lock.unlock();

        if ((mPlayStatus == MediaNone) || ((mPlayStatus == MediaClose) && mQueueAudio.empty())) break;

        AudioFrame frame;
        mQueueAudio.wait_and_pop(frame);

        int64_t currentTime = getCurrentMillisecond();
        int time = frame.pts * frame.timebase * 1000 - mStartTime;
        if (mPauseTime != 0 && mPlayStatus != MediaPause)
        {
            mPauseTimeSpace += currentTime - mPauseTime;
            mPauseTime = 0;
        }
        int space = currentTime - mPlayTime - mPauseTimeSpace;
        bool flag1 = space < time;
        bool flag2 = mSeekAudio == frame.seek;
        if (flag1 && flag2) continue;
        mQueueAudio.wait_and_pop();
        if (frame.seek) mSeekAudio = false;

        if (mPlayStatus == MediaPause)
        {
            delete [] frame.data;
            continue;
        }

       // qDebug() << "play audio " << mQueueAudio.size();

        if (nullptr == frame.data) return;
        if (nullptr != mIODevice && mVolume > 0)
        {
           mIODevice->write((const char*)frame.data, frame.size);
        }
        delete [] frame.data;
    }

    if (nullptr != mAudioOutput)
    {
        mAudioOutput->stop();
        delete mAudioOutput;
        mAudioOutput = nullptr;
    }
    mIODevice = nullptr;

    mAudioPlayStop = true;
    mCvClose.notify_one();

    qDebug() << "play audio over ";
}

void QtAvPlayer::initAudio(int rate, int channels, int samplesize)
{
    QAudioFormat audioFormat;
    audioFormat.setSampleRate(rate);
    audioFormat.setChannelCount(channels);
    audioFormat.setSampleSize(8 * samplesize);
    audioFormat.setSampleType(QAudioFormat::Float);
    audioFormat.setCodec("audio/pcm");

    if (nullptr != mAudioOutput)
    {
       mVolume = mAudioOutput->volume();
       mAudioOutput->stop();
       delete mAudioOutput;
       mAudioOutput = nullptr;
    }

    mAudioOutput = new QAudioOutput(audioFormat);
    mAudioOutput->setVolume(mVolume);
    mAudioOutput->setBufferSize(rate * channels * samplesize);
    mIODevice = mAudioOutput->start();
}

uint64_t QtAvPlayer::getCurrentMillisecond()
{
    return (double)std::chrono::system_clock::now().time_since_epoch().count() / std::chrono::system_clock::period::den * std::chrono::system_clock::period::num * 1000;
}
