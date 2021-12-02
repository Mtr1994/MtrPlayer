#include "card.h"

#include <QPainter>
#include <QPainterPath>

Card::Card(QWidget *parent) : QWidget(parent)
{

}

void Card::paintEvent(QPaintEvent *e)
{
    QPainter painter(this);
    painter.setPen(Qt::transparent);
    painter.setBrush(QBrush(QColor(0, 0, 0)));
    painter.setRenderHint(QPainter::Antialiasing, true);

    double radius = 9;
    QRectF rectContent = QRectF(0, 0, width(), height());

    QPainterPath path;
    path.addRoundedRect(rectContent, radius, radius);
    painter.setClipPath(path);
    painter.setClipping(true);

    painter.drawPath(path);

    e->accept();
}
