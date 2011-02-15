#ifndef CONSOLE_H
#define CONSOLE_H

#include <QWidget>
#include <QTextEdit>
#include <QListWidget>
#include <QString>
#include <QTabWidget>
#include <QContextMenuEvent>

/* Console trzyma edytor, historię i konsolę błędów (w zakładkach) */

//////////////////////
// zad 3

class History : public QListWidget {
public:
    History(QWidget * parent);
    ~History();
    void contextMenuEvent ( QContextMenuEvent * event );
private:
    QMenu * menu;
};

class Console : public QWidget {
    Q_OBJECT
public:
    Console(QWidget * parent);
    ~Console();
    void readFile(QString path);
    void saveBuffer(QString path);
    void saveHistory(QString path);
    
    void reportError(QString text);
    QString readInput();
public slots:
    void reset();
    void clearEditor();
    void insertIntoEditor(QListWidgetItem*);
private:
    QTextEdit *input;
    QListWidget *history, *errors;
    QTabWidget *tabs;
};

#endif
