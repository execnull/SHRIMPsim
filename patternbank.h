#ifndef PATTERNBANK_H
#define PATTERNBANK_H

#include "settings.h"

#include <opencv2/opencv.hpp>
#include <QMap>
#include <QString>

#define DIM 3

using namespace cv;

class PatternBank
{
public:
    PatternBank();
    ~PatternBank();

    bool createPatternBank(Settings* const settings);
    bool write(QJsonObject &json) const;
    bool setPatternBank(const QJsonArray& pb);
    QVector<int> getPatternBank() {return accepted_patt;};

    void setImagelist(QMap<QString, Mat> imagelist);
    //void addImage(QString path);

private:
    double entropy(const double p, const int npatt, const double bwidth);
    int findPatterns(const Mat& img, const QVector<int> &thr);

    QMap<QString, Mat> imagelist;
    long int pspace = 0;
    int* pbank = NULL;
    double* freqbank = NULL;
    QVector<int> accepted_patt;


};

#endif // PATTERNBANK_H
