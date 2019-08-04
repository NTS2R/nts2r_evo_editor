#ifndef COMMANDER_H
#define COMMANDER_H

#include <QByteArray>
#include <QString>
struct Commander
{
public:
    void setCommanderAttribute(QByteArray data);
    Commander& update();
    quint8 color;
    quint8 chapter;
    quint8 model;
    quint8 zhili;
    quint8 wuli;
    quint8 sudu;
    quint8 wofangliupai;
    quint8 difangliupai;

    quint8 skillRenHuiDang;
    quint8 skillBiGongWuZhi;
    quint8 skillFanHunJue;
    quint8 skillFangMouYiLin;
    quint8 skillShiFenTongMing;

    quint8 diaobaoliupai;
    QString face;
    quint8 faceControl;
    quint8 chtNameControl;
    QString chtName;
    QString chsName;
    QByteArray data;
};

#endif // COMMANDER_H
