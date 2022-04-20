#ifndef BUTTONDESIGN_H
#define BUTTONDESIGN_H

#include <QPushButton>
#include <QPaintEvent>
#include <QMouseEvent>

class ButtonDesigned : public QPushButton
{
    Q_OBJECT
public:
    enum {Button_Min, Button_Normal, Button_Max, Button_Close, Button_Menu, Button_Satrt, Button_Pause, Button_Volume, Button_Backward, Button_None};
    explicit ButtonDesigned(QWidget *parent = nullptr);
    explicit ButtonDesigned(int id, QWidget *parent = nullptr);

    void setID(int id);

protected:
    void paintEvent(QPaintEvent *e) override;
    void enterEvent(QEvent * e) override;
    void leaveEvent(QEvent* e) override;

private:
    void paintMin();
    void paintNormal();
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

#endif // BUTTONDESIGN_H
