#ifndef EVAL_H
#define EVAL_H

#include <QString>
#include <QMap>
#include <QVector>

class Turtle;

/* klasa Eval jest klasą bazową dla wszystkich klas wykonujących jakieś
   czynności, definiuje kilka metod, najważniejsze:
   eval(turtle) - wykonuje się na danym żółwiu
   clone(newParent) - zwraca nową kopię siebie z przypisanym jako rodzic
        newParent (potrzebne do wywołań procedur, które przed wywołaniem są
        kopiowane)
   get(what) - pobiera zmienną o podanej nazwie lub konwertuje tekst na inta
   set(what, int value, int deep) - próbuje ustawić zmienną, więcej w eval.cpp
   
   virtualne metody getValue i setValue muszą być nadpisane, jeśli klasa jest
   zdolna przechowywać tablicę zmiennych (tutaj jest to For i Block)
   
   W tym pliku zdefiniowane są jeszcze klasy Action, Arithmetic, Block, For
   i Proc. opisane są poniżej.
*/

class Eval {
public:
    Eval(Eval *parent);
    virtual ~Eval();
    
    void setParent(Eval * newParent);
    Eval * getParent();
    
    virtual Eval * clone(Eval * newParent) = 0;
    
    virtual void eval(Turtle* turtle) = 0;
    
    int get(QString & what);
    int set(QString & what, int value, int deep);
    
protected:
    virtual bool getValue(QString & what, int * place = NULL);
    virtual void setValue(QString & what, int value);
    Eval *parent;
};


/* klasa bazowa dla wszystkich podstawowych akcji + Var, czyli ustawianie
   zmiennej */
class Action : public Eval {
public:
    enum { Go, Rotate, Reset, SetAngle, Up, Down, Wait, Var };
    //Action(Eval *parent, int func, int param);
    
    // konstruktor dla wszystkich akcji z wyjątkiem Var
    Action(Eval *parent, int func, QString paramName);
    // konstruktor dla Var
    Action(Eval *parent, QString varName, QString param);
    ~Action();
    
    Eval * clone(Eval * newParent);
    
    void eval(Turtle* turtle);

private:
    QString param, varName;
    int func;
};

/* klasa Arithmetic obsługuje 3-argumentowe operacje arytmetyczne,
   trzeci argument określa zmienną, gdzie ma trafić wynik */
class Arithmetic : public Eval {
public:
    enum { Plus, Minus, Divide, Multiply, Mod, Min, Max, Pow, Root, Color };
    
    Arithmetic(Eval *parent, int func, QString opA, QString opB, QString target);
    ~Arithmetic();
    
    void eval(Turtle * turtle);
    Eval * clone(Eval * newParent);
    
private:
    QString opA, opB, target;
    int func;
};

/* klasa Block, może przechowywać wiele 'dzieci', służy jako baza dla procedur
*/
class Block : public Eval {
public:
    Block(Eval *parent);
    ~Block();
    Eval * clone(Eval * newParent);
    
    Block & addChild(Eval *child);
    
    void clear();
    void reset();
    
    void eval(Turtle* turtle);
    
protected:
    bool getValue(QString & what, int * place = NULL);
    void setValue(QString & what, int value);

    QVector<Eval*> children;
    QMap<QString, int> vars; 
};

/* pętla For z jednym argumentem */
class For : public Eval {
public:
    For(Eval *parent, QString param);
    For(const For & old);
    ~For();
    Eval * clone(Eval * newParent);
    
    void eval(Turtle* turtle);
    void setChild(Eval * child);
    
protected:
    bool getValue(QString & what, int * place = NULL);
    void setValue(QString & what, int value);
    
private:
    Eval * child;
    QString param;
    QMap<QString, int> vars; 
};

// klasa Proc, kopiuje blok procedury dopiero w momencie wywołania
class Proc : public Eval {
public:
    Proc(Eval *parent, QMap<QString, Block*> * procs, QString procname);
    ~Proc();
    
    Eval * clone(Eval * newParent);
    void eval(Turtle * turtle);
    
private:
    QString procname;
    QMap<QString, Block*> * procedures;
};

#endif
