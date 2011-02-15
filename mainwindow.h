#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "sandbox.h"

#include <QMainWindow>

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow();
    ~MainWindow();
    
public slots:
    void about();
    
private:
    void createMenu();
    
    Sandbox* sandbox;
};

#endif
