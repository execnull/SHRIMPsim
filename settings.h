#ifndef SETTINGS_H
#define SETTINGS_H

#include <QString>
#include <QVector>
#include <QDebug>
#include <QJsonObject>

/*!
  Class storing the settings of the model. This was created to transport the information easier and to
  keep them contained. The settings contained are the number of patterns used, the bandwidth, the thresholds
  and whether it's b/w or greyscale. 
*/
class Settings
{
public:
    Settings();
    Settings(int npatt, double bwidth, int thr1, int thr2, int thr3,QString name);

    void setNpatt(int npatt) { this->npatt = npatt;}; ///
    void setBwidth(double bwidth) { this->bwidth = bwidth;};
    void setThr(QStringList list);

    QString getName();
    int getNPatt();
    double getBwidth();
    bool getIsGreyscale();
    QVector<int> getThr();

    void printSettings();
    void write(QJsonObject &json) const;


private:
    QString name;
    int npatt;
    double bwidth;
    QVector<int> thr;
};

#endif // SETTINGS_H
