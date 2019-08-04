#ifndef MILITARYCOMMANDER_H
#define MILITARYCOMMANDER_H

#include <QWidget>
#include <QByteArray>
#include "ui_militarycommander.h"
#include <QVector>
#include "commander.h"

class MilitaryCommander : public QWidget, public Ui::MilitaryCommander
{
    Q_OBJECT
public:
    explicit MilitaryCommander(QWidget *parent = nullptr);
    constexpr static int least_length = 25;
public slots:
    void refreshMiliaryCommanderToListView();
private:
    void setCommanderList();
    void setSkillCheckBox(Commander const& commander);
    Commander updateCommander(Commander const& commander);
    void saveNesFile();

private slots:
    void setCurrentItem();
    void on_saveButton_clicked();

private:
    constexpr static int base_address = 0x64010;
    constexpr static int low_index_address = 0x6DE10;
    constexpr static int hight_index_address = 0x6DF10;

    QVector<Commander> commanderVector;
};

#endif // MILITARYCOMMANDER_H
