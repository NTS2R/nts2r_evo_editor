#include "militarycommander.h"
#include "mainwindow.h"
#include <QDebug>
#include <QListWidget>
#include <QMessageBox>
#include "xlsxdocument.h"
#include "xlsxchartsheet.h"
#include "xlsxcellrange.h"
#include "xlsxchart.h"
#include "xlsxrichstring.h"
#include "xlsxworkbook.h"
MilitaryCommander::MilitaryCommander(QWidget *parent) : QWidget(parent)
{
    this->setupUi(this);
    connect(this->refreshButton, &QPushButton::clicked,
            this, &MilitaryCommander::refreshMiliaryCommanderToListView);
    connect(this->commanderList, &QListWidget::itemSelectionChanged,
            this, &MilitaryCommander::setCurrentItem);
    dataEdit->setReadOnly(true);

    buttonGroup.addButton(gongCheck, 1);
    buttonGroup.addButton(fangCheck, 2);
    buttonGroup.addButton(mingCheck, 3);
    buttonGroup.addButton(biCheck, 4);
    buttonGroup.setExclusive(true);
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
        QByteArray dajiang;
        dajiang.append(nes[daJiangAdddress + index]);
        dajiang.append(nes[gongAdddress + index]);
        dajiang.append(nes[fangAdddress + index]);
        dajiang.append(nes[mingAdddress + index]);
        dajiang.append(nes[biAdddress + index]);

        // !!! temp action !!!
        commanderData.buhuo = static_cast<quint8>(nes[buhuoAddress + index]);
        commanderData.zhansha = static_cast<quint8>(nes[zhanshaAddress + index]);
        commanderData.attackCount = static_cast<quint8>(nes[attackCountAddress + index]);
        commanderData.celveCount = static_cast<quint8>(nes[celveAddress + index]);
        commanderData.setCommanderAttribute(commanderValue, commanderAnimation, commanderAddress, dajiang);
        commanderVector.push_back(commanderData);
    }
    setCommanderList();
}

void MilitaryCommander::saveNesFile() {
    auto& nes = MainWindow::nesFileByteArray;

    for (int index = 0x00; index <= 0xFF; ++index) {
        int commanderAddress = commanderVector[index].dataAddress;
        quint8 lowAddress = (commanderAddress - baseAddress) & 0xFF;
        quint8 highAddress = ((commanderAddress - baseAddress) & 0xFF00) >> 8;
        nes[lowIndexAddress + index] = static_cast<char>(lowAddress);
        nes[highIndexAdddress + index] = static_cast<char>(highAddress);
        auto byteArray = commanderVector[index].data;
        for (int i = 0; i < byteArray.length(); i++) {
            nes[commanderAddress + i] = byteArray[i];
        }
        nes[attackAnimationAddress + index] = static_cast<char>(commanderVector[index].attackAnimation);
        nes[deadAnimationAddress + index] = static_cast<char>(commanderVector[index].deadAnimation);
        nes[daJiangAdddress + index] = static_cast<char>(commanderVector[index].dajiang);
        nes[gongAdddress + index] = static_cast<char>(commanderVector[index].gong);
        nes[fangAdddress + index] = static_cast<char>(commanderVector[index].fang);
        nes[mingAdddress + index] = static_cast<char>(commanderVector[index].ming);
        nes[biAdddress + index] = static_cast<char>(commanderVector[index].bi);
        nes[buhuoAddress + index] = static_cast<char>(commanderVector[index].buhuo);
        nes[zhanshaAddress + index] = static_cast<char>(commanderVector[index].zhansha);
        nes[attackCountAddress + index] = static_cast<char>(commanderVector[index].attackCount);
        nes[celveAddress + index] = static_cast<char>(commanderVector[index].celveCount);
    }
}

QString MilitaryCommander::getChsName(QString chsName) {
    auto chsNameList = chsName.split(" ");
    QList<int> nameList;
    for (auto chsNameChar: chsNameList) {
        nameList.append(chsNameChar.toInt(nullptr, 16));
    }
    QString name;
    for (int i = 0; i < nameList.size(); ++i) {
        if (nameList[i] == 0xFF) {
            break;
        }
        if (nameList[i] >= 0xB0) {
            continue;
        }
        int j = i;
        while (nameList[--j] < 0xB0);
        auto chsChar = MainWindow::chsNameLibrary[nameList[j] - 0xB0][nameList[i]];
        qDebug() << chsChar;
        name.append(chsChar);
    }
    return name;
}

QString MilitaryCommander::getChtName(QString chtName, quint8 chtNameControl) {
    auto chtNameList = chtName.split(" ");
    QString name;
    for (int i = 0; i < 3; ++i) {
        auto index = chtNameList[i].toUInt(nullptr, 16);
        if (index == 0xFF) {
            break;
        }
        auto areaIndex = ((1 << (2 - i)) & chtNameControl) > 0;
        auto chtChar = MainWindow::chtNameLibrary[areaIndex][index];
        name.append(chtChar);
    }
    return name;
}

void MilitaryCommander::setCommanderList() {
    this->commanderList->clear();
    for (int index = 0x00; index <= 0xFF; ++index) {
        auto commander = commanderVector[index];
        qDebug() << commander.chsName;
        QString name;
        name.append(getChsName(commander.chsName));
        name.append("/");
        name.append(getChtName(commander.chtName, commander.chtNameControl));

        commanderList->addItem(
                    QString("%1 : %2").arg(index, 2, 16, QChar('0')).arg( name ).toUpper()
                    );
    }
}

void MilitaryCommander::setSkillCheckBox(const Commander &commander) {
    auto skillRenHuiDang = commander.skillRenHuiDang;
    skillRen->setChecked(skillRenHuiDang & 0b10000000);
    skillHui->setChecked(skillRenHuiDang & 0b01000000);
    skillDang->setChecked(skillRenHuiDang & 0b00100000);
    auto skillBiGongWuZhiShu = commander.skillBiGongWuZhiShu;
    skillBi->setChecked(skillBiGongWuZhiShu & 0b10000000);
    skillGong->setChecked(skillBiGongWuZhiShu & 0b01000000);
    skillWu->setChecked(skillBiGongWuZhiShu & 0b00100000);
    skillZhi->setChecked(skillBiGongWuZhiShu & 0b00010000);
    skillShu->setChecked(skillBiGongWuZhiShu & 0b00001000);
    auto skillFanHunJue = commander.skillFanHunJue;
    skillFan->setChecked(skillFanHunJue & 0b10000000);
    skillHun->setChecked(skillFanHunJue & 0b01000000);
    skillJue->setChecked(skillFanHunJue & 0b00100000);
    auto skillFangMouLiaoLin = commander.skillFangMouLiaoLin;
    skillFang->setChecked(skillFangMouLiaoLin & 0b10000000);
    skillMou->setChecked(skillFangMouLiaoLin & 0b01000000);
    skillLiao->setChecked(skillFangMouLiaoLin & 0b00100000);
    skillLin->setChecked(skillFangMouLiaoLin & 0b00010000);
    auto skillShiFenTongMing = commander.skillShiFenTongMing;
    skillShi->setChecked(skillShiFenTongMing & 0b00001000);
    skillFen->setChecked(skillShiFenTongMing & 0b00000100);
    skillTong->setChecked(skillShiFenTongMing & 0b00000010);
    skillMing->setChecked(skillShiFenTongMing & 0b00000001);

    skillQi->setChecked(commander.skillQi & 0b10000000);

    auto dajiang = commander.dajiang;
    gongCheck->setChecked(dajiang & 0b10000000);
    fangCheck->setChecked(dajiang & 0b01000000);
    mingCheck->setChecked(dajiang & 0b00100000);
    biCheck->setChecked(dajiang & 0b00010000);
}

void MilitaryCommander::setCurrentItem() {
    auto index = commanderList->currentRow();
    auto commander = commanderVector[index];
    dataAddressSpinBox->setValue(commander.dataAddress);
    dataEdit->setText(commander.data.toHex(' ').toUpper());
    //智力
    zhiliSpinBox->setValue(commander.zhili);
    //武力
    wuliSpinBox->setValue(commander.wuli);
    //速度
    suduSpinBox->setValue(commander.sudu);
    //颜色
    colorSpinBox->setValue(commander.color);
    //章节
    chapterSpinBox->setValue(commander.chapter);
    //模型
    modelSpinBox->setValue(commander.model);
    dixingComboBox->setCurrentIndex(commander.dixing);
    jiceSpinBox->setValue(commander.jice);
    gongjiliSpinBox->setValue(commander.gongjili);
    fangyuliSpinBox->setValue(commander.fangyuli);
    wofangliupaiSpinBox->setValue(commander.wofangliupai);
    difangliupaiSpinBox->setValue(commander.difangliupai);
    diaobaoliupaiSpinBox->setValue(commander.diaobaoliupai);
    weaponComboBox->setCurrentIndex(commander.weapon);
    faceText->setText(commander.face.toUpper());
    faceControlSpinBox->setValue(commander.faceControl);
    moulvezhiSpinBox->setValue(commander.moulvezhi);
    jimouSpinBox->setValue(commander.jimou);

    //CHS角色名字 offset25 - 最后
    simpliedNameText->setText(commander.chsName.toUpper());
    //CHT角色名字 offset22 -24
    traditionalNameText->setText(commander.chtName.toUpper());
    chtNameControlSpinBox->setValue(commander.chtNameControl);
    setSkillCheckBox(commander);
    gongSpinBox->setValue(commander.gong);
    fangSpinBox->setValue(commander.fang);
    mingSpinBox->setValue(commander.ming);
    biSpinBox->setValue(commander.bi);

    attackAnimationSpinBox->setValue(commander.attackAnimation);
    deadAnimationSpinBox->setValue(commander.deadAnimation);
    buhuoIndexSpinBox->setValue(commander.buhuo);
    zhanshaIndexSpinBox->setValue(commander.zhansha);
    attackCountSpinBox->setValue(commander.attackCount);
    celveCountSpinBox->setValue(commander.celveCount);
}

Commander MilitaryCommander::updateCommander(const Commander &commander) {
    auto newCommander = commander;
    newCommander.dataAddress = dataAddressSpinBox->value();
    newCommander.zhili = static_cast<quint8>(zhiliSpinBox->value());
    newCommander.wuli = static_cast<quint8>(wuliSpinBox->value());
    newCommander.sudu = static_cast<quint8>(suduSpinBox->value());
    newCommander.color = static_cast<quint8>(colorSpinBox->value());
    newCommander.chapter = static_cast<quint8>(chapterSpinBox->value());
    newCommander.model = static_cast<quint8>(modelSpinBox->value());
    newCommander.jice = static_cast<quint8>(jiceSpinBox->value());
    newCommander.dixing = static_cast<quint8>(dixingComboBox->currentIndex());
    newCommander.wofangliupai = static_cast<quint8>(wofangliupaiSpinBox->value());
    newCommander.difangliupai = static_cast<quint8>(difangliupaiSpinBox->value());
    newCommander.diaobaoliupai = static_cast<quint8>(diaobaoliupaiSpinBox->value());
    newCommander.weapon = static_cast<quint8>(weaponComboBox->currentIndex());

    newCommander.face = faceText->text();
    newCommander.faceControl = static_cast<quint8>(faceControlSpinBox->value());
    newCommander.chsName = simpliedNameText->toPlainText();
    newCommander.chtName = traditionalNameText->text();
    newCommander.chtNameControl = static_cast<quint8>(chtNameControlSpinBox->value());
    newCommander.attackAnimation = static_cast<quint8>(attackAnimationSpinBox->value());
    newCommander.deadAnimation = static_cast<quint8>(deadAnimationSpinBox->value());
    newCommander.buhuo = static_cast<quint8>(buhuoIndexSpinBox->value());
    newCommander.zhansha = static_cast<quint8>(zhanshaIndexSpinBox->value());
    newCommander.attackCount = static_cast<quint8>(attackCountSpinBox->value());
    newCommander.celveCount = static_cast<quint8>(celveCountSpinBox->value());

    newCommander.gongjili = static_cast<quint8>(gongjiliSpinBox->value());
    newCommander.fangyuli = static_cast<quint8>(fangyuliSpinBox->value());
    newCommander.moulvezhi = static_cast<quint8>(moulvezhiSpinBox->value());
    newCommander.jimou = static_cast<quint8>(jimouSpinBox->value());

    quint8 dajiang = static_cast<quint8>((static_cast<quint8>(gongCheck->isChecked()) << 7) |
            (static_cast<quint8>(fangCheck->isChecked()) << 6) |
            (static_cast<quint8>(mingCheck->isChecked()) << 5) |
            (static_cast<quint8>(biCheck->isChecked())) << 4);
    newCommander.dajiang = (newCommander.dajiang & 0x0f) | dajiang;
    newCommander.gong = static_cast<quint8>(gongSpinBox->value());
    newCommander.fang = static_cast<quint8>(fangSpinBox->value());
    newCommander.ming = static_cast<quint8>(mingSpinBox->value());
    newCommander.bi = static_cast<quint8>(biSpinBox->value());
    quint8 skillRenHuiDang = static_cast<quint8>(
                (static_cast<quint8>(skillRen->isChecked()) << 7) |
                (static_cast<quint8>(skillHui->isChecked()) << 6) |
                (static_cast<quint8>(skillDang->isChecked()) << 5));
    newCommander.skillRenHuiDang = skillRenHuiDang;
    quint8 skillBiGongWuZhiShu = static_cast<quint8>(
                (static_cast<quint8>(skillBi->isChecked()) << 7) |
                (static_cast<quint8>(skillGong->isChecked()) << 6) |
                (static_cast<quint8>(skillWu->isChecked()) << 5) |
                (static_cast<quint8>(skillZhi->isChecked()) << 4) |
                (static_cast<quint8>(skillShu->isChecked()) << 3)
                );
    newCommander.skillBiGongWuZhiShu = skillBiGongWuZhiShu;

    quint8 skillFanHunJue = static_cast<quint8>(
                (static_cast<quint8>(skillFan->isChecked()) << 7) |
                (static_cast<quint8>(skillHun->isChecked()) << 6) |
                (static_cast<quint8>(skillJue->isChecked()) << 5)
                );
    newCommander.skillFanHunJue = skillFanHunJue;

    quint8 skillFangMouYiLin = static_cast<quint8>(
                (static_cast<quint8>(skillFang->isChecked()) << 7) |
                (static_cast<quint8>(skillMou->isChecked()) << 6) |
                (static_cast<quint8>(skillLiao->isChecked()) << 5) |
                (static_cast<quint8>(skillLin->isChecked()) << 4)
                );
    newCommander.skillFangMouLiaoLin = skillFangMouYiLin;

    quint8 skillShiFenTongMing = static_cast<quint8>(
                (static_cast<quint8>(skillShi->isChecked()) << 3) |
                (static_cast<quint8>(skillFen->isChecked()) << 2) |
                (static_cast<quint8>(skillTong->isChecked()) << 1) |
                (static_cast<quint8>(skillMing->isChecked()))
                );
    newCommander.skillShiFenTongMing = skillShiFenTongMing;
    newCommander.skillQi = static_cast<quint8>(
                static_cast<quint8>(skillQi->isChecked()) << 7);
    return newCommander.update();
}

void MilitaryCommander::on_saveButton_clicked()
{
    auto index = commanderList->currentRow();

    if (index < 0x00 || index > 0xFF ) {
        qDebug("Invaild rows");
        return;
    }
    auto dataAddress = dataAddressSpinBox->value();
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

void MilitaryCommander::exportMilitary(QString fileName) {
    QXlsx::Document xlsx;
    int excelOffest = 2;
    if(!xlsx.selectSheet(tr("武将"))) {
        xlsx.addSheet(tr("武将"));
        xlsx.selectSheet(tr("武将"));
    }
    xlsx.write(1, 1, tr("番号"));
    xlsx.write(1, 2, tr("简体中文名字"));
    xlsx.write(1, 3, tr("战斗中文名字"));
    xlsx.write(1, 4, tr("合成等级"));
    xlsx.write(1, 5, tr("是否可合成"));
    xlsx.write(1, 6, tr("是否可做合成素材"));

    xlsx.write(1, 7, tr("登场章节"));
    xlsx.write(1, 8, tr("数据地址"));
    xlsx.write(1, 9, tr("武力"));
    xlsx.write(1, 10, tr("智力"));
    xlsx.write(1, 11, tr("速度"));
    xlsx.write(1, 12, tr("武器"));
    xlsx.write(1, 13, tr("地形"));
    auto& nes = MainWindow::nesFileByteArray;
    for (int index = 0x00; index <= 0xFF; ++index) {
        int excelIndex = index + excelOffest;
        xlsx.write(excelIndex, 1, QString("0x%1").arg(index, 2, 16, QChar('0')).toUpper());
        xlsx.write(excelIndex, 2, getChsName(commanderVector[index].chsName));
        xlsx.write(excelIndex, 3, getChtName(commanderVector[index].chtName, commanderVector[index].chtNameControl));
        xlsx.write(excelIndex, 4, static_cast<quint8>(nes.at(mergeAddress + index)));
        xlsx.write(excelIndex, 7, commanderVector[index].chapter);
        xlsx.write(excelIndex, 8, QString("0x%1").arg(commanderVector[index].dataAddress, 2, 16, QChar('0')).toUpper());
        xlsx.write(excelIndex, 9, commanderVector[index].wuli);
        xlsx.write(excelIndex, 10, commanderVector[index].zhili);
        xlsx.write(excelIndex, 11, commanderVector[index].sudu);
        auto weaponString = QString("%1").arg(commanderVector[index].weapon, 2, 16, QChar('0'));
        qDebug() << weaponString;
        xlsx.write(excelIndex, 12, MainWindow::weaponName[weaponString]);
        auto dixingString = QString("%1").arg(commanderVector[index].dixing, 2, 16, QChar('0')).toUpper();
        qDebug() << dixingString;
        xlsx.write(excelIndex, 13, MainWindow::dixingName[dixingString]);
    }
    for (int index = 0x00; index <= 0x7F; ++index) {
        auto notToObjectIndex = static_cast<quint8>(nes.at(notCompositeToObjetcstartAddress + index));
        int notToObjectExcelIndex = notToObjectIndex + excelOffest;
        xlsx.write(notToObjectExcelIndex, 5, tr("否"));
        auto notAsObjectIndex = static_cast<quint8>(nes.at(notCompositeAsObjetcstartAddress + index));
        int notAsObjectExcelIndex = notAsObjectIndex + excelOffest;
        xlsx.write(notAsObjectExcelIndex, 6, tr("否"));
    }
    if(!xlsx.selectSheet(tr("合成表"))) {
        xlsx.addSheet(tr("合成表"));
        xlsx.selectSheet(tr("合成表"));
    }
    for (int index = 0x00; index <= 0xFF; ++index) {
        QString name;
        name.append(
                    QString("%1 %2/%3")
                    .arg(index, 2, 16, QChar('0')).toUpper()
                    .arg(getChsName(commanderVector[index].chsName))
                    .arg(getChtName(commanderVector[index].chtName, commanderVector[index].chtNameControl))
                    );
        int offestIndex = index + excelOffest;
        xlsx.write(offestIndex, 1, name);
        xlsx.write(1, offestIndex, name);
    }
    for (int i = 0x00; i <= 0xFF; ++i) {
        for (int j = 0x00; j <= 0xFF; ++j) {
            if (i == j) {
                continue;
            }
            xlsx.selectSheet(tr("武将"));
            if (xlsx.read(i + excelOffest, 6).toString() == tr("否")) {
                continue;
            }
            if (xlsx.read(j + excelOffest, 6).toString() == tr("否")) {
                continue;
            }
            int result = (i & 0xAA) | (j & 0x55);
            qDebug() << "result:" << result;
            if (xlsx.read(result + excelOffest, 5).toString() == tr("否")) {
                continue;
            }
            xlsx.selectSheet(tr("合成表"));
            QString name;
            name.append(
                        QString("0x%1 %2/%3")
                        .arg(result, 2, 16, QChar('0')).toUpper()
                        .arg(getChsName(commanderVector[result].chsName))
                        .arg(getChtName(commanderVector[result].chtName, commanderVector[result].chtNameControl))
                        );
            xlsx.write(i + excelOffest, j + excelOffest, name);
        }
    }
    QFile file{fileName};
    if (file.exists()) {
        file.remove();
    }

    xlsx.saveAs(fileName);
}
