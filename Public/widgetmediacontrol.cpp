#include "widgetmediacontrol.h"
#include "ui_widgetmediacontrol.h"
#include "Public/appsignal.h"

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
    connect(ui->slider, &QSlider::sliderMoved, this, &WidgetMediaControl::slot_media_seek);
    connect(ui->sliderVolume, &QSlider::sliderMoved, AppSignal::getInstance(), &AppSignal::sgl_change_volume);

    connect(ui->btnBackward, &QPushButton::clicked, AppSignal::getInstance(), &AppSignal::sgl_stop_media);

    ui->btnPlay->setID(Button::Button_Satrt);
    ui->btnVolume->setID(Button::Button_Volume);
    ui->btnBackward->setID(Button::Button_Backward);
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

void WidgetMediaControl::slot_media_process(int64_t pts)
{
    ui->slider->setValue(pts);

    // 更新时间
    mVideoPass = pts * mTimebase;
    updateTime();
}

void WidgetMediaControl::slot_media_seek(int64_t pts)
{
    mVideoPass = pts * mTimebase;
    updateTime();
    emit AppSignal::getInstance()->sgl_media_seek(pts);
}

void WidgetMediaControl::on_btnPlay_clicked()
{
    if (ui->btnPlay->isChecked())
    {
        emit AppSignal::getInstance()->sgl_media_start();
    }
    else
    {
        emit AppSignal::getInstance()->sgl_media_pause();
    }
}

void WidgetMediaControl::updateTime()
{
    ui->lbDuration->setText(QString("%1:%2:%3").arg(mVideoPass / 3600, 2, 10, QLatin1Char('0')).arg(mVideoPass / 60, 2, 10, QLatin1Char('0')).arg(mVideoPass % 60, 2, 10, QLatin1Char('0')));
    uint64_t tmp = mVideoDuration - mVideoPass;
    ui->lbDurationLeft->setText(QString("%1:%2:%3").arg(tmp / 3600, 2, 10, QLatin1Char('0')).arg(tmp / 60, 2, 10, QLatin1Char('0')).arg(tmp % 60, 2, 10, QLatin1Char('0')));
}
