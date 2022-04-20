#include "widgetmediacontrol.h"
#include "ui_widgetmediacontrol.h"

//test
#include <QDebug>

WidgetMediaControl::WidgetMediaControl(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WidgetMediaControl)
{
    ui->setupUi(this);

    init();
}

WidgetMediaControl::~WidgetMediaControl()
{
    delete ui;
}

void WidgetMediaControl::init()
{
    connect(ui->btnPlay, &QPushButton::clicked, this, &WidgetMediaControl::slot_button_play_clicked);
    connect(ui->btnPreviousFrame, &QPushButton::clicked, this, &WidgetMediaControl::sgl_trigger_action_previous_frame);
    connect(ui->btnNextFrame, &QPushButton::clicked, this, &WidgetMediaControl::sgl_trigger_action_next_frame);

    connect(ui->slider, &QSlider::sliderMoved, this, &WidgetMediaControl::slot_duration_slider_moved);
    connect(ui->slider, &QSlider::sliderPressed, this, [this]{ mSliderPressed = true; });
    connect(ui->slider, &QSlider::sliderReleased, this, [this]{ mSliderPressed = false; });

    connect(ui->sliderVolume, &QSlider::sliderMoved, this, &WidgetMediaControl::sgl_triggre_action_change_volume);

    ui->sliderVolume->setMaximum(100);
    ui->sliderVolume->setValue(36);
}

float WidgetMediaControl::getVolume()
{
    return ui->sliderVolume->value() / 100.0;
}

void WidgetMediaControl::slot_media_duration(int64_t duration, double timebase)
{
    ui->slider->setMaximum(duration);
    mTimebase = timebase;
    mVideoDuration = floor(duration * mTimebase);

    updateTime();

    ui->btnPlay->setChecked(true);
}

void WidgetMediaControl::slot_media_process_change(int64_t pts)
{
    if (mSliderPressed) return;
    ui->slider->setValue(pts);

    // 更新时间
    mVideoPass = pts * mTimebase;
    updateTime();
}

void WidgetMediaControl::slot_media_seek(int64_t pts)
{
    mVideoPass = pts * mTimebase;
    updateTime();

    emit sgl_triggre_action_seek(pts);
}

void WidgetMediaControl::slot_button_play_clicked()
{
    if (ui->btnPlay->isChecked())
    {
        emit sgl_triggre_action_play();
    }
    else
    {
        emit sgl_triggre_action_pause();
    }
}

void WidgetMediaControl::slot_duration_slider_moved(int64_t pts)
{
    emit sgl_triggre_action_seek(pts);
}

void WidgetMediaControl::updateTime()
{
    ui->lbDuration->setText(QString("%1:%2:%3").arg(mVideoPass / 3600, 2, 10, QLatin1Char('0')).arg(mVideoPass / 60, 2, 10, QLatin1Char('0')).arg(mVideoPass % 60, 2, 10, QLatin1Char('0')));
    ui->lbDurationLeft->setText(QString("%1:%2:%3").arg(mVideoDuration / 3600, 2, 10, QLatin1Char('0')).arg(mVideoDuration / 60, 2, 10, QLatin1Char('0')).arg(mVideoDuration % 60, 2, 10, QLatin1Char('0')));
}
