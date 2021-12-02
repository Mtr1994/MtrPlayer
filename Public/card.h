#ifndef CARD_H
#define CARD_H

#include <QWidget>
#include <QPaintEvent>

class Card : public QWidget
{
    Q_OBJECT
public:
    explicit Card(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *e);

};

#endif // CARD_H
