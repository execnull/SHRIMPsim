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

//Pattern Bank constructor. The pattern bank space and the frequency bank are usin the levels of grey
//specified by the user to find the memory that they need 
PatternBank::PatternBank(Settings s) : settings(s)
{
    qDebug() << "PatternBank: Created for greyscale" << settings.getIsGreyscale();
    if (!settings.getIsGreyscale()) {
        pspace = 1 << (DIM * DIM);
    }
    else
        pspace = 1 << (2 * DIM * DIM);
    pbank = new int[pspace]();
    freqbank = new double[pspace]();
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
double PatternBank::entropy(const double p)
{
    return -p * log10(p) / max(1/(double)settings.getNPatt(), p/settings.getBwidth());
}

int PatternBank::findPatterns(const Mat& img)
{
    int npatt = 0;
    for (int y = 0; y < img.rows - DIM + 1; y++) {
        for (int x = 0; x < img.cols - DIM + 1; x++) {
            npatt++;
            Mat p = img(Rect(x, y, DIM, DIM)).clone();
            pbank[Utils::getPatternAddress(p, DIM, settings.getThr())]++;
        }
    }
    return npatt;
}

bool PatternBank::createPatternBank()
{
    qDebug() << "PatternBank: create Pattern Bank from" << imagelist.size() << "images";
    int npatt = 0;

    //find all the patterns in the image list
    for (const auto& img: imagelist.keys()) {
        qDebug() << img ;
        npatt += findPatterns(imagelist.value(img));
    }

    //qDebug() << "PatternBank: found" << npatt << "patterns";

    for(int i = 0; i < pspace; i++) {
        //std::cout << std::hex << "0x" << std::setw(3) << std::setfill('0') << i <<
        //" 0x" <<  std::setw(8) << std::setfill('0') << pbank[i] << std::dec << std::endl;
        if (pbank[i] != 0) {
            double pi = pbank[i] / (double) npatt;
            freqbank[i] = entropy(pi);
        }
    }

    //N PATTERNS
    std::priority_queue<std::pair<double, int>> q;
    for (int i = 0; i < pspace; ++i) {
        q.push(std::pair<double, int>(freqbank[i], i));
    }
    int k = settings.getNPatt(); // number of indices we need
    for (int i = 0; i < k; ++i) {
        int ki = q.top().second;
        accepted_patt.push_back(ki);
        q.pop();
    }

    qDebug() << "PatternBank: selected" << accepted_patt.size() << "patterns";

    return true;
}
