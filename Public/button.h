#ifndef BUTTON_H
#define BUTTON_H

#include <QPushButton>
#include <QPaintEvent>
#include <QMouseEvent>

class Button : public QPushButton
{
    Q_OBJECT
public:
    enum {Button_Min, Button_Max, Button_Close, Button_Menu, Button_Satrt, Button_Pause, Button_Volume, Button_Backward, Button_None};
    explicit Button(QWidget *parent = nullptr);
    explicit Button(int id, QWidget *parent = nullptr);

    void setID(int id);

protected:
    void paintEvent(QPaintEvent *e);
    void enterEvent(QEvent* e);
    void leaveEvent(QEvent* e);

private:
    void paintMin();
    void paintMax();
    void paintClose();
    void paintMenu();
    void paintStart();
    void paintPause();
    void paintVolume();
    void paintBackward();

    // 什么都不显示
    void paintNone();

private:
    uint8_t mID;
    bool mMouseHovered = false;
};

#endif // BUTTON_H
