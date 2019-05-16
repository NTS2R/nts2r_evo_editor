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
        commanderVector.push_back(commanderValue);
//        qDebug() << commanderValue.toHex();
    }
    for (auto& commander : commanderVector) {
        qDebug() << commander;
    }
    setCommanderList();
}

void MilitaryCommander::setCommanderList() {
    this->commanderList->clear();
    for (int index = 0x00; index <= 0xFF; ++index) {
        QByteArray commander = commanderVector[index];
        QByteArray name = commander.mid(least_length);
        qDebug() << name.toHex();
        commanderList->addItem(
                    QString("%1 : %2").arg(index, 2, 16, QChar('0')).arg( name.toHex().data() )
                    );
    }
}

void MilitaryCommander::setCurrentItem() {
    auto index = commanderList->currentRow();
    auto commander = commanderVector[index];
    textEdit->setText(commander.toHex(' '));
    //智力 offset2
    zhiliText->setText(QString("%1").arg(static_cast<quint8>(commander.at(2))));
    //武力 offset3
    wuliText->setText(QString("%1").arg(static_cast<quint8>(commander.at(3))));
    //速度 offset4
    suduText->setText(QString("%1").arg(static_cast<quint8>(commander.at(4))));
    quint8 offset0 = static_cast<quint8>(commander.at(0));
    //颜色 offset0 low
    colorText->setText(QString("%1").arg(offset0 & 0xf, 0, 16));
    //章节 offset0 high
    chapterText->setText(QString("%1").arg(offset0 >> 4, 0, 16));
    //模型 offset1
    modelText->setText(QString("%1").arg(static_cast<quint8>(commander.at(1)), 0, 16));

    //CHS角色名字 offset25 - 最后
    auto chsName = commander.mid(least_length).toHex(' ');
    simpliedNameText->setText(chsName);
    //CHT角色名字 offset22 -24
    auto chtName = commander.mid(22, 3).toHex(' ');
    traditionalNameText->setText(chtName);
}
