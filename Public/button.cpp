#include "button.h"

#include <QPainter>
#include <QPainterPath>

//test
#include <QDebug>

Button::Button(QWidget *parent) : QPushButton(parent)
{
    setMouseTracking(true);
}

Button::Button(int id, QWidget *parent) : QPushButton(parent), mID(id)
{
    setMouseTracking(true);
}

void Button::setID(int id)
{
    if (id < 0 || id > Button_None) return;
    mID = id;
}

void Button::paintEvent(QPaintEvent *e)
{
    switch (mID) {
    case Button_Min:
        paintMin();
        break;
    case Button_Max:
        paintMax();
        break;
    case Button_Close:
        paintClose();
        break;
    case Button_Menu:
        paintMenu();
        break;
    case Button_Satrt:
        paintStart();
        break;
    case Button_Pause:
        paintPause();
        break;
    case Button_Volume:
        paintVolume();
        break;
    case Button_Backward:
        paintBackward();
        break;
    default:
        paintNone();
        break;
    }

    e->accept();
}

void Button::enterEvent(QEvent *e)
{
    e->accept();
    mMouseHovered = true;
    update();
}

void Button::leaveEvent(QEvent *e)
{
    e->accept();
    mMouseHovered = false;
    update();
}

void Button::paintMin()
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    QPen pen(QBrush(QColor(102, 102, 102)), 1.5);
    pen.setColor(QColor(102, 102, 102));
    pen.setCapStyle(Qt::RoundCap);
    pen.setJoinStyle(Qt::RoundJoin);

    float width = this->width();
    float height = this->height();

    if (mMouseHovered)
    {
        pen.setColor(QColor(229, 125, 40));
        pen.setBrush(QBrush(QColor(229, 125, 40)));
    }

    painter.setPen(pen);

    QPointF point1(width * 0.05, height * 0.5);
    QPointF point2(width * 0.95, height * 0.5);

    QPainterPath path;
    path.moveTo(point1);
    path.lineTo(point2);

    painter.drawPath(path);
}

void Button::paintMax()
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    QPen pen(QBrush(QColor(102, 102, 102)), 1.5);
    pen.setColor(QColor(102, 102, 102));
    pen.setCapStyle(Qt::RoundCap);
    pen.setJoinStyle(Qt::RoundJoin);

    float width = this->width();
    float height = this->height();

    if (mMouseHovered)
    {
        pen.setColor(QColor(229, 125, 40));
        pen.setBrush(QBrush(QColor(229, 125, 40)));
    }
    painter.setPen(pen);

    QPointF point1(width * 0.1, height * 0.1);
    QPointF point2(width * 0.9, height * 0.1);
    QPointF point3(width * 0.9, height * 0.9);
    QPointF point4(width * 0.1, height * 0.9);

    QPainterPath path;
    path.moveTo(point1);
    path.lineTo(point2);
    path.lineTo(point3);
    path.lineTo(point4);
    path.lineTo(point1);

    painter.drawPath(path);
}

void Button::paintClose()
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    QPen pen(QBrush(QColor(102, 102, 102)), 1.5);
    pen.setColor(QColor(102, 102, 102));
    pen.setCapStyle(Qt::RoundCap);
    pen.setJoinStyle(Qt::RoundJoin);

    float width = this->width();
    float height = this->height();

    if (mMouseHovered)
    {
        pen.setColor(QColor(229, 125, 40));
        pen.setBrush(QBrush(QColor(229, 125, 40)));
    }

    painter.setPen(pen);

    QPointF point1(width * 0.1, height * 0.1);
    QPointF point2(width * 0.9, height * 0.9);
    QPointF point3(width * 0.9, height * 0.1);
    QPointF point4(width * 0.1, height * 0.9);

    QPainterPath path;
    path.moveTo(point1);
    path.lineTo(point2);
    path.moveTo(point3);
    path.lineTo(point4);

    painter.drawPath(path);
}

void Button::paintMenu()
{

}

void Button::paintStart()
{
    if (this->isChecked())
    {
        paintPause();
        return;
    }

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    QPen pen(QBrush(QColor(254, 254, 254)), 3);
    pen.setCapStyle(Qt::RoundCap);
    pen.setJoinStyle(Qt::RoundJoin);
    painter.setPen(pen);

    float width = this->width();
    float height = this->height();

    QPointF point1(width * 0.2, height * 0.1);
    QPointF point2(width * 0.8, height * 0.5);
    QPointF point3(width * 0.2, height * 0.9);

    QPainterPath path;
    path.moveTo(point1);
    path.lineTo(point2);
    path.lineTo(point3);
    path.lineTo(point1);

    painter.drawPath(path);
    painter.fillPath(path, QBrush(QColor(254, 254, 254)));
}

void Button::paintVolume()
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    QPen pen(QBrush(QColor(254, 254, 254)), 2);
    pen.setCapStyle(Qt::RoundCap);
    pen.setJoinStyle(Qt::RoundJoin);
    painter.setPen(pen);

    float width = this->width();
    float height = this->height();

    QPointF point1(width - 12 - 12, height * 0.5 - 3);
    QPointF point2(width - 12 - 6, height * 0.5 - 5);
    QPointF point3(width - 12 - 0, height * 0.5 - 9);
    QPointF point4(width - 12  - 0, height * 0.5 + 9);
    QPointF point5(width - 12  - 6, height * 0.5 + 5);
    QPointF point6(width - 12 - 12, height * 0.5 + 3);

    QPainterPath path;
    path.moveTo(point1);
    path.lineTo(point2);
    path.lineTo(point3);
    path.lineTo(point4);
    path.lineTo(point5);
    path.lineTo(point6);
    path.lineTo(point1);

    // 画弧
    QPointF point7(width - 8, height * 0.5 - 3);
    QPointF point8(width - 8, height * 0.5 + 3);
    QPointF point9(width - 3, height * 0.5 - 9);
    QPointF pointa(width - 3, height * 0.5 + 9);

    path.moveTo(point7);
    path.lineTo(point8);
    path.moveTo(point9);
    path.lineTo(pointa);

    painter.drawPath(path);
}

void Button::paintBackward()
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    QPen pen(QBrush(QColor(254, 254, 254)), 3);
    pen.setCapStyle(Qt::RoundCap);
    pen.setJoinStyle(Qt::RoundJoin);
    painter.setPen(pen);

    float width = this->width();
    float height = this->height();

    QPointF point1(width * 0.6, height * 0.2);
    QPointF point2(width * 0.3, height * 0.5);
    QPointF point3(width * 0.6, height * 0.8);

    QPainterPath path;
    path.moveTo(point1);
    path.lineTo(point2);
    path.lineTo(point3);

    painter.drawPath(path);

    setToolTip("返回主界面");
}

void Button::paintPause()
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    QPen pen(QBrush(QColor(254, 254, 254)), 5);
    pen.setCapStyle(Qt::RoundCap);
    pen.setJoinStyle(Qt::RoundJoin);
    painter.setPen(pen);

    float width = this->width();
    float height = this->height();

    QPointF point1(width * 0.24, height * 0.2);
    QPointF point2(width * 0.24, height * 0.8);
    QPointF point3(width * 0.64, height * 0.2);
    QPointF point4(width * 0.64, height * 0.8);

    QPainterPath path;
    path.moveTo(point1);
    path.lineTo(point2);

    path.moveTo(point3);
    path.lineTo(point4);

    painter.drawPath(path);
}

void Button::paintNone()
{

}
