/* copywrong 2009 Szymon Witamborski */

#include "mainwindow.h"

#include <QApplication>

int main (int argc, char ** argv) {
    QApplication app(argc, argv);
    
    MainWindow window;
    window.show();
    
    return app.exec();
}
