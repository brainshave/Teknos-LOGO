#include "interpreter.h"
//#include "console.h"

#include <QMapIterator>
#include <QDebug>
#include <QStringList>
#include <QRegExp>

Interpreter::Interpreter()
: grandpa(NULL)
{}

Interpreter::~Interpreter() {
    reset();
}

void Interpreter::reset() {
    QMapIterator<QString, Block*> it(procedures);
    while(it.hasNext()) {
        Block* tmp = it.next().value();
        if(tmp) delete tmp;
    }
    procedures.clear();
    grandpa.reset();
}

Block * Interpreter::read(QString codeIn) {
    try {
        QString code;
        code = codeIn.replace(QRegExp("\\{[^}]*\\}"), " ").replace("["," [ ").replace("]"," ] ").toLower().simplified();
        
        //qDebug() << code;
        QStringList tokens(code.split(" ", QString::SkipEmptyParts));
        QStringListIterator it(tokens);
    
        /* hierarchia bloków: istnieje jeden blok główny (grandpa),
           do niego dołączane są w razie potrzeby (przy wystąpieniu [ ) kolejne,
           elementy dodawane są zawsze do najbardziej zewnętrznego bloku,
           czyli tego na początku tej listy: */
        QList<Block*> blocksHier;
        blocksHier.prepend(&grandpa);
        
        while (it.hasNext()) {
            QString token = it.next();
    
            //qDebug() << token;
            Eval * tmp = 0;
    
            if(tmp = getAction(token, it, blocksHier.first()));
            else if(tmp = getArithmetic(token, it, blocksHier.first()));
            
            // dodawanie bloku jako dziecka bloku aktualnego
            else if (token == "[") {
                Block * tmpblock = new Block(blocksHier.first());
                blocksHier.first()->addChild(tmpblock);
                blocksHier.prepend(tmpblock);
            }
            
            /* elementy w następnym obrocie pętli będą dodawane już do 'wyższego
               bloku' w hierarchii */
            else if (token == "]") {
                if (blocksHier.size() > 1) {
                    blocksHier.removeFirst();
                }
            }
            
            else if (token == "for") {
                if (! it.hasNext()) throw QString("For definition not ended");
                For * tmpfor = new For(blocksHier.first(), it.next());
                blocksHier.first()->addChild(tmpfor);
                
                // jeśli po forze otworzyliśmy blok
                if (it.hasNext() && (it.peekNext() == "[")) {
                    Block * bl = new Block(tmpfor);
                    tmpfor->setChild(bl);
                    
                    /* dzieki temu następne elementy będą wczytywane to nowego bloku
                       aż do ] */
                    blocksHier.prepend(bl);
                    it.next();
                }
                else if (it.hasNext() && QRegExp("\\D*").exactMatch(it.peekNext())){
                    QString tok2 = it.next();
                    Eval * tmpaction;
                    if(tmpaction = getAction(tok2, it, tmpfor));
                    else if(tmpaction = getArithmetic(tok2, it, tmpfor));
                    
                    if (tmpaction) tmpfor->setChild(tmpaction);
                    else if(procedures.contains(tok2)) {
                        tmpfor->setChild(new Proc(tmpfor, &procedures, tok2));
                    }
                    else throw QString("%1 - unknown operation/procedure").arg(tok2);
                }
                else if (it.hasNext() ) {
                    throw QString("%1 - number cannot be action").arg(it.next());
                }
            } else if (token == "block") {
                Block * tmpblock = new Block(&grandpa);
                
                if(it.hasNext() && it.peekNext() != "" && it.peekNext() != "[" ) {
                    QString procName = it.peekNext();
                    
                    if(procedures.contains(procName)) {
                        delete procedures[procName];
                        procedures.remove(procName);
                    }
                    
                    procedures.insert(procName, tmpblock);
                    
                    it.next();
                }
                
                if(it.hasNext() && it.peekNext() == "[") {
                    blocksHier.prepend(tmpblock);
                    it.next();
                } else if(it.hasNext()) {
                    Action * tmpaction = getAction(it.next(), it, tmpblock);
                    tmpblock->addChild(tmpaction);
                }
            } else if (procedures.contains(token)) {
                tmp = new Proc(blocksHier.first(), &procedures, token);
                //qDebug() << QString("Creatign Proc: %1, it's parent: %2").arg((int)tmp).arg((int)blocksHier.first());
            } else {
                throw QString("%1 - don't understand").arg(token);
            }
            
            if (tmp) blocksHier.first()->addChild(tmp);
        }
        //qDebug() << "wyszedlem z interperetera";
    } catch (QString err) {
        grandpa.clear();
        throw err;
    } catch (...) {
        grandpa.clear();
        throw 0;
    }
    return &grandpa;
}

Action * Interpreter::getAction(const QString & token, QStringListIterator & it, Eval * parentBlock) {
    Action * tmp = 0;
    if (token == "var") {
        tmp = new Action(parentBlock, it.hasNext() ? it.next() : "",
                         it.hasNext() ? it.next() : "");
    } else if (token == "fw") {
        tmp = new Action(parentBlock, Action::Go,
                         (it.hasNext() ? it.next() : ""));
    } else if (token == "bk") {
        tmp = new Action(parentBlock, Action::Go,
                         (it.hasNext() ? QString("-") + it.next() : ""));
    } else if (token == "lt") {
        tmp = new Action(parentBlock, Action::Rotate,
                         (it.hasNext() ? QString("-") + it.next() : ""));
    } else if (token == "rt") {
        tmp = new Action(parentBlock, Action::Rotate,
                         (it.hasNext() ? it.next() : ""));
    } else if (token == "rot") {
        tmp = new Action(parentBlock, Action::SetAngle,
                         (it.hasNext() ? it.next() : ""));
    } else if (token == "wait") {
        tmp = new Action(parentBlock, Action::Wait,
                         (it.hasNext() ? it.next() : ""));
    } else if (token == "up") {
        tmp = new Action(parentBlock, Action::Up, "");
    } else if (token == "down") {
        tmp = new Action(parentBlock, Action::Down, "");
    } else if (token == "reset") {
        tmp = new Action(parentBlock, Action::Reset, "");
    }
    return tmp;
}

Arithmetic * Interpreter::getArithmetic(const QString token, QStringListIterator & it, Eval * parentBlock) {
    QString opA, opB, target;
    Arithmetic * tmp = 0;
    if (it.hasNext()) {
        opA = it.next();
        if (it.hasNext()) {
            opB = it.next();
            if (it.hasNext()) {
                target = it.next();
                if (token == "+") {
                    tmp = new Arithmetic(parentBlock, Arithmetic::Plus, opA, opB, target);
                } else if (token == "-") {
                    tmp = new Arithmetic(parentBlock, Arithmetic::Minus, opA, opB, target);
                } else if (token == "/") {
                    tmp = new Arithmetic(parentBlock, Arithmetic::Divide, opA, opB, target);
                } else if (token == "*") {
                    tmp = new Arithmetic(parentBlock, Arithmetic::Multiply, opA, opB, target);
                } else if (token == "%") {
                    tmp = new Arithmetic(parentBlock, Arithmetic::Mod, opA, opB, target);
                } else if (token == "min") {
                    tmp = new Arithmetic(parentBlock, Arithmetic::Min, opA, opB, target);
                } else if (token == "max") {
                    tmp = new Arithmetic(parentBlock, Arithmetic::Max, opA, opB, target);
                } else if (token == "pow") {
                    tmp = new Arithmetic(parentBlock, Arithmetic::Pow, opA, opB, target);
                } else if (token == "root") {
                    tmp = new Arithmetic(parentBlock, Arithmetic::Root, opA, opB, target);
                } else if (token == "color") {
                    tmp = new Arithmetic(parentBlock, Arithmetic::Color, opA, opB, target);
                } else {
                    it.previous(); it.previous(); it.previous();
                }
            } else {
                it.previous(); it.previous();
            }
        } else {
            it.previous();
        }
    }
            
    return tmp;
}
