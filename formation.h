#ifndef FORMATION_H
#define FORMATION_H

#include <QWidget>
#include "ui_formation.h"
namespace Ui {
class Formation;
}

class Formation : public QWidget, public Ui::Formation
{
    Q_OBJECT

public:
    explicit Formation(QWidget *parent = nullptr);
    ~Formation();

private:

};

#endif // FORMATION_H
