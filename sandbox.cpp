#include "sandbox.h"
#include "eval.h"

#include <QGridLayout>
#include <QDebug>
#include <QShortcut>
#include <QFileDialog>

Sandbox::Sandbox(QWidget *parent)
: QWidget(parent)
{
    this->setContentsMargins(0,0,0,0);
    QGridLayout *layout = new QGridLayout;
    
    layout->setContentsMargins(0,0,0,0);
    layout->setSpacing(5);
    sand = new Sand(this);
    layout->addWidget(sand, 0,0);
    layout->setColumnStretch(0, 2);
    
    turtle = sand->getTurtle();
    /*for(int i = 1; i <= 40; ++i) {
        turtle->rotate(30);
        turtle->go(i);
    }*/
    
    console = new Console(this);
    layout->addWidget(console, 0,1);
    layout->setColumnStretch(1,1);
    console->setFocus();
    this->setLayout(layout);
}

Sandbox::~Sandbox() {
    delete sand;
    delete console;
}

void Sandbox::run() {
    Block * object = 0;
    try {
        object = reader.read(console->readInput());
        if (object) {
            //qDebug() << "eval";
            object->eval(turtle);
            //qDebug() << "clear";
            object->clear();
        }    
    } catch (QString err) {
        console->reportError(err);
        if(object) object->clear();
    } catch (...) {
        console->reportError(QString("Unknown error"));
    }
}

void Sandbox::reset() {
    console->reset();
    turtle->reset();
    reader.reset();
}
void Sandbox::setAntialiasing(bool checked) {
    turtle->setAntialiasing(checked);
}

void Sandbox::clearEditor() {
    console->clearEditor();
}

void Sandbox::load() {
    QString name = QFileDialog::getOpenFileName(this, "Open file", "", "LOGO files(*.logo)");
    if (name.length()) console->readFile(name);
}


void Sandbox::loadBackground() {
    QString name = QFileDialog::getOpenFileName(this, "Load Background", "", "Images (*.png *.jpg *.gif *.bmp *.jpeg)");
    if (QFile::exists(name)) sand->setImage(name);
}

void Sandbox::saveEditor() {
    QString name = QFileDialog::getSaveFileName(this, "Save Buffer", "asdf.logo", "LOGO files (*.logo)");
    if (name.length()) console->saveBuffer(name);
}
void Sandbox::saveHistory() {
    QString name = QFileDialog::getSaveFileName(this, "Save History", "asdf.logo", "LOGO files (*.logo)");
    if (name.length()) console->saveHistory(name);
}
void Sandbox::exportBitmap() {
    QString name = QFileDialog::getSaveFileName(this, "Export PNG Image", "logo_image.png", "PNG files (*.png)");
    if (name.length()) sand->exportImage(name);
}
