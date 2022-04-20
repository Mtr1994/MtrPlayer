#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "Public/appsignal.h"

#include <QScreen>
#include <QDesktopWidget>
#include <QFileDialog>
#include <QMenu>

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

    ui->lbTitle->setText("木头人播放系统");

    ui->btnMin->setID(ButtonDesigned::Button_Min);
    ui->btnMax->setID(ButtonDesigned::Button_Max);
    ui->btnNormal->setID(ButtonDesigned::Button_Normal);
    ui->btnClose->setID(ButtonDesigned::Button_Close);

    ui->btnNormal->setVisible(false);

    connect(ui->btnClose, &QPushButton::clicked, this, &MainWindow::slot_button_close_clicked);
    connect(ui->btnMin, &QPushButton::clicked, this, [this]{showMinimized();});
    connect(ui->btnMax, &QPushButton::clicked, this, [this]{ui->btnMax->setVisible(false); ui->btnNormal->setVisible(true); showMaximized();});
    connect(ui->btnNormal, &QPushButton::clicked, this, [this]{ui->btnMax->setVisible(true); ui->btnNormal->setVisible(false); showNormal();});

    // 视频信息监听
    connect(AppSignal::getInstance(), &AppSignal::sgl_media_duration, ui->widgetMediaControl, &WidgetMediaControl::slot_media_duration);
    // 假定时间差不多长度，使用第一个 4K 高清视频的时间轴为基础
    connect(ui->widgetPlayer, &QtAvPlayer::sgl_media_process_change, ui->widgetMediaControl, &WidgetMediaControl::slot_media_process_change);

    // 播放控制
    connect(ui->widgetMediaControl, &WidgetMediaControl::sgl_triggre_action_pause, this, &MainWindow::slot_triggre_action_pause);
    connect(ui->widgetMediaControl, &WidgetMediaControl::sgl_triggre_action_play, this, &MainWindow::slot_triggre_action_play);
    connect(ui->widgetMediaControl, &WidgetMediaControl::sgl_triggre_action_seek, this, &MainWindow::slot_triggre_action_seek);
    connect(ui->widgetMediaControl, &WidgetMediaControl::sgl_triggre_action_change_volume, this, &MainWindow::slot_triggre_action_change_volume);
    connect(ui->widgetMediaControl, &WidgetMediaControl::sgl_trigger_action_previous_frame, this, &MainWindow::slot_trigger_action_previous_frame);
    connect(ui->widgetMediaControl, &WidgetMediaControl::sgl_trigger_action_next_frame, this, &MainWindow::slot_trigger_action_next_frame);

    // 自定义菜单
    connect(ui->widgetContent, &QWidget::customContextMenuRequested, this, &MainWindow::slot_custom_context_menu_requested);
}

void MainWindow::slot_triggre_action_pause()
{
    ui->widgetPlayer->pause();
}

void MainWindow::slot_triggre_action_play()
{
    ui->widgetPlayer->start();
}

void MainWindow::slot_triggre_action_seek(int64_t pts)
{
    ui->widgetPlayer->seek(pts);
}

void MainWindow::slot_triggre_action_change_volume(float volume)
{
    volume = volume / 100;
    ui->widgetPlayer->setVolume(volume);
}

void MainWindow::slot_trigger_action_previous_frame()
{
    ui->widgetPlayer->previous();
}

void MainWindow::slot_trigger_action_next_frame()
{
    ui->widgetPlayer->next();
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

void MainWindow::slot_button_close_clicked()
{
    exit(0);
}

void MainWindow::slot_open_file()
{
    // 暂停播放
    ui->widgetPlayer->pause();

    QString fileName = QFileDialog::getOpenFileName(nullptr, tr("选择视频文件"), "", tr("视频文件 (*.mp4 *.mkv *.avi *.mov *.flv *.wmv *.mpg)"));
    if (fileName.isEmpty()) return;

    // 先关闭
    ui->widgetPlayer->stop();

    ui->widgetPlayer->play(fileName.toStdString());
}

void MainWindow::slot_custom_context_menu_requested(const QPoint &pos)
{
    Q_UNUSED(pos);
    QMenu menu(this);
    QAction actionOpen("打开文件");
    connect(&actionOpen, &QAction::triggered, this, &MainWindow::slot_open_file);
    menu.addAction(&actionOpen);

    QAction actionClose("关闭程序");
    connect(&actionClose, &QAction::triggered, this, []{ exit(0); });
    menu.addAction(&actionClose);

    menu.exec(QCursor::pos());
}
