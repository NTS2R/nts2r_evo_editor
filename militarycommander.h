#ifndef MILITARYCOMMANDER_H
#define MILITARYCOMMANDER_H

#include <QWidget>
#include <QByteArray>
#include <QAxObject>
#include "ui_militarycommander.h"
#include <QVector>
#include "commander.h"
#include <QButtonGroup>

class MilitaryCommander : public QWidget, public Ui::MilitaryCommander
{
    Q_OBJECT
public:
    explicit MilitaryCommander(QWidget *parent = nullptr);
    constexpr static int leastLength = 25;
    void exportMilitary(QString filename);
    void importMilitary(QString filename);
    void importSpecial(QString filename);
public slots:
    void refreshMiliaryCommanderToListView();

private:
    void setCommanderList();
    void setSkillCheckBox(Commander const& commander);
    Commander updateCommander(Commander const& commander);
    void saveNesFile();
    QString getChsName(QString chsName);
    QString getChtName(QString chtName, quint8 chtNameControl);

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
    constexpr static int buhuoAddress = 0xF9610;
    constexpr static int zhanshaAddress = 0xF9710;
    constexpr static int attackCountAddress = 0xF8010;
    constexpr static int celveAddress = 0xFB210;
    constexpr static int mergeAddress = 0xF9910;
    // index 00 - 7F
    constexpr static int notCompositeAsObjetcstartAddress= 0xF9810;
    constexpr static int notCompositeToObjetcstartAddress= 0xF9890;
    // military limit
    constexpr static int militrayLimitLow = 0xF8210;
    constexpr static int militrayLimitHigh = 0xF8310;
    constexpr static int specialInvaild = 0x7bc4a;
    constexpr static int restoreInvaild = 0x7bc5c;
    constexpr static int successfulPercentInhalf = 0x7bc45;
    QVector<Commander> commanderVector;
    QButtonGroup buttonGroup;
};

#endif // MILITARYCOMMANDER_H
