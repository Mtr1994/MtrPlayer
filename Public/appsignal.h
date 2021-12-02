#ifndef APPSIGNAL_H
#define APPSIGNAL_H

#include <QObject>

class AppSignal : public QObject
{
    Q_OBJECT
private:
    explicit AppSignal(QObject *parent = nullptr);
    AppSignal(const AppSignal& signal) = delete;
    AppSignal& operator=(const AppSignal& signal) = delete;

public:
    static AppSignal* getInstance();

signals:
    void sgl_media_duration(int64_t duration, double timebase);
    void sgl_media_pause();
    void sgl_media_start();
    void sgl_media_seek(int64_t pts);
    void sgl_change_volume(float vloume);
    void sgl_stop_media();
};

#endif // APPSIGNAL_H
