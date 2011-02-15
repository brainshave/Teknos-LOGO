#include "mainwindow.h"

#include <QApplication>
#include <QMenuBar>
#include <QMenu>
#include <QKeySequence>
#include <QAction>
#include <QGridLayout>
#include <QMessageBox>

MainWindow::MainWindow() {
    this->setWindowTitle("Teknos LOGO");
    this->setGeometry(20, 20, 700, 500);
    
    sandbox = new Sandbox(this);
    this->setCentralWidget(sandbox);
    
    sandbox->setFocus();
    
    createMenu();
}

MainWindow::~MainWindow() {}

void MainWindow::createMenu() {
    QMenuBar *bar = this->menuBar();
    
    QMenu *menuFile = new QMenu("File", this);
    
    
    menuFile->addAction("Open...", sandbox, SLOT(load()),
                        QKeySequence("Ctrl+O"));
    menuFile->addAction("Save History As...", sandbox, SLOT(saveHistory()),
                        QKeySequence("Ctrl+H"));
    menuFile->addAction("Save Buffer As...", sandbox, SLOT(saveEditor()),
                        QKeySequence("Ctrl+S"));
    menuFile->addAction("Export Bitmap...", sandbox, SLOT(exportBitmap()),
                        QKeySequence("Ctrl+E"));
    menuFile->addAction("Quit", qApp, SLOT(closeAllWindows()),
                        QKeySequence("Ctrl+Q"));
    
    bar->addMenu(menuFile);
    
    QMenu *menuTurtle = new QMenu("Turtle", this);
    menuTurtle->addAction("Run Buffer", sandbox, SLOT(run()),
                          QKeySequence("Ctrl+Return"));
    menuTurtle->addAction("Clear Buffer", sandbox, SLOT(clearEditor()),
                          QKeySequence("Ctrl+K"));
    menuTurtle->addAction("Reset", sandbox, SLOT(reset()),
                          QKeySequence("Ctrl+R"));
    
    bar->addMenu(menuTurtle);
    
    QMenu *menuOptions = new QMenu("Options", this);
    QAction * actionAA = menuOptions->addAction("Antialiasing", sandbox, SLOT(setAntialiasing(bool)));
    actionAA->setCheckable(true);
    
    ///////////
    // zad 2
    menuOptions->addAction("Set Background...", sandbox, SLOT(loadBackground()));
    
    bar->addMenu(menuOptions);
    
    QMenu *menuHelp = new QMenu("Help", this);
    
    menuHelp->addAction("About", this, SLOT(about()));
    
    bar->addMenu(menuHelp);
    
}

void MainWindow::about() {
    QMessageBox::about(this, "Teknos",
        tr("copywrong 2009 by Szymon Witamborski"));
}
