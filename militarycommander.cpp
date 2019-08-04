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
        int commanderAddress = base_address + high_address * 0x100 + low_address;

        QByteArray commanderValue;
        for (int i = 0; i < least_length; ++i) {
            commanderValue.append(nes.at(commanderAddress + i));
        }
        int startIndex = least_length;
        while (true) {
            auto value = nes.at(commanderAddress + startIndex++);
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

void MilitaryCommander::saveNesFile() {
    auto& nes = MainWindow::nesFileByteArray;

    for (int index = 0x00; index <= 0xFF; ++index) {
        quint8 low_address = static_cast<quint8>(nes.at(low_index_address + index));
        quint8 high_address = static_cast<quint8>(nes.at(hight_index_address + index));
//        qDebug() << QString("low: %1, high: %2").arg(low_address).arg(high_address);
        int commanderAddress = base_address + high_address * 0x100 + low_address;
        auto byteArray = commanderVector[index].data;
        for (int i = 0; i < byteArray.length(); i++) {
            nes[commanderAddress + i] = byteArray[i];
        }
    }
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

void MilitaryCommander::setSkillCheckBox(const Commander &commander) {
    auto skillRenHuiDang = commander.skillRenHuiDang;
    skillRen->setChecked(skillRenHuiDang & 0b100);
    skillHui->setChecked(skillRenHuiDang & 0b010);
    skillDang->setChecked(skillRenHuiDang & 0b001);
    auto skillBiGongWuZhi = commander.skillBiGongWuZhi;
    skillBi->setChecked(skillBiGongWuZhi & 0b1000);
    skillWu->setChecked(skillBiGongWuZhi & 0b0100);
    skillGong->setChecked(skillBiGongWuZhi & 0b0010);
    skillZhi->setChecked(skillBiGongWuZhi & 0b0001);
    auto skillFanHunJue = commander.skillFanHunJue;
    skillFan->setChecked(skillFanHunJue & 0b100);
    skillHun->setChecked(skillFanHunJue & 0b010);
    skillJue->setChecked(skillFanHunJue & 0b001);
    auto skillFangMouYiLin = commander.skillFangMouYiLin;
    skillFang->setChecked(skillFangMouYiLin & 0b1000);
    skillMou->setChecked(skillFangMouYiLin & 0b0100);
    skillYi->setChecked(skillFangMouYiLin & 0b0010);
    skillLin->setChecked(skillFangMouYiLin & 0b0001);
    auto skillShiFenTongMing = commander.skillShiFenTongMing;
    skillShi->setChecked(skillShiFenTongMing & 0b1000);
    skillFen->setChecked(skillShiFenTongMing & 0b0100);
    skillTong->setChecked(skillShiFenTongMing & 0b0010);
    skillMing->setChecked(skillShiFenTongMing & 0b0001);
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
    setSkillCheckBox(commander);
}

Commander MilitaryCommander::updateCommander(const Commander &commander) {
    auto newCommander = commander;
    newCommander.zhili = static_cast<quint8>(zhiliText->toPlainText().toUInt());
    newCommander.wuli = static_cast<quint8>(wuliText->toPlainText().toUInt());
    newCommander.sudu = static_cast<quint8>(suduText->toPlainText().toUInt());
    newCommander.color = static_cast<quint8>(colorText->toPlainText().toUInt(nullptr, 16));
    newCommander.chapter = static_cast<quint8>(chapterText->toPlainText().toUInt(nullptr, 16));
    newCommander.model = static_cast<quint8>(modelText->toPlainText().toUInt(nullptr, 16));
    newCommander.wofangliupai = static_cast<quint8>(wofangliupaiText->toPlainText().toUInt(nullptr, 16));
    newCommander.difangliupai = static_cast<quint8>(difangliupaiText->toPlainText().toUInt(nullptr, 16));
    newCommander.diaobaoliupai = static_cast<quint8>(diaobaoliupaiText->toPlainText().toUInt(nullptr, 16));
    newCommander.face = faceText->toPlainText();
    newCommander.faceControl = static_cast<quint8>(faceControlText->toPlainText().toUInt(nullptr, 16));
    newCommander.chsName = simpliedNameText->toPlainText();
    newCommander.chtName = traditionalNameText->toPlainText();
    newCommander.chtNameControl = static_cast<quint8>(chtNameControlText->toPlainText().toUInt(nullptr, 16));

    quint8 skillRenHuiDang = static_cast<quint8>((static_cast<quint8>(skillRen->isChecked()) << 2) |
            (static_cast<quint8>(skillHui->isChecked()) << 1) |
            (static_cast<quint8>(skillDang->isChecked())));
    newCommander.skillRenHuiDang = skillRenHuiDang;
    quint8 skillBiGongWuZhi = static_cast<quint8>((static_cast<quint8>(skillBi->isChecked()) << 3) |
            (static_cast<quint8>(skillWu->isChecked()) << 2) |
            (static_cast<quint8>(skillGong->isChecked()) << 1) |
            (static_cast<quint8>(skillZhi->isChecked())));
    newCommander.skillBiGongWuZhi = skillBiGongWuZhi;

    quint8 skillFanHunJue = static_cast<quint8>((static_cast<quint8>(skillFan->isChecked()) << 2) |
            (static_cast<quint8>(skillHun->isChecked()) << 1) |
            (static_cast<quint8>(skillJue->isChecked())));
    newCommander.skillFanHunJue = skillFanHunJue;

    quint8 skillFangMouYiLin = static_cast<quint8>((static_cast<quint8>(skillFang->isChecked()) << 3) |
            (static_cast<quint8>(skillMou->isChecked()) << 2) |
            (static_cast<quint8>(skillYi->isChecked()) << 1) |
            (static_cast<quint8>(skillLin->isChecked())));
    newCommander.skillFangMouYiLin = skillFangMouYiLin;

    quint8 skillShiFenTongMing = static_cast<quint8>((static_cast<quint8>(skillShi->isChecked()) << 3) |
            (static_cast<quint8>(skillFen->isChecked()) << 2) |
            (static_cast<quint8>(skillTong->isChecked()) << 1) |
            (static_cast<quint8>(skillMing->isChecked())));
    newCommander.skillShiFenTongMing = skillShiFenTongMing;

    return newCommander.update();
}

void MilitaryCommander::on_saveButton_clicked()
{
    auto index = commanderList->currentRow();

    if (index < 0x00 || index > 0xFF ) {
        qDebug("Invaild rows");
        return;
    }
    auto commander = commanderVector[index];
    commanderVector[index] = updateCommander(commander);
    saveNesFile();
}
