#include "sand.h"

#include <QPainter>
#include <QColor>
#include <QFrame>
#include <QRect>

Sand::Sand(QWidget *parent)
: QLabel(parent)
{
    this->setMinimumSize(100,100);
    pix = new QPixmap(2048, 2048);
    QPainter paint(pix);
    paint.fillRect(0, 0, 2048, 2048, QColor("white"));
    turtle = new Turtle(this, pix);
}

Sand::~Sand() {
    delete turtle;
    delete pix;
}

void Sand::paintEvent(QPaintEvent *e) {
    QPainter paint(this);
    int x, y, w, h;
    QRect rect = e->rect();
    w = rect.width();
    h = rect.height();
    x = ((pix->width() - this->width()) / 2 ) + rect.x();
    y = ((pix->height() - this->height()) / 2) + rect.y();
    
    // kopiuje widoczny środek pixmapy na widget
    paint.drawPixmap(rect.x(),rect.y(), *pix, x, y, w, h);
}

void Sand::resizeEvent(QResizeEvent *e) {
    turtle->refreshPos();
}

void Sand::exportImage(QString path) {
    // kopiuje widoczny obszar do pliku
    int x, y, w, h;
    w = this->width();
    h = this->height();
    x = (pix->width() - this->width()) / 2 ;
    y = (pix->height() - this->height()) / 2;
    pix->copy(x,y,w,h).save(path, "PNG", 0);
}

/////////////
// 2
void Sand::setImage(QString path) {
    if(path != "") {
        QPixmap tmp(path);
        QPainter paint(pix);
        int x = (pix->width() - tmp.width()) /2;
        int y = (pix->height() - tmp.height()) /2;
        paint.drawPixmap(x,y, tmp);
        this->update();
    }
}

Turtle* Sand::getTurtle() { return turtle; }
