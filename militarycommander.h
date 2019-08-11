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
    constexpr static int leastLength = 25;
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
    constexpr static int baseAddress = 0x64010;
    constexpr static int lowIndexAddress = 0x6DE10;
    constexpr static int highIndexAdddress = 0x6DF10;

    constexpr static int attackAnimationAddress = 0x52281;
    constexpr static int deadAnimationAddress = 0x531B6;
    constexpr static int daJiangAdddress = 0x1F510;
    constexpr static int gongAdddress = 0x1F610;
    constexpr static int fangAdddress = 0x1F710;
    constexpr static int mingAdddress = 0x1F810;
    constexpr static int biAdddress = 0x1F910;
    QVector<Commander> commanderVector;
};

#endif // MILITARYCOMMANDER_H
