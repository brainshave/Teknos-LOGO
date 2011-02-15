#include "console.h"

#include <QVBoxLayout>
#include <QDebug>
#include <QFile>
#include <QTextCodec>
#include <QMenu>

//////////////////////
// zad 3

History::History(QWidget * parent) : QListWidget(parent) {
    
    menu = new QMenu(this);
    
    menu->addAction("Clear History", this, SLOT(clear()));
}
History::~History() {}

void History::contextMenuEvent ( QContextMenuEvent * event ) {
    
    menu->popup(event->globalPos());
}


Console::Console(QWidget * parent)
: QWidget(parent)
{
    this->setContentsMargins(0,0,0,0);
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setSpacing(0);
    
    layout->setContentsMargins(0,0,0,0);
    
    tabs = new QTabWidget(this);
    input = new QTextEdit(tabs);
    input->setAcceptRichText(false);
    //input->grabKeyboard();
    tabs->addTab(input, "Buffer");
    
    history = new History(tabs);
    history->setSpacing(2);
    QObject::connect(history, SIGNAL(itemDoubleClicked(QListWidgetItem *)),
                     this, SLOT(insertIntoEditor(QListWidgetItem*)));
    tabs->addTab(history, "History");
    
    errors = new QListWidget(tabs);
    history->setSpacing(2);
    tabs->addTab(errors, "Errors");
    
    layout->addWidget(tabs);
    this->setLayout(layout);
    
    this->setFocus();
    tabs->setFocus();
    input->setFocus();
}

QString Console::readInput() {
    QString text = input->toPlainText();
    history->addItem(text);
    history->setCurrentItem(history->item(history->count() - 1));
    return text;
}

void Console::reportError(QString text) {
    errors->addItem(text);
    errors->setCurrentItem(errors->item(errors->count() - 1));
    tabs->setCurrentWidget(errors);
}

void Console::insertIntoEditor(QListWidgetItem* item) {
    input->insertPlainText(item->text());
}

void Console::reset() {
    clearEditor();
    history->clear();
    errors->clear();
}
void Console::clearEditor() {
    input->clear();
}

void Console::readFile(QString path) {
    if (!QFile::exists(path)) {
        this->reportError(QString("File does not exists %1").arg(path));
        return;
    }
    
    QFile file(path);
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextCodec * codec = QTextCodec::codecForName("UTF-8");
    
    input->setPlainText(codec->toUnicode(file.readAll()));
    file.close();
}


void Console::saveBuffer(QString path) {
    QFile file(path);
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextCodec * codec = QTextCodec::codecForName("UTF-8");
    
    file.write(codec->fromUnicode(input->toPlainText()));
    file.close();
}
void Console::saveHistory(QString path) {
    QFile file(path);
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextCodec * codec = QTextCodec::codecForName("UTF-8");
    
    for(int i = 0; i < history->count(); ++i) {
        file.write(codec->fromUnicode(history->item(i)->text()));
        file.write("\n");
    }
    
    file.close();
}
Console::~Console() {}
