#ifndef QTAVPLAYER_H
#define QTAVPLAYER_H

#include <stdint.h>

typedef struct {
    int64_t pts;
    bool seek;
    int width;
    int height;
    int size;
    double timebase;
    uint8_t* y;
    uint8_t* u;
    uint8_t* v;
} VideoFrame;

typedef struct {
    int64_t pts;
    bool seek;
    int size;
    double timebase;
    uint8_t* data;
} AudioFrame;

enum {MediaNone, MediaPlaying, MediaPause, MediaClose};


#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QMouseEvent>

#include "Public/threadsafequeue.h"

class QAudioOutput;
class QIODevice;
class QtAvPlayer : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    explicit QtAvPlayer(QWidget *parent = nullptr);
    ~QtAvPlayer();

    void play(const std::string& path);
    void pause();
    void start();
    void seek(int32_t pts);
    void setVolume(float volume);
    void stop();

signals:
    void sgl_update_player();
    void sgl_player_click();
    void sgl_media_process(int64_t pts); // 播放进度（当前 pts）

protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int w, int h);
    void mouseDoubleClickEvent(QMouseEvent *event);

private slots:
    void slot_update_player();

private:
    void clear();
    void parse(const std::string& path);

    void playVideo();
    void playAudio();

    void initAudio(int rate, int channels, int samplesize);

    uint64_t getCurrentMillisecond();

private:
    std::mutex mMutexParse;
    std::condition_variable mCvParse;

    std::mutex mMutexPause;
    std::condition_variable mCvPause;

    std::mutex mMutexClose;
    std::condition_variable mCvClose;

    std::mutex mMutexVideoPlay;
    ThreadSafeQueue<VideoFrame> mQueueVideo;
    ThreadSafeQueue<AudioFrame> mQueueAudio;

    int mVideoOriginWidth;
    int mVideoOriginHeight;
    int mVideoWidth;
    int mVideoHeight;

    QAudioOutput* mAudioOutput = nullptr;
    float mVolume = 0;
    QIODevice* mIODevice = nullptr;

    uint8_t* mDataY = nullptr;
    uint8_t* mDataU = nullptr;
    uint8_t* mDataV = nullptr;

    int64_t mPlayTime = 0;  // 视频播放系统时间
    int64_t mStartTime = 0; // 视频播放起始视频时间
    int64_t mPauseTime = 0;
    int64_t mPauseTimeSpace = 0;

    bool mVideoPlayStop = true;
    bool mAudioPlayStop = true;
    bool mParseStop = true;

    int mPlayStatus = MediaNone;

    int32_t mSeekPts = 0;
    bool mSeekVideo = false;
    bool mSeekAudio = false;

    // 视频总的帧数
    int32_t mVideoDuration = 0;

    //OPENGL
    QOpenGLShaderProgram m_program;
    QOpenGLBuffer vbo;
    int idY,idU,idV;
};

#endif // QTAVPLAYER_H
