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
    connect(this->refreshNameShowButton, &QPushButton::clicked,
            this, &MilitaryCommander::refreshName);
}

void MilitaryCommander::refreshName() {
    auto chsName = simpliedNameText->toPlainText();
    chsNameLabel->setText(getChsName(chsName));
    QString chtName;
    chtName.append(QString("%1").arg(chtNameSpinBox1->value(), 2, 16, QChar('0')).toUpper());
    chtName.append(" ");
    chtName.append(QString("%1").arg(chtNameSpinBox2->value(), 2, 16, QChar('0')).toUpper());
    chtName.append(" ");
    chtName.append(QString("%1").arg(chtNameSpinBox3->value(), 2, 16, QChar('0')).toUpper());
    auto chtNameControl = static_cast<quint8>(chtNameControlSpinBox->value());
    chtNameLabel->setText(getChtName(chtName, chtNameControl));
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
//        qDebug() << chsChar;
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
        QString name{getChsName(commander.chsName)};
        commanderList->addItem(
                    QString("%1: %2").arg(index, 2, 16, QChar('0')).arg( name ).toUpper()
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

    faceSpinBox1->setValue(commander.face[0]);
    faceSpinBox2->setValue(commander.face[1]);
    faceSpinBox3->setValue(commander.face[2]);
    faceSpinBox4->setValue(commander.face[3]);
    faceSpinBox5->setValue(commander.face[4]);
    faceSpinBox6->setValue(commander.face[5]);
    faceControlSpinBox->setValue(commander.faceControl);
    moulvezhiSpinBox->setValue(commander.moulvezhi);
    jimouSpinBox->setValue(commander.jimou);

    //CHS角色名字 offset25 - 最后
    simpliedNameText->setText(commander.chsName.toUpper());
    chsNameLabel->setText(getChsName(commander.chsName));
    //CHT角色名字 offset22 -24
    auto chtNameList = commander.chtName.toUpper().split(' ');
    chtNameSpinBox1->setValue(static_cast<int>(chtNameList[0].toUInt(nullptr, 16)));
    chtNameSpinBox2->setValue(static_cast<int>(chtNameList[1].toUInt(nullptr, 16)));
    chtNameSpinBox3->setValue(static_cast<int>(chtNameList[2].toUInt(nullptr, 16)));
    chtNameControlSpinBox->setValue(commander.chtNameControl);
    chtNameLabel->setText(getChtName(commander.chtName, commander.chtNameControl));

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

    newCommander.face[0] = static_cast<quint8>(faceSpinBox1->value());
    newCommander.face[1] = static_cast<quint8>(faceSpinBox2->value());
    newCommander.face[2] = static_cast<quint8>(faceSpinBox3->value());
    newCommander.face[3] = static_cast<quint8>(faceSpinBox4->value());
    newCommander.face[4] = static_cast<quint8>(faceSpinBox5->value());
    newCommander.face[5] = static_cast<quint8>(faceSpinBox6->value());

    newCommander.faceControl = static_cast<quint8>(
                faceControlSpinBox->value()
                );
    newCommander.chsName = simpliedNameText->toPlainText();
    QString chtName;
    chtName.append(QString("%1").arg(chtNameSpinBox1->value(), 2, 16, QChar('0')).toUpper());
    chtName.append(" ");
    chtName.append(QString("%1").arg(chtNameSpinBox2->value(), 2, 16, QChar('0')).toUpper());
    chtName.append(" ");
    chtName.append(QString("%1").arg(chtNameSpinBox3->value(), 2, 16, QChar('0')).toUpper());
    newCommander.chtName = chtName;
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
    xlsx.write(1, 14, tr("大将"));
    xlsx.write(1, 15, tr("加成百分比%"));

    xlsx.write(1, 16, tr("仁"));
    xlsx.write(1, 17, tr("慧"));
    xlsx.write(1, 18, tr("挡"));
    xlsx.write(1, 19, tr("奇"));
    xlsx.write(1, 20, tr("避"));
    xlsx.write(1, 21, tr("攻"));
    xlsx.write(1, 22, tr("武"));
    xlsx.write(1, 23, tr("智"));
    xlsx.write(1, 24, tr("术"));
    xlsx.write(1, 25, tr("返"));
    xlsx.write(1, 26, tr("魂"));
    xlsx.write(1, 27, tr("觉"));
    xlsx.write(1, 28, tr("防"));
    xlsx.write(1, 29, tr("谋"));
    xlsx.write(1, 30, tr("疗"));
    xlsx.write(1, 31, tr("临"));
    xlsx.write(1, 32, tr("识"));
    xlsx.write(1, 33, tr("奋"));
    xlsx.write(1, 34, tr("统"));
    xlsx.write(1, 35, tr("命"));
    xlsx.write(1, 36, tr("攻击次数"));
    xlsx.write(1, 37, tr("策略次数"));
    xlsx.write(1, 38, tr("攻击倍率"));

    QXlsx::Format firstFormat;
    firstFormat.setFontBold(true);
    firstFormat.setHorizontalAlignment(QXlsx::Format::AlignHCenter);
    xlsx.setRowFormat(1, firstFormat);

    QXlsx::Format excelNumberFormat;
    excelNumberFormat.setNumberFormat("#0");

    QXlsx::Format extraNumberFormat;
    extraNumberFormat.setNumberFormat("#0.0000");

    quint8 judgeValue = 230u;
    quint32 judgeAccValue = 615u;

    auto& nes = MainWindow::nesFileByteArray;
    for (int index = 0x00; index <= 0xFF; ++index) {
        int excelIndex = index + excelOffest;
        xlsx.write(excelIndex, 1, QString("0x%1").arg(index, 2, 16, QChar('0')).toUpper());
        xlsx.write(excelIndex, 2, getChsName(commanderVector[index].chsName));
        xlsx.write(excelIndex, 3, getChtName(commanderVector[index].chtName, commanderVector[index].chtNameControl));
        xlsx.write(excelIndex, 4, static_cast<quint8>(nes.at(mergeAddress + index)));
        xlsx.write(excelIndex, 7, commanderVector[index].chapter);
        xlsx.write(excelIndex, 8, QString("0x%1").arg(commanderVector[index].dataAddress, 2, 16, QChar('0')).toUpper());
        xlsx.write(excelIndex, 9, commanderVector[index].wuli, excelNumberFormat);
        xlsx.write(excelIndex, 10, commanderVector[index].zhili, excelNumberFormat);
        xlsx.write(excelIndex, 11, commanderVector[index].sudu, excelNumberFormat);
        auto weaponString = QString("%1").arg(commanderVector[index].weapon, 2, 16, QChar('0'));
        qDebug() << weaponString;
        xlsx.write(excelIndex, 12, MainWindow::weaponName[weaponString]);
        auto dixingString = QString("%1").arg(commanderVector[index].dixing, 2, 16, QChar('0')).toUpper();
        qDebug() << dixingString;
        xlsx.write(excelIndex, 13, MainWindow::dixingName[dixingString]);

        auto dajiang = commanderVector[index].dajiang & 0xF0;

        QMap<int, QPair<QString, quint8>> dajiangName {
            {0b10000000, {tr("攻"), commanderVector[index].gong}},
            {0b01000000, {tr("防"), commanderVector[index].fang}},
            {0b00100000, {tr("命"), commanderVector[index].ming}},
            {0b00010000, {tr("避"), commanderVector[index].bi}}
        };

        if (dajiangName.find(dajiang) != dajiangName.end()) {
            xlsx.write(excelIndex, 14, dajiangName[dajiang].first);
            xlsx.write(excelIndex, 15, dajiangName[dajiang].second / 255.0 * 100, extraNumberFormat);
//            xlsx.write(excelIndex, 15, dajiangName[dajiang].second);
        }

        auto renHuiDang = commanderVector[index].skillRenHuiDang;
        if (renHuiDang & 0b10000000) {
            xlsx.write(excelIndex, 16, tr("仁"));
        }
        if (renHuiDang & 0b01000000) {
            xlsx.write(excelIndex, 17, tr("慧"));
        }
        if (renHuiDang & 0b00100000) {
            xlsx.write(excelIndex, 18, tr("挡"));
        }

        if (commanderVector[index].skillQi) {
            xlsx.write(excelIndex, 19, tr("奇"));
        }

        auto skillBiGongWuZhiShu = commanderVector[index].skillBiGongWuZhiShu;

        if (skillBiGongWuZhiShu & 0b10000000) {
            xlsx.write(excelIndex, 20, tr("避"));
        }
        if (skillBiGongWuZhiShu & 0b01000000) {
            xlsx.write(excelIndex, 21, tr("攻"));
        }
        if (skillBiGongWuZhiShu & 0b00100000) {
            xlsx.write(excelIndex, 22, tr("武"));
        }
        if (skillBiGongWuZhiShu & 0b00010000) {
            xlsx.write(excelIndex, 23, tr("智"));
        }
        if (skillBiGongWuZhiShu & 0b00001000) {
            xlsx.write(excelIndex, 24, tr("术"));
        }

        auto skillFanHunJue = commanderVector[index].skillFanHunJue;
        if (skillFanHunJue & 0b10000000) {
            xlsx.write(excelIndex, 25, tr("返"));
        }
        if (skillFanHunJue & 0b01000000) {
            xlsx.write(excelIndex, 26, tr("魂"));
        }
        if (skillFanHunJue & 0b00100000) {
            xlsx.write(excelIndex, 27, tr("觉"));
        }

        auto skillFangMouLiaoLin = commanderVector[index].skillFangMouLiaoLin;

        if (skillFangMouLiaoLin & 0b10000000) {
            xlsx.write(excelIndex, 28, tr("防"));
        }
        if (skillFangMouLiaoLin & 0b01000000) {
            xlsx.write(excelIndex, 29, tr("谋"));
        }
        if (skillFangMouLiaoLin & 0b00100000) {
            xlsx.write(excelIndex, 30, tr("疗"));
        }
        if (skillFangMouLiaoLin & 0b00010000) {
            xlsx.write(excelIndex, 31, tr("临"));
        }

        auto skillShiFenTongMing = commanderVector[index].skillShiFenTongMing;
        if (skillShiFenTongMing & 0b00001000) {
            xlsx.write(excelIndex, 32, tr("识"));
        }
        if (skillShiFenTongMing & 0b00000100) {
            xlsx.write(excelIndex, 33, tr("奋"));
        }
        if (skillShiFenTongMing & 0b00000010) {
            xlsx.write(excelIndex, 34, tr("统"));
        }
        if (skillShiFenTongMing & 0b00000001) {
            xlsx.write(excelIndex, 35, tr("命"));
        }

        xlsx.write(excelIndex, 36, commanderVector[index].attackCount, excelNumberFormat);
        xlsx.write(excelIndex, 37, commanderVector[index].celveCount, excelNumberFormat);


//        武力222  156  83
//        智力200  200  169
//        速度222  111  140
//        三围160  191  124
        if ((0ul
             + commanderVector[index].wuli
             + commanderVector[index].zhili
             + commanderVector[index].sudu) >= judgeAccValue) {
            QXlsx::Format excelColorNumberFormat = excelNumberFormat;
            // 三围160  191  124
            excelColorNumberFormat.setPatternBackgroundColor(QColor{160, 191, 124});
            xlsx.write(excelIndex, 9, commanderVector[index].wuli, excelColorNumberFormat);
            xlsx.write(excelIndex, 10, commanderVector[index].zhili, excelColorNumberFormat);
            xlsx.write(excelIndex, 11, commanderVector[index].sudu, excelColorNumberFormat);
        }

        if (commanderVector[index].wuli >= judgeValue) {
            QXlsx::Format excelColorNumberFormat = excelNumberFormat;
            // 武力222  156  83
            excelColorNumberFormat.setPatternBackgroundColor(QColor{222, 156, 83});
            xlsx.write(excelIndex, 9, commanderVector[index].wuli, excelColorNumberFormat);
        }

        if (commanderVector[index].zhili >= judgeValue) {
            QXlsx::Format excelColorNumberFormat = excelNumberFormat;
            // 智力200  200  169
            excelColorNumberFormat.setPatternBackgroundColor(QColor{200, 200, 169});
            xlsx.write(excelIndex, 10, commanderVector[index].zhili, excelColorNumberFormat);
        }

        if (commanderVector[index].sudu >= judgeValue) {
            QXlsx::Format excelColorNumberFormat = excelNumberFormat;
            // 速度222  111  140
            excelColorNumberFormat.setPatternBackgroundColor(QColor{222, 156, 83});
            xlsx.write(excelIndex, 11, commanderVector[index].sudu, excelColorNumberFormat);
        }

        auto limitLow = static_cast<quint16>(nes.at(militrayLimitLow + index));
        auto limitHigh = static_cast<quint16>(nes.at(militrayLimitHigh + index));
        double limit = limitHigh + limitLow / 255.0;
        xlsx.write(excelIndex, 38, limit, extraNumberFormat);
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
                    QString("%1 %2")
                    .arg(index, 2, 16, QChar('0')).toUpper()
                    .arg(getChsName(commanderVector[index].chsName))
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
//            qDebug() << "result:" << result;
            if (xlsx.read(result + excelOffest, 5).toString() == tr("否")) {
                continue;
            }
            xlsx.selectSheet(tr("合成表"));
            QString name;
            name.append(
                        QString("0x%1 %2")
                        .arg(result, 2, 16, QChar('0')).toUpper()
                        .arg(getChsName(commanderVector[result].chsName))
                        );
            QXlsx::Format excelColorNumberFormat = excelNumberFormat;
            // 三围160  191  124
            excelColorNumberFormat.setPatternBackgroundColor(QColor{160, 191, 124});
            if (((0ul
                 + commanderVector[result].wuli
                 + commanderVector[result].zhili
                 + commanderVector[result].sudu) >= judgeAccValue) |
                    (commanderVector[result].wuli >= judgeValue) |
                    (commanderVector[result].zhili >= judgeValue) |
                    (commanderVector[result].sudu >= judgeValue)
                    ) {
                QXlsx::Format excelColorNumberFormat = excelNumberFormat;
                // 三围160  191  124
                excelColorNumberFormat.setPatternBackgroundColor(QColor{160, 191, 124});
                xlsx.write(i + excelOffest, j + excelOffest, name, excelColorNumberFormat);
            } else
                xlsx.write(i + excelOffest, j + excelOffest, name);
        }
    }

    if(!xlsx.selectSheet(tr("特殊"))) {
        xlsx.addSheet(tr("特殊"));
        xlsx.selectSheet(tr("特殊"));
    }
    excelOffest = 2;
    xlsx.write(1, 1, tr("特殊系无效剧情战设置 18个"));
    for (int i = 0; i < 18; ++i) {
        xlsx.write(1, i+excelOffest,
                   QString("0x%1").arg(
                       static_cast<quint8>(nes.at(specialInvaild + i)),
                       2,
                       16,
                       QChar('0')).toUpper());
    }
    xlsx.write(2, 1, tr("回复以外策略无效剧情战设置 13个"));
    for (int i = 0; i < 13; ++i) {
        xlsx.write(2, i+excelOffest,
                   QString("0x%1").arg(
                       static_cast<quint8>(nes.at(restoreInvaild + i)),
                       2,
                       16,
                       QChar('0')).toUpper());
    }

    xlsx.write(3, 1, tr("成功率减半计策设置 5个"));
    for (int i = 0; i < 5; ++i) {
        xlsx.write(3, i+excelOffest,
                   QString("0x%1").arg(
                       static_cast<quint8>(nes.at(successfulPercentInhalf + i)),
                       2,
                       16,
                       QChar('0')).toUpper());
    }

    QFile file{fileName};
    if (file.exists()) {
        file.remove();
    }

    xlsx.saveAs(fileName);
}

void MilitaryCommander::importMilitary(QString filename) {
    QXlsx::Document xlsx{filename};
    int excelOffest = 2;
    if(!xlsx.selectSheet(tr("武将"))) {
        QMessageBox::warning(nullptr, tr("警告"), tr("没有武将表"));
        return;
    }
    auto& nes = MainWindow::nesFileByteArray;
    for (int index = 0x00; index <= 0xFF; ++index) {
        int excelIndex = index + excelOffest;
        nes.data()[mergeAddress + index] = static_cast<char>(xlsx.read(excelIndex, 4).toUInt());
        commanderVector[index].chapter = static_cast<quint8>(xlsx.read(excelIndex, 7).toUInt());
        commanderVector[index].wuli = static_cast<quint8>(xlsx.read(excelIndex, 9).toUInt());
        commanderVector[index].zhili = static_cast<quint8>(xlsx.read(excelIndex, 10).toUInt());
        commanderVector[index].sudu = static_cast<quint8>(xlsx.read(excelIndex, 11).toUInt());
        auto weaponString = xlsx.read(excelIndex, 12).toString();
        for (auto p = MainWindow::weaponName.begin(); p != MainWindow::weaponName.end(); ++p) {
            if(p.value() == weaponString) {
                commanderVector[index].weapon = static_cast<quint8>(p.key().toUInt(nullptr, 16));
                break;
            }
        }
        auto dixingString = xlsx.read(excelIndex, 13).toString();
        for (auto p = MainWindow::dixingName.begin(); p != MainWindow::dixingName.end(); ++p) {
            if(p.value() == dixingString) {
                commanderVector[index].dixing = static_cast<quint8>(p.key().toUInt(nullptr, 16));
                break;
            }
        }
        commanderVector[index].attackCount = static_cast<quint8>(xlsx.read(excelIndex, 36).toUInt());
        commanderVector[index].celveCount = static_cast<quint8>(xlsx.read(excelIndex, 37).toUInt());
        auto limit = static_cast<quint16>(xlsx.read(excelIndex, 38).toDouble() * 100);
        nes.data()[militrayLimitLow + index] = static_cast<char>(static_cast<quint8>(limit & 0xFF));
        nes.data()[militrayLimitHigh + index] = static_cast<char>(static_cast<quint8>((limit >> 8) & 0xFF));
        QMap<QString, quint8*> dajiangName {
            {tr("攻"), &commanderVector[index].gong},
            {tr("防"), &commanderVector[index].fang},
            {tr("命"), &commanderVector[index].ming},
            {tr("避"), &commanderVector[index].bi}
        };
        auto dajiang = xlsx.read(excelIndex, 14).toString();

        for (auto p = dajiangName.begin(); p != dajiangName.end(); ++p) {
            if(p.key() == dajiang) {
                auto dajiangValue = xlsx.read(excelIndex, 15).toDouble() * 100;
                *p.value() = static_cast<quint8>(dajiangValue);
            }
        }
        if (xlsx.read(excelIndex, 16).toString() == tr("仁")) {
            commanderVector[index].skillRenHuiDang |= 0b10000000;
        }
        if (xlsx.read(excelIndex, 17).toString() == tr("慧")) {
            commanderVector[index].skillRenHuiDang |= 0b01000000;
        }
        if (xlsx.read(excelIndex, 18).toString() == tr("挡")) {
            commanderVector[index].skillRenHuiDang |= 0b00100000;
        }
        if (xlsx.read(excelIndex, 19).toString() == tr("奇")) {
            commanderVector[index].skillQi |= 0b10000000;
        }

        if (xlsx.read(excelIndex, 20).toString() == tr("避")) {
            commanderVector[index].skillBiGongWuZhiShu |= 0b10000000;
        }
        if (xlsx.read(excelIndex, 21).toString() == tr("攻")) {
            commanderVector[index].skillBiGongWuZhiShu |= 0b01000000;
        }
        if (xlsx.read(excelIndex, 22).toString() == tr("武")) {
            commanderVector[index].skillBiGongWuZhiShu |= 0b00100000;
        }
        if (xlsx.read(excelIndex, 23).toString() == tr("智")) {
            commanderVector[index].skillBiGongWuZhiShu |= 0b00010000;
        }
        if (xlsx.read(excelIndex, 24).toString() == tr("术")) {
            commanderVector[index].skillBiGongWuZhiShu |= 0b00001000;
        }

        if (xlsx.read(excelIndex, 25).toString() == tr("返")) {
            commanderVector[index].skillFanHunJue |= 0b01000000;
        }
        if (xlsx.read(excelIndex, 26).toString() == tr("魂")) {
            commanderVector[index].skillFanHunJue |= 0b01000000;
        }
        if (xlsx.read(excelIndex, 27).toString() == tr("觉")) {
            commanderVector[index].skillFanHunJue |= 0b00100000;
        }

        if (xlsx.read(excelIndex, 28).toString() == tr("防")) {
            commanderVector[index].skillFangMouLiaoLin |= 0b10000000;
        }
        if (xlsx.read(excelIndex, 29).toString() == tr("谋")) {
            commanderVector[index].skillFangMouLiaoLin |= 0b01000000;
        }
        if (xlsx.read(excelIndex, 30).toString() == tr("疗")) {
            commanderVector[index].skillFangMouLiaoLin |= 0b00100000;
        }
        if (xlsx.read(excelIndex, 31).toString() == tr("临")) {
            commanderVector[index].skillFangMouLiaoLin |= 0b00010000;
        }

        if (xlsx.read(excelIndex, 32).toString() == tr("识")) {
            commanderVector[index].skillShiFenTongMing |= 0b00001000;
        }
        if (xlsx.read(excelIndex, 33).toString() == tr("奋")) {
            commanderVector[index].skillShiFenTongMing |= 0b00000100;
        }
        if (xlsx.read(excelIndex, 34).toString() == tr("统")) {
            commanderVector[index].skillShiFenTongMing |= 0b00000010;
        }
        if (xlsx.read(excelIndex, 35).toString() == tr("命")) {
            commanderVector[index].skillShiFenTongMing |= 0b00000001;
        }

        commanderVector[index].dataAddress =
                static_cast<quint16>(xlsx.read(excelIndex, 8).toString().toUInt(nullptr, 16));
    }

    size_t notToObjectIndex = 0;
    for (int index = 0x00; index <= 0xFF; ++index) {
        nes.data()[notCompositeToObjetcstartAddress + index] = static_cast<char>(0xFF);
    }
    for (int index = 0x00; index <= 0xFF; ++index) {
        if (notToObjectIndex == 0x80) {
            QMessageBox::warning(nullptr, tr("警告"),
                                 tr("不可合成武将已经过128名\n当前导入到番号为%1\n余下不会被导入到ROM")
                                 .arg(index, 0, 16, QChar('0')));
            break;
        } else if (xlsx.read(excelOffest + index, 5).toString() == tr("否")) {
            nes.data()[notCompositeToObjetcstartAddress + notToObjectIndex] = static_cast<char>(index);
            notToObjectIndex++;
        }
    }

    size_t notAsObjectIndex = 0;
    for (int index = 0x00; index <= 0xFF; ++index) {
        nes.data()[notCompositeAsObjetcstartAddress + index] = static_cast<char>(0xFF);
    }
    for (int index = 0x00; index <= 0xFF; ++index) {
        if (notAsObjectIndex == 0x80) {
            QMessageBox::warning(nullptr, tr("警告"),
                                 tr("不可作为合成素材武将已经过128名\n当前导入到番号为%1\n余下不会被导入到ROM")
                                 .arg(index, 0, 16, QChar('0')));
            break;
        } else if (xlsx.read(excelOffest + index, 6).toString() == tr("否")) {
            nes.data()[notCompositeAsObjetcstartAddress + notAsObjectIndex] = static_cast<char>(index);
            notAsObjectIndex++;
        }
    }

    QMessageBox::warning(nullptr, tr("成功"), tr("读取完成 请保存ROM"));
}

void MilitaryCommander::importSpecial(QString filename) {
    QXlsx::Document xlsx{filename};
    int excelOffest = 2;
    if(!xlsx.selectSheet(tr("特殊"))) {
        QMessageBox::warning(nullptr, tr("警告"), tr("没有特殊表"));
        return;
    }
    auto& nes = MainWindow::nesFileByteArray;

    for (int i = 0; i < 18; ++i) {
        nes.data()[specialInvaild + i] = static_cast<char>(xlsx.read(1, i+excelOffest).toString().toUInt(nullptr, 16));
    }
    for (int i = 0; i < 13; ++i) {
        nes.data()[restoreInvaild + i] = static_cast<char>(xlsx.read(2, i+excelOffest).toString().toUInt(nullptr, 16));
    }
    for (int i = 0; i < 5; ++i) {
        nes.data()[successfulPercentInhalf + i] = static_cast<char>(xlsx.read(3, i+excelOffest).toString().toUInt(nullptr, 16));
    }
    QMessageBox::warning(nullptr, tr("成功"), tr("读取完成 请保存ROM"));
}
