#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "eval.h"

#include <QStringListIterator>

class Interpreter {
public:
    Interpreter();
    ~Interpreter();
    
    Block * read(QString codeIn);
    void reset();
    
private:
    Action * getAction(const QString & token, QStringListIterator & it, Eval * parentBlock);
    Arithmetic * getArithmetic(const QString token, QStringListIterator & it, Eval * parentBlock);
    
    QMap<QString, Block*> procedures;
    Block grandpa;
};

#endif
