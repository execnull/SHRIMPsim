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
    ~ModelShrimp();

    /*!
      Function parsing the settings. Currently only one is available, for the command line.
      However, more can be provided, e.g. to parse the settings from the gui.
    */
    void parseSettings(QCommandLineParser &parser);
    /*!
     * Function adding an image to be processed by the model
     */
    void addImage(QFileInfo& file);

    QStringList getLoadedImages();

    /*!
      Function loading the Pattern Bank from the class
    */
    bool loadPatternBank(const QString pblocation);
    bool writePatternBank(const QString pblocation);

    /*!
      Function that takes all the information and runs the simulation. 
    */
    void runMS();

private:
    Settings modelSettings;
    PatternBank pb;

    QMap<QString, Mat> originalImages;
    QMap<QString, Mat> greyscalelImages;
    QMap<QString, Mat> downsampledImages;
    QMap<QString, Mat> reconstructedImages;

    QString savePB = NULL;
    QString saveResults = NULL;
};

#endif // MODELSHRIMP_H
