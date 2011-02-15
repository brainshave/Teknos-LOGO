#ifndef SAND_H
#define SAND_H

#include "turtle.h"

#include <QLabel>
#include <QPixmap>
#include <QResizeEvent>
#include <QPaintEvent>
#include <QResizeEvent>

/* Sand zajmuje się pixmapą, która ma duży ustalony rozmiar (2048x2048)
   i z niej podczas paintEvent rysowany jest kawałek ze środka */

class Sand : public QLabel {
    Q_OBJECT
public:
    Sand(QWidget *parent);
    ~Sand();
    Turtle *getTurtle();
    
    // eksportuje widoczną część pixmapy
    void exportImage(QString path);
    //////////
    // 2
    void setImage(QString path);
protected:
    void resizeEvent(QResizeEvent *e);
    void paintEvent(QPaintEvent *e);
private:
    QPixmap *pix;
    Turtle *turtle;
};

#endif
