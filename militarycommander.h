#ifndef MILITARYCOMMANDER_H
#define MILITARYCOMMANDER_H

#include <QWidget>
#include "ui_militarycommander.h"

class MilitaryCommander : public QWidget, public Ui::MilitaryCommander
{
    Q_OBJECT
public:
    explicit MilitaryCommander(QWidget *parent = nullptr);

signals:

public slots:
};

#endif // MILITARYCOMMANDER_H
