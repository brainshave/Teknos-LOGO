#ifndef SANDBOX_H
#define SANDBOX_H

#include "sand.h"
#include "console.h"
#include "turtle.h"
#include "interpreter.h"

#include <QWidget>

/* Sandbox - klasa, która trzyma w sobie obiekt klasy Console (edytor, historia,
   błędy, Sand, czyli pole na którym jest żółw oraz żółwia, oraz pojedyńczy
   obiekt interpretera */

class Sandbox : public QWidget {
    Q_OBJECT
public:
    Sandbox(QWidget * parent);
    ~Sandbox();
    
public slots:
    // uruchamia to co aktualnie w edytorze
    void run();
    
    // resetuje wszystko (żółwia, pixmapę i konsolę)
    void reset();
    // resetuje tylko edytor (Ctrl+K)
    void clearEditor();
    void load();
    void saveHistory();
    void saveEditor();
    void exportBitmap();
    
    ///////////////
    // 2
    void loadBackground();
    
    void setAntialiasing(bool checked);
private:
    Sand *sand;
    Console *console;
    Turtle * turtle;
    
    Interpreter reader;
};

#endif
