#ifndef COMMANDER_H
#define COMMANDER_H

#include <QByteArray>
#include <QString>
struct Commander
{
public:
    void setCommanderAttribute(QByteArray data, QByteArray animation, int dataAddress);
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
};

#endif // COMMANDER_H
