#ifndef TURTLE_H
#define TURTLE_H

#include <QLabel>
#include <QPixmap>
#include <QThread>

class Turtle : public QLabel {
    Q_OBJECT
public:
    Turtle(QWidget *parent, QPixmap * pix);
    ~Turtle();
    
    //void setPos(int x, int y);
    void reset();
    void go(int pixels);
    void setAngle(double degree); //ustawia asdf bezwzględnie
    void rotate(double degree); // obraca w prawo
    void up();
    void down();
    void wait(int msecs);
    
    /* odświerza pozycję w oknie znając swoje przesunięcie
       względem pixmapy oraz nowego rozmiaru okna */
public slots:
    void paintEvent(QPaintEvent *e);
    void setAntialiasing(bool checked);
    void refreshPos();
    ///////////////////////////////
    // zad1 : ustawianie koloru
    void setColor(QColor color);
private:
    QWidget *parent;
    QPixmap *pix;
    /////////////////////////////
    // zad 1 trzymany kolor
    QColor color;
    
    QPixmap *face;
    bool isUp, antialias;
    double azimuth;
    double xPix, yPix; // pozycja na pixmapie
    
    class Clock : public QThread {
    public:
        void run();
        void waitOn(QThread* th, int msecs);
    };
    
    Clock clock;
};

#endif
