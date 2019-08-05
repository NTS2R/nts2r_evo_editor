#include "commander.h"
#include "militarycommander.h"
#include <QDebug>
void Commander::setCommanderAttribute(QByteArray data) {
    this->data = data;
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
    difangliupai = static_cast<quint8>(data.at(5));
    //我方流派 offset6
    wofangliupai = static_cast<quint8>(data.at(6));
    //特技 仁 慧 挡 offset7 80/40/20
    skillRenHuiDang = static_cast<quint8>(static_cast<quint8>(data.at(7)) & (0b11100000)) >> 5;
    //特技 避 攻 武 智 术 offset12 80/40/20/10/08
    skillBiGongWuZhiShu = static_cast<quint8>(static_cast<quint8>(data.at(12)) & (0b11111000)) >> 3;
    //特技 返 魂 觉 offset13 80/40/20
    skillFanHunJue = static_cast<quint8>(static_cast<quint8>(data.at(13)) & (0b11100000)) >> 5;
    //掉宝流派 offset13 0x00-0x1F
    diaobaoliupai = static_cast<quint8>(data.at(13)) & (0b00011111);
    //脸谱 offset 14 - 19
    face = data.mid(14, 6).toHex(' ');
    //特技 offset 20 1临2疗4谋8防 1命2统4奋8识
    auto skillFangMouLiaoLinShiFenTongMing = static_cast<quint8>(data.at(20));
    skillFangMouLiaoLin = skillFangMouLiaoLinShiFenTongMing >> 4;
    skillShiFenTongMing = skillFangMouLiaoLinShiFenTongMing & 0xf;
    //脸谱控制 & 繁体中文控制
    quint8 offset21 = static_cast<quint8>(data.at(21));
    faceControl = offset21 >> 4;
    chtNameControl = offset21 & 0xf;
    //CHT角色名字 offset22 -24
    chtName = data.mid(22, 3).toHex(' ');
    //CHS角色名字 offset25 - 最后
    chsName = data.mid(MilitaryCommander::least_length).toHex(' ');
}

Commander& Commander::update() {
    qDebug() << data;
    data[0] = static_cast<char>(chapter << 4 | color);
    qDebug() << data;
    data[1] = static_cast<char>(model);
    data[2] = static_cast<char>(zhili);
    data[3] = static_cast<char>(wuli);
    data[4] = static_cast<char>(sudu);
    data[5] = static_cast<char>(difangliupai);
    data[6] = static_cast<char>(wofangliupai);
    //特技 仁 慧 挡 offset7 80/40/20
    data[7] = (data[7] & (0b00011111)) | static_cast<char>(skillRenHuiDang << 5);
    //特技 避 攻 武 智 术 offset12 80/40/20/10/08
    data[12] = (data[12] & (0x07)) | static_cast<char>(skillBiGongWuZhiShu << 3);
    //特技 返 魂 觉 offset13 80/40/20
    //掉宝流派 offset13 0x00-0x1F
    data[13] = static_cast<char>(skillFanHunJue << 5) + static_cast<char>(diaobaoliupai);
    //脸谱 offset 14 - 19
    auto faceList = face.split(' ');
    for (int i = 0; i < 6; i++) {
        data[14 + i] = static_cast<char>(faceList[i].toUInt(nullptr, 16));
    }
    //特技 offset 20 1临2医4谋8防 1命2统4奋8识
    data[20] = static_cast<char>((skillFangMouLiaoLin << 4)+ skillShiFenTongMing);
    qDebug() << ((skillFangMouLiaoLin << 4)+ skillShiFenTongMing);
    //脸谱控制 & 繁体中文控制 offset21
    data[21] = static_cast<char>((faceControl << 4)+ chtNameControl);
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
