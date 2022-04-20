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
    void slot_triggre_action_pause();
    void slot_triggre_action_play();
    void slot_triggre_action_seek(int64_t pts);
    void slot_triggre_action_change_volume(float volume);
    void slot_trigger_action_previous_frame();
    void slot_trigger_action_next_frame();

protected:
    void mousePressEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);

private slots:
    void slot_button_close_clicked();

    void slot_open_file();

    void slot_custom_context_menu_requested(const QPoint &pos);

private:
    Ui::MainWindow *ui;

    QPoint mLastMousePosition;
    bool mMousePressed = false;
};
#endif // MAINWINDOW_H
