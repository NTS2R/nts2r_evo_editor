#include "militarycommander.h"
#include "mainwindow.h"
#include <QDebug>
#include <QListWidget>
#include <QMessageBox>
MilitaryCommander::MilitaryCommander(QWidget *parent) : QWidget(parent)
{
    this->setupUi(this);
    connect(this->refreshButton, &QPushButton::clicked,
            this, &MilitaryCommander::refreshMiliaryCommanderToListView);
    connect(this->commanderList, &QListWidget::itemSelectionChanged,
            this, &MilitaryCommander::setCurrentItem);
    dataEdit->setReadOnly(true);
}

void MilitaryCommander::refreshMiliaryCommanderToListView() {
    qDebug() << "Refresh";
    this->commanderVector.clear();

    auto& nes = MainWindow::nesFileByteArray;

    for (int index = 0x00; index <= 0xFF; ++index) {
        quint8 low_address = static_cast<quint8>(nes.at(lowIndexAddress + index));
        quint8 high_address = static_cast<quint8>(nes.at(highIndexAdddress + index));
//        qDebug() << QString("low: %1, high: %2").arg(low_address).arg(high_address);
        int commanderAddress = baseAddress + high_address * 0x100 + low_address;

        QByteArray commanderValue;
        for (int i = 0; i < leastLength; ++i) {
            commanderValue.append(nes.at(commanderAddress + i));
        }
        int startIndex = leastLength;
        while (true) {
            auto value = nes.at(commanderAddress + startIndex++);
            commanderValue.append(value);
            if (static_cast<quint8>(value) == static_cast<quint8>(0xFF))
                break;
        }
        QByteArray commanderAnimation;
        commanderAnimation.append(nes.at(attackAnimationAddress + index));
        commanderAnimation.append(nes.at(deadAnimationAddress + index));
        Commander commanderData;
        commanderData.setCommanderAttribute(commanderValue, commanderAnimation, commanderAddress);
        commanderVector.push_back(commanderData);
    }
    setCommanderList();
}

void MilitaryCommander::saveNesFile() {
    auto& nes = MainWindow::nesFileByteArray;

    for (int index = 0x00; index <= 0xFF; ++index) {
        int commanderAddress = commanderVector[index].dataAddress;
        quint8 lowAddress = (commanderAddress - baseAddress) & 0xFF;
        quint8 highAddress = ((commanderAddress - baseAddress) & 0xFF00) >> 16;
        nes[lowIndexAddress + index] = static_cast<char>(lowAddress);
        nes[highIndexAdddress + index] = static_cast<char>(highAddress);
        auto byteArray = commanderVector[index].data;
        for (int i = 0; i < byteArray.length(); i++) {
            nes[commanderAddress + i] = byteArray[i];
        }
        nes[attackAnimationAddress + index] = static_cast<char>(commanderVector[index].attackAnimation);
        nes[deadAnimationAddress + index] = static_cast<char>(commanderVector[index].deadAnimation);
    }
}

void MilitaryCommander::setCommanderList() {
    this->commanderList->clear();
    for (int index = 0x00; index <= 0xFF; ++index) {
        auto commander = commanderVector[index];
        qDebug() << commander.chsName;
        commanderList->addItem(
                    QString("%1 : %2").arg(index, 2, 16, QChar('0')).arg( commander.chsName ).toUpper()
                    );
    }
}

void MilitaryCommander::setSkillCheckBox(const Commander &commander) {
    auto skillRenHuiDang = commander.skillRenHuiDang;
    skillRen->setChecked(skillRenHuiDang & 0b100);
    skillHui->setChecked(skillRenHuiDang & 0b010);
    skillDang->setChecked(skillRenHuiDang & 0b001);
    auto skillBiGongWuZhiShu = commander.skillBiGongWuZhiShu;
    skillBi->setChecked(skillBiGongWuZhiShu & 0b10000);
    skillGong->setChecked(skillBiGongWuZhiShu & 0b01000);
    skillWu->setChecked(skillBiGongWuZhiShu & 0b00100);
    skillZhi->setChecked(skillBiGongWuZhiShu & 0b00010);
    skillShu->setChecked(skillBiGongWuZhiShu & 0b00001);
    auto skillFanHunJue = commander.skillFanHunJue;
    skillFan->setChecked(skillFanHunJue & 0b100);
    skillHun->setChecked(skillFanHunJue & 0b010);
    skillJue->setChecked(skillFanHunJue & 0b001);
    auto skillFangMouLiaoLin = commander.skillFangMouLiaoLin;
    skillFang->setChecked(skillFangMouLiaoLin & 0b1000);
    skillMou->setChecked(skillFangMouLiaoLin & 0b0100);
    skillLiao->setChecked(skillFangMouLiaoLin & 0b0010);
    skillLin->setChecked(skillFangMouLiaoLin & 0b0001);
    auto skillShiFenTongMing = commander.skillShiFenTongMing;
    skillShi->setChecked(skillShiFenTongMing & 0b1000);
    skillFen->setChecked(skillShiFenTongMing & 0b0100);
    skillTong->setChecked(skillShiFenTongMing & 0b0010);
    skillMing->setChecked(skillShiFenTongMing & 0b0001);

    skillQi->setChecked(commander.skillQi & 1);
}

void MilitaryCommander::setCurrentItem() {
    auto index = commanderList->currentRow();
    auto commander = commanderVector[index];
    dataAddressPlainTextEdit->setPlainText(QString("%1").arg(commander.dataAddress, 5, 16).toUpper());
    dataEdit->setText(commander.data.toHex(' ').toUpper());
    //智力
    zhiliText->setText(QString("%1").arg(commander.zhili));
    //武力
    wuliText->setText(QString("%1").arg(commander.wuli));
    //速度
    suduText->setText(QString("%1").arg(commander.sudu));
    //颜色
    colorText->setText(QString("%1").arg(commander.color, 0, 16).toUpper());
    //章节
    chapterText->setText(QString("%1").arg(commander.chapter, 0, 16).toUpper());
    //模型
    modelText->setText(QString("%1").arg(commander.model, 0, 16).toUpper());

    wofangliupaiText->setPlainText(QString("%1").arg(commander.wofangliupai, 2, 16, QChar('0')).toUpper());
    difangliupaiText->setPlainText(QString("%1").arg(commander.difangliupai, 2, 16, QChar('0')).toUpper());
    diaobaoliupaiText->setPlainText(QString("%1").arg(commander.diaobaoliupai, 2, 16, QChar('0')).toUpper());
    faceText->setPlainText(commander.face.toUpper());
    faceControlText->setPlainText(QString("%1").arg(commander.faceControl, 0, 16).toUpper());
    //CHS角色名字 offset25 - 最后
    simpliedNameText->setText(commander.chsName.toUpper());
    //CHT角色名字 offset22 -24
    traditionalNameText->setText(commander.chtName.toUpper());
    chtNameControlText->setPlainText(QString("%1").arg(commander.chtNameControl, 0, 16).toUpper());
    setSkillCheckBox(commander);

    attackAnimationTextEdit->setText(QString("%1").arg(commander.attackAnimation, 2, 16, QChar('0')).toUpper());
    deadAnimationTextEdit->setText(QString("%1").arg(commander.deadAnimation, 2, 16, QChar('0')).toUpper());
}

Commander MilitaryCommander::updateCommander(const Commander &commander) {
    auto newCommander = commander;
    newCommander.dataAddress = dataAddressPlainTextEdit->toPlainText().toInt(nullptr, 16);
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
    newCommander.attackAnimation = static_cast<quint8>(attackAnimationTextEdit->toPlainText().toUInt(nullptr, 16));
    newCommander.deadAnimation = static_cast<quint8>(deadAnimationTextEdit->toPlainText().toUInt(nullptr, 16));

    quint8 skillRenHuiDang = static_cast<quint8>((static_cast<quint8>(skillRen->isChecked()) << 2) |
            (static_cast<quint8>(skillHui->isChecked()) << 1) |
            (static_cast<quint8>(skillDang->isChecked())));
    newCommander.skillRenHuiDang = skillRenHuiDang;
    quint8 skillBiGongWuZhiShu = static_cast<quint8>((static_cast<quint8>(skillBi->isChecked()) << 4) |
            (static_cast<quint8>(skillGong->isChecked()) << 3) |
            (static_cast<quint8>(skillWu->isChecked()) << 2) |
            (static_cast<quint8>(skillZhi->isChecked()) << 1) |
            (static_cast<quint8>(skillShu->isChecked())));
    newCommander.skillBiGongWuZhiShu = skillBiGongWuZhiShu;

    quint8 skillFanHunJue = static_cast<quint8>((static_cast<quint8>(skillFan->isChecked()) << 2) |
            (static_cast<quint8>(skillHun->isChecked()) << 1) |
            (static_cast<quint8>(skillJue->isChecked())));
    newCommander.skillFanHunJue = skillFanHunJue;

    quint8 skillFangMouYiLin = static_cast<quint8>((static_cast<quint8>(skillFang->isChecked()) << 3) |
            (static_cast<quint8>(skillMou->isChecked()) << 2) |
            (static_cast<quint8>(skillLiao->isChecked()) << 1) |
            (static_cast<quint8>(skillLin->isChecked())));
    newCommander.skillFangMouLiaoLin = skillFangMouYiLin;

    quint8 skillShiFenTongMing = static_cast<quint8>((static_cast<quint8>(skillShi->isChecked()) << 3) |
            (static_cast<quint8>(skillFen->isChecked()) << 2) |
            (static_cast<quint8>(skillTong->isChecked()) << 1) |
            (static_cast<quint8>(skillMing->isChecked())));
    newCommander.skillShiFenTongMing = skillShiFenTongMing;
    newCommander.skillQi = static_cast<quint8>(skillQi->isChecked());
    return newCommander.update();
}

void MilitaryCommander::on_saveButton_clicked()
{
    auto index = commanderList->currentRow();

    if (index < 0x00 || index > 0xFF ) {
        qDebug("Invaild rows");
        return;
    }
    auto dataAddress = dataAddressPlainTextEdit->toPlainText().toInt(nullptr, 16);
    if ((dataAddress < baseAddress + 0x8000) || (dataAddress > baseAddress + 0xBFFF)) {
        QMessageBox::warning(nullptr, QString("错误"),
                             QString("人物属性应该在%1-%2的范围内")
                             .arg(baseAddress + 0x8000, 5, 16)
                             .arg(baseAddress + 0xBFFF, 5, 16));
        return;
    }

    auto commander = commanderVector[index];
    commanderVector[index] = updateCommander(commander);
    saveNesFile();
}
