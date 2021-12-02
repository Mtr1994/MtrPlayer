#ifndef WIDGETMEDIACONTROL_H
#define WIDGETMEDIACONTROL_H

#include <QWidget>

namespace Ui {
class WidgetMediaControl;
}

class WidgetMediaControl : public QWidget
{
    Q_OBJECT

public:
    explicit WidgetMediaControl(QWidget *parent = nullptr);
    ~WidgetMediaControl();

    void init();

    float getVolume();

public slots:
    void slot_media_duration(int64_t duration, double timebase);
    void slot_media_process(int64_t pts);
    void slot_media_seek(int64_t pts);

private slots:
    void on_btnPlay_clicked();

private:
    void updateTime();

private:
    Ui::WidgetMediaControl *ui;

    // seek frame flag
    bool mIsSeekFrame = false;

    uint64_t mVideoPass = 0;
    uint64_t mVideoDuration = 0;

    double mTimebase = 0;
};

#endif // WIDGETMEDIACONTROL_H
