#ifndef MODELSHRIMP_H
#define MODELSHRIMP_H

#include "settings.h"
#include "patternbank.h"
#include <opencv2/opencv.hpp>

#include <QCommandLineParser>
#include <QMap>
#include <QFileInfo>

/*!
  The model handles the transformation of the images. 
*/
class ModelShrimp
{
public:
    ModelShrimp();

    /*!
      Function parsing the settings. Currently only one is available, for the command line.
      However, more can be provided, e.g. to parse the settings from the gui.
    */
    void parseSettings(QCommandLineParser &parser);

    void addImage(QFileInfo& file);

    void getOriginalImages();

    /*!
      Function loading the Pattern Bank from the class
    */
    void loadPatternBank();
    bool writePatternBank();

    /*!
      Function that takes all the information and runs the simulation. 
    */
    void runMS();

private:
    Settings modelSettings;
    PatternBank* pb;

    QMap<QString, Mat> originalImages;
    QMap<QString, Mat> greyscalelImages;
    QMap<QString, Mat> downsampledImages;
    QMap<QString, Mat> reconstructedImages;

    bool savePB = false;
    bool saveResults = false;
    QString saveres;


};

#endif // MODELSHRIMP_H
