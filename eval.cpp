#include "eval.h"
#include "turtle.h"

#include <QVectorIterator>
#include <QDebug>
#include <QRegExp>

#include <math.h>

Eval::Eval(Eval *parent = 0)
: parent(parent)
{}

void Eval::setParent(Eval * newParent) {
    parent = newParent;
}
Eval * Eval::getParent() { return parent; }


int Eval::get(QString & what) {
    int val = 0;
    if(QRegExp("[\\d-]+").exactMatch(what)) {
        return what.toInt();
    } else if(getValue(what, &val)) {
        return val;
    } else if(parent) {
        return parent->get(what);
    }
    else throw QString("Undefined variable \"%1\"").arg(what);
}


/* Działanie set jest proste, jest zrobione tak, żeby nie trzeba było
   pisać jej osobno w każdej klasie dziedziczącej po Eval.
   Na początku jest wywoływana w obiektach typu Action lub Arithmetic,
   które nie potrafią same z siebie przechowywać zmiennych, więc 'zlecają'
   to bloku nadrzędnemu wywołując je z deep = 0.
   Najważniejszy jest parametr deep, który określa gdzie ma być ustawiona
   zmienna jeśli nie było jej wcześniej w żadnym bloku aż do najbardziej
   zewnętrznego. Za każdym razem gdy wywoływane jest set z parenta, deep jest
   zwiększane o 1. Jeśli natomiast wywołania dotarły do bloku nadrzędnego,
   set zwraca deep zmiejszane o 1. Generalnie blok powinien dostać od parenta
   taką samą wartość z jaką został wywołany. Jeśli jednak w którymś z parentów
   isnitała zmienna o takiej nazwie to jest ustawiana tam i ten blok zwraca -1.
   Jeśli do któregoś bloku zostało zwrócone 0 z parenta, tzn. że w tym bloku
   musi ta zmienna być ustawiona */
int Eval::set(QString & what, int value, int deep) {
    if(this->getValue(what)) { // jeśli w tym zasięgu już była wcześniej zdefiniowana
        this->setValue(what,value);
        return -1;
    } else if(parent) {
        int pdeep;
        if(pdeep = parent->set(what, value, deep + 1)) { // jesli parent nie zwrócił 0
            return pdeep - 1;
        } else { // jeśli parent zwrócił 0 - ustawiamy
            this->setValue(what, value);
            return -1;
        }
    } else {
        /* jeśli jesteśmy w najbardziej zewnętrznym bloku i zostaliśmy wywołani
           z deep == 0 */
        if(deep == 0) {
            this->setValue(what, value);
            return -1;
        } else {
            return deep - 1;
        }
    }
}

bool Eval::getValue(QString & what, int * place) {
    return false;
}
void Eval::setValue(QString & what, int value) {
    throw QString("Can't set value in simple Eval object");
}

Eval::~Eval() {}

/******** class Action ********/

Action::Action(Eval * parent, int func, QString param)
: Eval(parent), func(func), param(param)
{}

// konstruktor dla Var:
Action::Action(Eval * parent, QString varName, QString param)
: Eval(parent), func(Var), param(param), varName(varName)
{}

Action::~Action() {}

Eval * Action::clone(Eval * newParent) {
    if (func == Var) return new Action(newParent, varName, param);
    else return new Action(newParent, func, param);
}

void Action::eval(Turtle* turtle) {
    switch(func) {
    case Go:        turtle->go(get(param));         break;
    case Reset:     turtle->reset();                break;
    case Rotate:    turtle->rotate(get(param));     break;
    case SetAngle:  turtle->setAngle(get(param));   break;
    case Up:        turtle->up();                   break;
    case Down:      turtle->down();                 break;
    case Wait:      turtle->wait(get(param));       break;
    case Var:       set(varName, get(param), -1);   break;
    }
}

/******* class Arithmetic *******/

Arithmetic::Arithmetic(Eval *parent, int func, QString opA, QString opB, QString target)
: Eval(parent), opA(opA), opB(opB), target(target), func(func)
{}

Arithmetic::~Arithmetic() {}
    
void Arithmetic::eval(Turtle * turtle) {
    int a = get(opA);
    int b = get(opB);
    int c = 0;
    if (! b && ( func == Divide || func == Mod || func == Root ) )
        throw QString("Div/Mod/Root by zero: %1, %2").arg(a).arg(b);
    
    switch(func) {
    case Plus:      set(target, a + b, -1);           break;
    case Minus:     set(target, a - b, -1);           break;
    case Divide:    set(target, a / b, -1);           break;
    case Multiply:  set(target, a * b, -1);           break;
    case Mod:       set(target, a % b, -1);           break;
    case Min:       set(target, qMin(a, b), -1);      break;
    case Max:       set(target, qMax(a, b), -1);      break;
    case Pow:       set(target, qRound(pow((double)a, b)), -1);     break;
    case Root:      set(target, qRound(pow((double)a, 1.0/(double)b)), -1); break;
        ////////////////////////
        // zad 1
    case Color:
        c = get(target);
        if(a > 255 || b > 255 || c > 255 || a < 0 || b < 0 || c < 0)
            throw QString("Color out of bounds");
        
        turtle->setColor(QColor(a,b,c));
    break;
    }
}

Eval * Arithmetic::clone(Eval * newParent) {
    return new Arithmetic(newParent, func, opA, opB, target);
}

/******** class Block ********/

Block::Block(Eval *parent)
: Eval(parent)
{}

Block::~Block() {
    clear();
}

Eval * Block::clone(Eval * newParent) {
    Block * tmpblock = new Block(newParent);
    QVectorIterator<Eval*> it(children);
    
    while(it.hasNext()) {
        tmpblock->addChild(it.next()->clone(tmpblock));
    }
    
    tmpblock->vars = this->vars;
    return tmpblock;
}

void Block::clear() {
    QVectorIterator<Eval*> it(children);
    while(it.hasNext()) {
        if(it.peekNext())
            delete it.next();
    }
    children.clear();
}

void Block::reset() {
    vars.clear();
    clear();
}

Block & Block::addChild(Eval *child) {
    children.append(child);
    return *this;
}

void Block::eval(Turtle* turtle) {
    QVectorIterator<Eval*> it(children);
    while(it.hasNext()) {
        it.next()->eval(turtle);
    }
}

bool Block::getValue(QString & what, int * place) {
    if(vars.contains(what)) {
        if (place) *place = vars[what];
        return true;
    } else {
        return false;
    }
}
void Block::setValue(QString & what, int value) {
    vars[what] = value;
}

/******** class For ********/

For::For(Eval * parent, QString param)
: Eval(parent), child(0), param(param)
{}

For::~For() {
    if (child) delete child;
}

Eval * For::clone(Eval * newParent) {
    For * tmpfor = new For(newParent, param);
    tmpfor->setChild(this->child->clone(tmpfor));
    return tmpfor;
}

void For::setChild(Eval * newChild) {
    if (child) delete child;
    child = newChild;
}

void For::eval(Turtle* turtle) {
    if(child) {
        for ( int i = 0; i < get(param); ++i) {
            child->eval(turtle);
        }
    }
}


bool For::getValue(QString & what, int * place) {
    if(vars.contains(what)) {
        if (place) *place = vars[what];
        return true;
    } else {
        return false;
    }
}

void For::setValue(QString & what, int value) {
    vars[what] = value;
}

/******** class Proc ********/

Proc::Proc(Eval *parent, QMap<QString, Block*> * procs, QString procname)
: Eval(parent), procedures(procs), procname(procname)
{}

Proc::~Proc() {}

Eval * Proc::clone(Eval * newParent) {
    return new Proc(newParent, procedures, procname);
}


void Proc::eval(Turtle * turtle) {
    if (procedures->contains(procname) ) {
        /* całe klonowanie zaczyna się zawsze tutaj: */
        Eval * tmp = procedures->value(procname)->clone(this);
        tmp->eval(turtle);
        delete tmp;
    }
}
