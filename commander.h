#ifndef COMMANDER_H
#define COMMANDER_H

#include <QByteArray>
#include <QString>
struct Commander
{
public:
    void setCommanderAttribute(QByteArray data, QByteArray animation, int dataAddress, QByteArray dajiangData);
    Commander& update();
    int dataAddress;
    quint8 color;
    quint8 chapter;
    quint8 model;
    quint8 zhili;
    quint8 wuli;
    quint8 sudu;
    quint8 wofangliupai;
    quint8 difangliupai;
    quint8 dixing;
    quint8 jice;
    quint8 direnjimou;
    quint8 direnmoulvezhi;
    quint8 direngongjili;
    quint8 direnfangyuli;
    quint8 gongjili;
    quint8 fangyuli;
    quint8 jimou;
    quint8 moulvezhi;
    quint8 weapon;
    quint8 dajiang;
    quint8 gong, fang, ming, bi;

    quint8 skillRenHuiDang;
    quint8 skillBiGongWuZhiShu;
    quint8 skillFanHunJue;
    quint8 skillFangMouLiaoLin;
    quint8 skillShiFenTongMing;
    quint8 skillQi;

    quint8 diaobaoliupai;
    QString face;
    quint8 faceControl;
    quint8 chtNameControl;
    QString chtName;
    QString chsName;
    QByteArray data;

    quint8 attackAnimation;
    quint8 deadAnimation;
    quint8 buhuo;
    quint8 zhansha;
    quint8 attackCount;
    quint8 celveCount;
};

#endif // COMMANDER_H
