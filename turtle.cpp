#include "turtle.h"

#include <QPainter>
#include <QColor>
#include <QBrush>
#include <QDebug>

#define _USE_MATH_DEFINES
#include <math.h>

Turtle::Turtle(QWidget *parent, QPixmap *npix)
:   QLabel(parent),
    parent(parent),
    pix(npix),
    face(NULL),
    isUp(false),
    antialias(false),
    azimuth(0.0),
    color(0,0,0)
{
    face = new QPixmap(":/turtel.png");
    if(face) {
        this->resize(face->width(), face->height());
    } else {
        this->setText("Z");
    }
    
    xPix = pix->width() / 2;
    yPix = pix->width() / 2;
    
    clock.start();
}

Turtle::~Turtle() {
    clock.quit();
    clock.wait();
}

void Turtle::reset() {
    xPix = pix->width() / 2;
    yPix = pix->height() / 2;
    azimuth = 0.0;
    
    ////////
    // zad 1
    color = QColor(0,0,0);
    
    QPainter paint(pix);
    paint.fillRect(0,0, pix->width(), pix->height(), QColor("white"));
    
    refreshPos();
    
    parent->update();
}
void Turtle::setAngle(double degree) {
    azimuth = degree;
    refreshPos();
    update();
}
void Turtle::rotate(double degree) {
    azimuth += degree;
    update();
    refreshPos();
}
void Turtle::up() {
    isUp = true;
}
void Turtle::down() {
    isUp = false;
}

void Turtle::setColor(QColor color) {
    this->color = color;
}

void Turtle::go(int pixels) {
    double rads = (-(azimuth-90.0) * M_PI) / 180.0;
    double dpix = pixels;
    double offsetX = cos(rads) * dpix;
    double offsetY = sin(rads) * dpix;

    if(! this->isUp ) {
        QPainter paint(pix);
        
        //////////////
        // zad 1
        paint.setPen(color);
        /////////////
        
        if (antialias) paint.setRenderHint(QPainter::Antialiasing);
        paint.drawLine(qRound(xPix), qRound(yPix),
                       qRound(xPix + offsetX), qRound(yPix - offsetY));
    }
    
    xPix += offsetX;
    yPix -= offsetY;
    
    refreshPos();
    parent->update();
}
void Turtle::setAntialiasing(bool checked) {
    antialias = checked;
}

void Turtle::paintEvent(QPaintEvent *e) {
    if(face) {
        this->resize(face->width(), face->height());
        QPainter paint(this);
        paint.setRenderHint(QPainter::SmoothPixmapTransform);
        paint.translate(face->width()/2, face->height()/2);
        //paint.translate(20,20);
        paint.rotate(azimuth);
        paint.translate(-face->width()/2, -face->height()/2);
        paint.drawPixmap(0,0, *face);
    }
}

void Turtle::wait(int msecs) {
    parent->repaint();
    //msleep(msecs);
    clock.waitOn(QThread::currentThread(), msecs);
    // clock.wait(msecs/4);
    // clock.wait(msecs/4);
    // clock.wait(msecs/4);
    // clock.wait(msecs/4);
}

void Turtle::refreshPos() {
    this->move(
        xPix - (pix->width() - parent->width() + this->width() ) / 2,
        yPix - (pix->height() - parent->height() + this->height() ) / 2);
}

void Turtle::Clock::run() { exec(); }

void Turtle::Clock::waitOn(QThread* th, int msecs) {
    msleep(msecs);
}
