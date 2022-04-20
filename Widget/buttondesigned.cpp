#include "buttondesigned.h"

#include <QPainter>
#include <QPainterPath>

//test
#include <QDebug>

ButtonDesigned::ButtonDesigned(QWidget *parent) : QPushButton(parent)
{
    setMouseTracking(true);

    QFont font("Microsoft YaHei", 6);
    QFontMetrics metrics(font);

    QRect fontRect = metrics.boundingRect("H");

    setMinimumHeight(fontRect.height() * 2.4);
    setMinimumWidth(fontRect.height() * 2.4 * 1.5);
}

ButtonDesigned::ButtonDesigned(int id, QWidget *parent) : QPushButton(parent), mID(id)
{
    setMouseTracking(true);
}

void ButtonDesigned::setID(int id)
{
    if (id < 0 || id > Button_None) return;
    mID = id;
}

void ButtonDesigned::paintEvent(QPaintEvent *e)
{
    switch (mID) {
    case Button_Min:
        paintMin();
        break;
    case Button_Normal:
        paintNormal();
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

void ButtonDesigned::enterEvent(QEvent *e)
{
    e->accept();
    mMouseHovered = true;
    update();
}

void ButtonDesigned::leaveEvent(QEvent *e)
{
    e->accept();
    mMouseHovered = false;
    update();
}

void ButtonDesigned::paintMin()
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, false);

    QPen pen(QBrush(QColor(153, 153, 153)), 1);
    pen.setColor(QColor(153, 153, 153));
    pen.setCapStyle(Qt::RoundCap);
    pen.setJoinStyle(Qt::RoundJoin);

    float width = this->width();
    float height = this->height();

    if (mMouseHovered)
    {
        painter.fillRect(0, 0, this->width(), this->height(), QBrush(QColor(229, 229, 229)));
    }

    if (mMouseHovered)
    {
        pen.setColor(QColor(0, 0, 0));
        pen.setBrush(QBrush(QColor(0, 0, 0)));
    }

    painter.setPen(pen);

    QPointF point1((width - height / 3.0) / 2.0, height * 0.5);
    QPointF point2((width + height / 3.0) / 2.0, height * 0.5);

    QPainterPath path;
    path.moveTo(point1);
    path.lineTo(point2);

    painter.drawPath(path);
}

void ButtonDesigned::paintNormal()
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, false);

    QPen pen(QBrush(QColor(153, 153, 153)), 1);
    pen.setColor(QColor(153, 153, 153));
    pen.setCapStyle(Qt::RoundCap);
    pen.setJoinStyle(Qt::RoundJoin);

    float width = this->width();
    float height = this->height();

    if (mMouseHovered)
    {
        painter.fillRect(0, 0, this->width(), this->height(), QBrush(QColor(229, 229, 229)));
    }

    if (mMouseHovered)
    {
        pen.setColor(QColor(0, 0, 0));
        pen.setBrush(QBrush(QColor(0, 0, 0)));
    }
    painter.setPen(pen);

    QPointF point1((width - height / 3.0) / 2.0 + 2, height / 3.0 + 2);
    QPointF point2((width - height / 3.0) / 2.0 + 2, height / 3.0);
    QPointF point3((width + height / 3.0) / 2.0, height / 3.0);
    QPointF point4((width + height / 3.0) / 2.0, height / 3.0 * 2 - 2);
    QPointF point5((width + height / 3.0) / 2.0 - 2, height / 3.0 * 2 - 2);

    QPainterPath path;
    path.moveTo(point1);
    path.lineTo(point2);
    path.lineTo(point3);
    path.lineTo(point4);
    path.lineTo(point5);
    painter.drawPath(path);

    QPointF point6((width - height / 3.0) / 2.0, height / 3.0 + 2);
    QPointF point7((width + height / 3.0) / 2.0 - 2, height / 3.0 + 2);
    QPointF point8((width + height / 3.0) / 2.0 - 2, height / 3.0 * 2);
    QPointF point9((width - height / 3.0) / 2.0, height / 3.0 * 2);

    QPainterPath path2;
    path2.moveTo(point6);
    path2.lineTo(point7);
    path2.lineTo(point8);
    path2.lineTo(point9);
    path2.lineTo(point6);
    painter.drawPath(path2);
}

void ButtonDesigned::paintMax()
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, false);

    QPen pen(QBrush(QColor(153, 153, 153)), 1);
    pen.setColor(QColor(153, 153, 153));
    pen.setCapStyle(Qt::RoundCap);
    pen.setJoinStyle(Qt::RoundJoin);

    float width = this->width();
    float height = this->height();

    if (mMouseHovered)
    {
        painter.fillRect(0, 0, this->width(), this->height(), QBrush(QColor(229, 229, 229)));
    }

    if (mMouseHovered)
    {
        pen.setColor(QColor(0, 0, 0));
        pen.setBrush(QBrush(QColor(0, 0, 0)));
    }
    painter.setPen(pen);

    QPointF point1((width - height / 3.0) / 2.0, height / 3.0);
    QPointF point2((width + height / 3.0) / 2.0, height / 3.0);
    QPointF point3((width + height / 3.0) / 2.0, height / 3.0 * 2);
    QPointF point4((width - height / 3.0) / 2.0, height / 3.0 * 2);

    QPainterPath path;
    path.moveTo(point1);
    path.lineTo(point2);
    path.lineTo(point3);
    path.lineTo(point4);
    path.lineTo(point1);

    painter.drawPath(path);
}

void ButtonDesigned::paintClose()
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    QPen pen(QBrush(QColor(153, 153, 153)), 1.2);
    pen.setColor(QColor(153, 153, 153));
    pen.setCapStyle(Qt::RoundCap);
    pen.setJoinStyle(Qt::RoundJoin);

    float width = this->width();
    float height = this->height();

    if (mMouseHovered)
    {
        painter.fillRect(0, 0, this->width(), this->height(), QBrush(QColor(232, 17, 35)));
    }

    if (mMouseHovered)
    {
        pen.setColor(QColor(255, 255, 255));
        pen.setBrush(QBrush(QColor(255, 255, 255)));
    }

    painter.setPen(pen);

    QPointF point1((width - height / 3.0) / 2.0, height / 3.0);
    QPointF point2((width + height / 3.0) / 2.0, height / 3.0 * 2);
    QPointF point3((width + height / 3.0) / 2.0, height / 3.0);
    QPointF point4((width - height / 3.0) / 2.0, height / 3.0 * 2);

    QPainterPath path;
    path.moveTo(point1);
    path.lineTo(point2);
    path.moveTo(point3);
    path.lineTo(point4);

    painter.drawPath(path);
}

void ButtonDesigned::paintMenu()
{

}

void ButtonDesigned::paintStart()
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

void ButtonDesigned::paintVolume()
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

void ButtonDesigned::paintBackward()
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

void ButtonDesigned::paintPause()
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

void ButtonDesigned::paintNone()
{

}
