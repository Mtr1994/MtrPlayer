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

signals:
    void sgl_triggre_action_pause();
    void sgl_triggre_action_play();
    void sgl_triggre_action_seek(int64_t pts);
    void sgl_triggre_action_change_volume(float volume);
    void sgl_trigger_action_previous_frame();
    void sgl_trigger_action_next_frame();

public slots:
    void slot_media_duration(int64_t duration, double timebase);
    void slot_media_process_change(int64_t pts);
    void slot_media_seek(int64_t pts);


private slots:
    void slot_button_play_clicked();

    void slot_duration_slider_moved(int64_t pts);

private:
    void updateTime();

private:
    Ui::WidgetMediaControl *ui;

    // seek frame flag
    bool mIsSeekFrame = false;

    uint64_t mVideoPass = 0;
    uint64_t mVideoDuration = 0;

    double mTimebase = 0;

    bool mSliderPressed = false;
};

#endif // WIDGETMEDIACONTROL_H
