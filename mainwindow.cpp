#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "Public/appsignal.h"

#include <QScreen>
#include <QDesktopWidget>
#include <QFileDialog>

//test
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    init();

    setWindowTitle("中国科学院深海科学与工程研究所-潜器视频播放系统");

    setWindowFlags(Qt::CustomizeWindowHint);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::init()
{
    QScreen *screen = QGuiApplication::screens().at(0);
    float width = 1024;
    float height = 640;
    if (nullptr != screen)
    {
        QRect rect = screen->availableGeometry();
        width = rect.width() * 0.64 < 1024 ? 1024 : rect.width() * 0.64;
        height = rect.height() * 0.64 < 640 ? 640 : rect.height() * 0.64;
    }

    resize(width, height);

    ui->lbTitle->setText("中国科学院深海科学与工程研究所-潜器视频播放系统");

    ui->btnMin->setID(Button::Button_Min);
    ui->btnMax->setID(Button::Button_Max);

    ui->btnClose->setID(Button::Button_Close);
    connect(ui->btnClose, &QPushButton::clicked, this, &MainWindow::slot_btnClose_clicked);
    connect(ui->btnOpen, &QPushButton::clicked, this, &MainWindow::slot_open_file);
    connect(ui->btnMin, &QPushButton::clicked, this, [this]{showMinimized();});
    connect(ui->btnMax, &QPushButton::clicked, this, [this]{showMaximized();});

    // 点击交换播放组件位置
    connect(ui->widgetPlayer0, &QtAvPlayer::sgl_player_click, this, &MainWindow::slot_exchange_palyer);
    connect(ui->widgetPlayer1, &QtAvPlayer::sgl_player_click, this, &MainWindow::slot_exchange_palyer);
    connect(ui->widgetPlayer2, &QtAvPlayer::sgl_player_click, this, &MainWindow::slot_exchange_palyer);
    connect(ui->widgetPlayer3, &QtAvPlayer::sgl_player_click, this, &MainWindow::slot_exchange_palyer);
    connect(ui->widgetPlayer4, &QtAvPlayer::sgl_player_click, this, &MainWindow::slot_exchange_palyer);
    connect(ui->widgetPlayer5, &QtAvPlayer::sgl_player_click, this, &MainWindow::slot_exchange_palyer);
    connect(ui->widgetPlayer6, &QtAvPlayer::sgl_player_click, this, &MainWindow::slot_exchange_palyer);

    // 启动隐藏 播放组件，显示开机组件
    ui->widgetContent->hide();

    // 视频信息监听
    connect(AppSignal::getInstance(), &AppSignal::sgl_media_duration, ui->widgetMediaControl, &WidgetMediaControl::slot_media_duration);
    // 假定时间差不多长度，使用第一个 4K 高清视频的时间轴为基础
    connect(ui->widgetPlayer0, &QtAvPlayer::sgl_media_process, ui->widgetMediaControl, &WidgetMediaControl::slot_media_process);

    connect(AppSignal::getInstance(), &AppSignal::sgl_media_pause, this, &MainWindow::slot_media_pause);
    connect(AppSignal::getInstance(), &AppSignal::sgl_media_start, this, &MainWindow::slot_media_start);
    connect(AppSignal::getInstance(), &AppSignal::sgl_media_seek, this, &MainWindow::slot_media_seek);
    connect(AppSignal::getInstance(), &AppSignal::sgl_change_volume, this, &MainWindow::slot_change_volume);
    connect(AppSignal::getInstance(), &AppSignal::sgl_stop_media, this, &MainWindow::slot_stop_media);
}

void MainWindow::slot_media_pause()
{
    ui->widgetPlayer0->pause();
    ui->widgetPlayer1->pause();
    ui->widgetPlayer2->pause();
    ui->widgetPlayer3->pause();
    ui->widgetPlayer4->pause();
    ui->widgetPlayer5->pause();
    ui->widgetPlayer6->pause();
}

void MainWindow::slot_media_start()
{
    ui->widgetPlayer0->start();
    ui->widgetPlayer1->start();
    ui->widgetPlayer2->start();
    ui->widgetPlayer3->start();
    ui->widgetPlayer4->start();
    ui->widgetPlayer5->start();
    ui->widgetPlayer6->start();
}

void MainWindow::slot_media_seek(int64_t pts)
{
    ui->widgetPlayer0->seek(pts);
    ui->widgetPlayer1->seek(pts);
    ui->widgetPlayer2->seek(pts);
    ui->widgetPlayer3->seek(pts);
    ui->widgetPlayer4->seek(pts);
    ui->widgetPlayer5->seek(pts);
    ui->widgetPlayer6->seek(pts);
}

void MainWindow::slot_change_volume(float volume)
{
    volume = volume / 100;
    QtAvPlayer * widget1 = (QtAvPlayer*)ui->widgetContent->childAt(9, 0)->children().at(1);
    if (nullptr == widget1) return;
    widget1->setVolume(volume);
}

void MainWindow::slot_stop_media()
{
    ui->widgetPlayer0->stop();
    ui->widgetPlayer1->stop();
    ui->widgetPlayer2->stop();
    ui->widgetPlayer3->stop();
    ui->widgetPlayer4->stop();
    ui->widgetPlayer5->stop();
    ui->widgetPlayer6->stop();

    ui->widgetContent->hide();
    ui->widgetStart->show();
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->pos().x() > ui->widgetTitle->width() || event->pos().y() > ui->widgetTitle->height()) return;
    if (event->button() == Qt::LeftButton)
    {
        mLastMousePosition = event->globalPos();
        mMousePressed = true;
    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    mMousePressed = false;
    event->accept();
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (!mMousePressed) return;
    if (!event->buttons().testFlag(Qt::LeftButton)) return;
    const QPoint position = pos() + event->globalPos() - mLastMousePosition; //the position of mainfrmae + (current_mouse_position - last_mouse_position)
    move(position.x(), position.y());
    mLastMousePosition = event->globalPos();
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    if (nullptr != event) event->ignore();

    float w = ui->widgetContent->width();
    float h = ui->widgetContent->height();

    int card0 = ceil(w * 0.6);
    int card1 = w - card0;

    if (nullptr == event)
    {
        ui->widgetContainer1->setGeometry(9, 0, card0 - 9, h - 9);
    }
    else
    {
        QWidget* widget0 = ui->widgetContent->childAt(9, 0);
        if (nullptr == widget0) return;
        widget0->setGeometry(9, 0, card0 - 9, h - 9);
    }
    ui->widgetPlayerList->setGeometry(card0, 0, card1 + 3, h);
    ui->widgetMediaControl->setGeometry(9, ui->widgetContent->height() - 200, card0 - 9, 180);
}

void MainWindow::showEvent(QShowEvent *event)
{
    resizeEvent(nullptr);
    event->accept();
}

void MainWindow::slot_btnClose_clicked()
{
    exit(0);
}

void MainWindow::slot_exchange_palyer()
{
    QWidget * widget1 = ui->widgetContent->childAt(9, 0);
    if (nullptr == widget1) return;
    auto flags = widget1->windowFlags();
    QRect rect1 = widget1->property("geometry").toRect();

    QtAvPlayer *player2 = dynamic_cast<QtAvPlayer*>(sender());
    QWidget * widget2 = (QWidget*)player2->parent();
    ui->gridLayoutPlayerList->replaceWidget(widget2, widget1);

    if (widget1 == widget2) return;

    QtAvPlayer *player1 = dynamic_cast<QtAvPlayer*>(widget1->children().at(1));
    player1->setVolume(0);

    widget2->setParent(ui->widgetContent, flags);
    widget2->setGeometry(rect1);
    float volume = ui->widgetMediaControl->getVolume();
    player2->setVolume(volume);
    widget2->show();

    ui->widgetMediaControl->raise();
}

void MainWindow::slot_open_file()
{
    QStringList fileName = QFileDialog::getOpenFileNames(nullptr, tr("选择视频文件"), "", tr("视频文件 (*.mp4 *.mkv *.avi *.mov *.flv *.wmv *.mpg)"));
    if (fileName.isEmpty()) return;

    for (int i = 0; i < fileName.size(); i++)
    {
        if (i == 0) ui->widgetPlayer0->play(fileName.at(i).toStdString());
        if (i == 1) ui->widgetPlayer1->play(fileName.at(i).toStdString());
        if (i == 2) ui->widgetPlayer2->play(fileName.at(i).toStdString());
        if (i == 3) ui->widgetPlayer3->play(fileName.at(i).toStdString());
        if (i == 4) ui->widgetPlayer4->play(fileName.at(i).toStdString());
        if (i == 5) ui->widgetPlayer5->play(fileName.at(i).toStdString());
        if (i == 6) ui->widgetPlayer6->play(fileName.at(i).toStdString());
    }

    for (int i = fileName.size(); i < 6; i++)
    {
        if (i == 0) ui->widgetPlayer0->stop();
        if (i == 1) ui->widgetPlayer1->stop();
        if (i == 2) ui->widgetPlayer2->stop();
        if (i == 3) ui->widgetPlayer3->stop();
        if (i == 4) ui->widgetPlayer4->stop();
        if (i == 5) ui->widgetPlayer5->stop();
        if (i == 6) ui->widgetPlayer6->stop();
    }

    if (fileName.size() == 0) return;

    // 防止打开之前，打开
    locatePlayer();

    ui->widgetStart->hide();
    ui->widgetContent->show();

    ui->widgetPlayer0->setVolume(float(0.32));

    resizeEvent(nullptr);
}

void MainWindow::locatePlayer()
{
    if(ui->widgetContainer1->parentWidget()->objectName() == "widgetContent") return;

    // 换位置 （每一个都要重新定位）（根据打开文件的名称，确定 Player, 并重新排序，优先 4K 高清在左侧）
}

