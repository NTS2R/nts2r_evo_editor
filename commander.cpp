#include "commander.h"
#include "militarycommander.h"
#include <QDebug>
void Commander::setCommanderAttribute(QByteArray data, QByteArray animation, int dataAddress, QByteArray dajiangData) {
    this->data = data;
    this->dataAddress = dataAddress;
    this->dajiang = static_cast<quint8>(dajiangData[0]);
    gong = static_cast<quint8>(dajiangData[1]);
    fang = static_cast<quint8>(dajiangData[2]);
    ming = static_cast<quint8>(dajiangData[3]);
    bi = static_cast<quint8>(dajiangData[4]);
    quint8 offset0 = static_cast<quint8>(data.at(0));
    //颜色 offset0 low
    color = offset0 & 0xf;
    //章节 offset0 high
    chapter = offset0 >> 4;
    //模型 offset1
    model = static_cast<quint8>(data.at(1));
    //智力 offset2
    zhili = static_cast<quint8>(data.at(2));
    //武力 offset3
    wuli = static_cast<quint8>(data.at(3));
    //速度 offset4
    sudu = static_cast<quint8>(data.at(4));
    //敌方流派 offset5
    auto difangliupaiAndQi = static_cast<quint8>(data.at(5));
    difangliupai = difangliupaiAndQi & 0x7F;
    skillQi = difangliupaiAndQi & 0x80;
    //我方流派 offset6
    wofangliupai = static_cast<quint8>(data.at(6));
    //特技 仁 慧 挡 offset7 80/40/20
    auto skillRenHuiDangAndJimou = static_cast<quint8>(data[7]);
    skillRenHuiDang = skillRenHuiDangAndJimou & 0b11100000;
    jimou = skillRenHuiDangAndJimou & 0b00011111;
    moulvezhi = static_cast<quint8>(static_cast<quint8>(data[8]));
    gongjili = static_cast<quint8>(static_cast<quint8>(data[9]));
    fangyuli = static_cast<quint8>(static_cast<quint8>(data[10]));
    // 地形 计策
    auto dixingAndJice = static_cast<quint8>(static_cast<quint8>(data[11]));
    dixing = (dixingAndJice & 0xF0) >> 4;
    jice = dixingAndJice & 0x0F;
    //特技 避 攻 武 智 术 offset12 80/40/20/10/08
    auto skillBiGongWuZhiShuAndWeapon = static_cast<quint8>(data.at(12));
    weapon = skillBiGongWuZhiShuAndWeapon & 0b00000111;
    skillBiGongWuZhiShu = skillBiGongWuZhiShuAndWeapon & 0b11111000;
    //特技 返 魂 觉 offset13 80/40/20
    skillFanHunJue = static_cast<quint8>(data.at(13) & 0b11100000);
    //掉宝流派 offset13 0x00-0x1F
    diaobaoliupai = static_cast<quint8>(data.at(13)) & (0b00011111);
    //脸谱 offset 14 - 19
    face = data.mid(14, 6).toHex(' ');
    //特技 offset 20 1临2疗4谋8防 1命2统4奋8识
    auto skillFangMouLiaoLinShiFenTongMing = static_cast<quint8>(data.at(20));
    skillFangMouLiaoLin = skillFangMouLiaoLinShiFenTongMing & 0xf0;
    skillShiFenTongMing = skillFangMouLiaoLinShiFenTongMing & 0x0f;
    //脸谱控制 & 繁体中文控制
    quint8 offset21 = static_cast<quint8>(data.at(21));
    faceControl = offset21 >> 4;
    chtNameControl = offset21 & 0xf;
    //CHT角色名字 offset22 -24
    chtName = data.mid(22, 3).toHex(' ');
    //CHS角色名字 offset25 - 最后
    chsName = data.mid(MilitaryCommander::leastLength).toHex(' ');

    // Animation
    attackAnimation = static_cast<quint8>(animation[0]);
    deadAnimation = static_cast<quint8>(animation[1]);
}

Commander& Commander::update() {
    qDebug() << data;
    data[0] = static_cast<char>(chapter << 4 | color);
    qDebug() << data;
    data[1] = static_cast<char>(model);
    data[2] = static_cast<char>(zhili);
    data[3] = static_cast<char>(wuli);
    data[4] = static_cast<char>(sudu);
    data[5] = static_cast<char>(difangliupai  | skillQi);
    data[6] = static_cast<char>(wofangliupai);
    //特技 仁 慧 挡 offset7 80/40/20
    data[7] = static_cast<char>(skillRenHuiDang | jimou);
    data[8] = static_cast<char>(moulvezhi);
    data[9] = static_cast<char>(gongjili);
    data[10] = static_cast<char>(fangyuli);
    data[11] = static_cast<char>((dixing << 4) + jice);
    //特技 避 攻 武 智 术 offset12 80/40/20/10/08
    data[12] = static_cast<char>(skillBiGongWuZhiShu | weapon);
    //特技 返 魂 觉 offset13 80/40/20
    //掉宝流派 offset13 0x00-0x1F
    data[13] = static_cast<char>(skillFanHunJue | diaobaoliupai);
    //脸谱 offset 14 - 19
    auto faceList = face.split(' ');
    for (int i = 0; i < 6; i++) {
        data[14 + i] = static_cast<char>(faceList[i].toUInt(nullptr, 16));
    }
    //特技 offset 20 1临2医4谋8防 1命2统4奋8识
    data[20] = static_cast<char>(skillFangMouLiaoLin | skillShiFenTongMing);
    qDebug() << (skillFangMouLiaoLin | skillShiFenTongMing);
    //脸谱控制 & 繁体中文控制 offset21
    data[21] = static_cast<char>((faceControl << 4) | chtNameControl);
    //CHT角色名字 offset22 -24
    auto chtNameList = chtName.split(' ');
    for (int i = 0; i < 3; i++) {
        data[22 + i] = static_cast<char>(chtNameList[i].toUInt(nullptr, 16));
    }
    //CHS角色名字 offset25 - 最后
    auto chsNameList = chsName.split(' ');
    qDebug() << chsNameList;
    for (int i = 0; i < chsNameList.length(); i++) {
        data[25 + i] = static_cast<char>(chsNameList[i].toUInt(nullptr, 16));
    }

    qDebug() << data;
    return *this;
}
