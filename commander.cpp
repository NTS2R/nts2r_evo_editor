#include "commander.h"
#include "militarycommander.h"
Commander::Commander()
{

}

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
    //掉宝流派 offset13
    diaobaoliupai = static_cast<quint8>(data.at(13));
    //脸谱 offset 14 - 19
    face = data.mid(14, 6).toHex(' ');
    //脸谱控制 & 繁体中文控制
    quint8 offset21 = static_cast<quint8>(data.at(21));
    faceControl = offset21 >> 4;
    chtNameControl = offset21 & 0xf;
    //CHT角色名字 offset22 -24
    chtName = data.mid(22, 3).toHex(' ');
    //CHS角色名字 offset25 - 最后
    chsName = data.mid(MilitaryCommander::least_length).toHex(' ');
}
