#include "patternbank.h"
#include "utils.h"

#include <queue>

#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QFile>
#include <QDebug>

PatternBank::PatternBank()
{

}

PatternBank::~PatternBank()
{
    delete[] pbank;
    delete[] freqbank;
}

//Set the list of images to work on. 
void PatternBank::setImagelist(QMap<QString, Mat> imagelist)
{
    this->imagelist = imagelist;
    qDebug() << "PatternBank: added " << imagelist.size() <<"images";
}

bool PatternBank::write(QJsonObject &json) const
{
    json["size"] = accepted_patt.size();
    QJsonArray patterns;
    foreach (const int patt, accepted_patt) {
        patterns.append(patt);
    }
    json["accepted_patt"] = patterns;

    return true;
}

//Del Viva's entropy function
double PatternBank::entropy(const double p, const int npatt, const double bwidth)
{
    qDebug() << "entropy " << p << " " << log10(p);
    return -p * log10(p) / max(1/(double)npatt, p/bwidth);
}

int PatternBank::findPatterns(const Mat& img, const QVector<int> &thr)
{
    int npatt = 0;
    for (int y = 0; y < img.rows - DIM + 1; y++) {
        for (int x = 0; x < img.cols - DIM + 1; x++) {
            npatt++;
            Mat p = img(Rect(x, y, DIM, DIM)).clone();
            pbank[Utils::getPatternAddress(p, DIM, thr)]++;
        }
    }
    return npatt;
}

bool PatternBank::createPatternBank(Settings* const settings)
{
    qDebug() << "PatternBank: Create Pattern Bank. ";
    if (!settings->getIsGreyscale()) {
        pspace = 1 << (DIM * DIM);
    }
    else
        pspace = 1 << (2 * DIM * DIM);
    pbank = new int[pspace]();
    freqbank = new double[pspace]();

    int npatt = 0;

    //find all the patterns in the image list
    qDebug() << "find patterns in image list";
    for (const auto& img: imagelist.keys()) {
        qDebug() << img ;
        npatt += findPatterns(imagelist.value(img), settings->getThr());
    }

    qDebug() << "PatternBank: found" << npatt << "patterns";

    for(int i = 0; i < pspace; i++) {
        if (pbank[i] != 0) {
            double pi = pbank[i] / (double) npatt;
            freqbank[i] = entropy(pi, settings->getNPatt(), settings->getBwidth());
        }
        std::cout << i << std::hex << " 0x" << std::setw(3) << std::setfill('0') << i
        << " 0x" <<  std::setw(8) << std::setfill('0') << pbank[i]
                     << std::dec << " " << freqbank[i] << std::endl;

    }

    //N PATTERNS
    std::priority_queue<std::pair<double, int>> q;
    for (int i = 0; i < pspace; ++i) {
        q.push(std::pair<double, int>(freqbank[i], i));
    }
    int k = settings->getNPatt(); // number of indices we need
    for (int i = 0; i < k; ++i) {
        int ki = q.top().second;
        accepted_patt.push_back(ki);
        q.pop();
    }

    qDebug() << "PatternBank: selected" << accepted_patt.size() << "patterns";

    return true;
}

bool PatternBank::setPatternBank(const QJsonArray& pb)
{
    accepted_patt.clear();
    for (auto p: pb)
        accepted_patt.push_back(p.toInt());
    return true;
}
