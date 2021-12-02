#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPoint>
#include <QMouseEvent>
#include <QResizeEvent>
#include <QShowEvent>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void init();

public slots:
    void slot_media_pause();
    void slot_media_start();
    void slot_media_seek(int64_t pts);
    void slot_change_volume(float volume);
    void slot_stop_media();

protected:
    void mousePressEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);

    void resizeEvent(QResizeEvent *event);
    void showEvent(QShowEvent *event);

private slots:
    void slot_btnClose_clicked();
    void slot_exchange_palyer();
    void slot_open_file();

private:
    void locatePlayer();

private:
    Ui::MainWindow *ui;

    QPoint mLastMousePosition;
    bool mMousePressed = false;
};
#endif // MAINWINDOW_H
