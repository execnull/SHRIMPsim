#include "settings.h"
#include <QDebug>
#include <QString>
#include <QJsonArray>


Settings::Settings()
{

}

Settings::Settings(int npatt, double bwidth, int thr1, int thr2 = 0, int thr3 = 0, QString name = "") :
    name(name),
    npatt(npatt),
    bwidth(bwidth)
{
    if (npatt < 0)
        npatt = 0;
    if (bwidth < 0)
        bwidth = 0;

    if (thr1 < 0)
        thr1 = 0;

    thr.push_back(thr1);
    thr.push_back(thr2);
    thr.push_back(thr3);
}

void Settings::setThr(QStringList list)
{
    thr.clear();
    if (list.size() == 1)
        thr.push_back(list[0].toInt());
    else if (list.size() == 3) {
        thr.push_back(list[0].toInt());
        thr.push_back(list[1].toInt());
        thr.push_back(list[2].toInt());
    }
}


QString Settings::getName()
{
    return name;
}

int Settings::getNPatt()
{
    return npatt;
}

double Settings::getBwidth()
{
    return bwidth;
}

QVector<int> Settings::getThr()
{
    return thr;
}

bool Settings::getIsGreyscale()
{
    return (thr.size() == 3) ? true : false;
 //   return ((thr.size() == 3)|| (thr[1] != 0 && thr[2] != 0)) ? true : false;

}


void Settings::printSettings() {
    QString thrs = "";
    for (int t: getThr())
        thrs += QString::number(t) + " ";
    qDebug("Settings: npatt %d bwidth %f BW %d thr %s ", npatt, bwidth, getIsGreyscale(), qPrintable(thrs));
}

void Settings::write(QJsonObject &json) const
{
    json["name"] = name;
    json["npatt"] = npatt;
    json["bwidth"] = bwidth;
    QJsonArray thrs;
    foreach (const int t, thr) {
        thrs.append(t);
    }
    json["thrs"] = thrs;
}
