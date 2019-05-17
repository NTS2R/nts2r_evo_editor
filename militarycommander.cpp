#include "militarycommander.h"
#include "mainwindow.h"
#include <QDebug>
#include <QListWidget>
MilitaryCommander::MilitaryCommander(QWidget *parent) : QWidget(parent)
{
    this->setupUi(this);
    connect(this->refreshButton, &QPushButton::clicked,
            this, &MilitaryCommander::refreshMiliaryCommanderToListView);
    connect(this->commanderList, &QListWidget::itemSelectionChanged,
            this, &MilitaryCommander::setCurrentItem);
}

void MilitaryCommander::refreshMiliaryCommanderToListView() {
    qDebug() << "Refresh";
    this->commanderVector.clear();

    auto& nes = MainWindow::nesFileByteArray;

    for (int index = 0x00; index <= 0xFF; ++index) {
        quint8 low_address = static_cast<quint8>(nes.at(low_index_address + index));
        quint8 high_address = static_cast<quint8>(nes.at(hight_index_address + index));
//        qDebug() << QString("low: %1, high: %2").arg(low_address).arg(high_address);
        int commander = base_address + high_address * 0x100 + low_address;

        QByteArray commanderValue;
        for (int i = 0; i < least_length; ++i) {
            commanderValue.append(nes.at(commander + i));
        }
        int startIndex = least_length;
        while (true) {
            auto value = nes.at(commander + startIndex++);
            commanderValue.append(value);
            if (static_cast<quint8>(value) == static_cast<quint8>(0xFF))
                break;
        }
        Commander commanderData;
        commanderData.setCommanderAttribute(commanderValue);
        commanderVector.push_back(commanderData);
    }
    setCommanderList();
}

void MilitaryCommander::setCommanderList() {
    this->commanderList->clear();
    for (int index = 0x00; index <= 0xFF; ++index) {
        auto commander = commanderVector[index];
        qDebug() << commander.chsName;
        commanderList->addItem(
                    QString("%1 : %2").arg(index, 2, 16, QChar('0')).arg( commander.chsName )
                    );
    }
}

void MilitaryCommander::setCurrentItem() {
    auto index = commanderList->currentRow();
    auto commander = commanderVector[index];
    textEdit->setText(commander.data.toHex(' '));
    //智力
    zhiliText->setText(QString("%1").arg(commander.zhili));
    //武力
    wuliText->setText(QString("%1").arg(commander.wuli));
    //速度
    suduText->setText(QString("%1").arg(commander.sudu));
    //颜色
    colorText->setText(QString("%1").arg(commander.color, 0, 16));
    //章节
    chapterText->setText(QString("%1").arg(commander.chapter, 0, 16));
    //模型
    modelText->setText(QString("%1").arg(commander.model, 0, 16));

    wofangliupaiText->setPlainText(QString("%1").arg(commander.wofangliupai, 2, 16, QChar('0')));
    difangliupaiText->setPlainText(QString("%1").arg(commander.difangliupai, 2, 16, QChar('0')));
    diaobaoliupaiText->setPlainText(QString("%1").arg(commander.diaobaoliupai, 2, 16, QChar('0')));
    faceText->setPlainText(commander.face);
    faceControlText->setPlainText(QString("%1").arg(commander.faceControl, 0, 16));
    //CHS角色名字 offset25 - 最后
    simpliedNameText->setText(commander.chsName);
    //CHT角色名字 offset22 -24
    traditionalNameText->setText(commander.chtName);
    chtNameControlText->setPlainText(QString("%1").arg(commander.chtNameControl, 0, 16));
}
