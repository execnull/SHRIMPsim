#include <QDebug>
#include <QFileInfo>
#include <QDir>
#include <QDirIterator>
#include <QJsonDocument>

#include <ctime>
#include "modelshrimp.h"
#include "utils.h"


ModelShrimp::ModelShrimp()
{

}

void ModelShrimp::addImage(QFileInfo& file)
{
    originalImages[file.canonicalFilePath()] = imread(qPrintable(file.canonicalFilePath()), IMREAD_COLOR);
    greyscalelImages[file.canonicalFilePath()] = imread(qPrintable(file.canonicalFilePath()), IMREAD_GRAYSCALE );
    qDebug() << "\t" << file.baseName()
             << ": Dimensions:" << originalImages[file.canonicalFilePath()].rows
             << "x" << originalImages[file.canonicalFilePath()].cols;

}

void ModelShrimp::getOriginalImages()
{

}

void ModelShrimp::parseSettings(QCommandLineParser &parser)
{
    //Just pass the basic settings to the Settings object
    qDebug() << "ModelShrimp: Parsing settings..";
    modelSettings.setNpatt(parser.value("npatt").toInt());
    modelSettings.setBwidth(parser.value("bwidth").toDouble());
    modelSettings.setThr(parser.values("thr"));

    //parse the path for saving the results
    if (parser.isSet("saveres")) {
        saveResults = true;
        saveres = QFileInfo(parser.value("saveres")).canonicalFilePath();
        qDebug() << "ModelShrimp: Results will be saved at " << saveres;
    }


    qDebug() << "ModelShrimp: Adding image for processing: ";
    const QStringList args = parser.positionalArguments();
    for (auto arg: args) { //if the image location is a directory, recursively add them all
        QFileInfo file = arg;
        if (file.isDir()) {
            QString dir = file.canonicalPath() + QDir::separator() + file.baseName() + QDir::separator();
            QDirIterator it(dir, QStringList() << "*.jpg", QDir::Files);
            while (it.hasNext()) {
                QFileInfo diritem = it.next();
                addImage(diritem);
            }
        }
        else { //if it's a list of images, just add them
            addImage(file);
        }
    }

    qDebug() << "ModelShrimp: Generating downsampled images:";
    double duration;

    std::clock_t start;
    //Generate the downsampled images, either on B/W or in greyscale
    for (const auto& img: greyscalelImages.keys()) {
        start = std::clock();
        downsampledImages[img] = Utils::downsampleImage(greyscalelImages[img], modelSettings.getThr());
        duration = ( std::clock() - start ) / (double) CLOCKS_PER_SEC;
        QFileInfo f = img;
        qDebug() << "\t"<< f.baseName() << ":"<< duration << "ms";
    }

    //Pattern Bank parsing
    if (!parser.isSet("pb")) {
        //Set the PatternBank to self-train
        pb = new PatternBank(modelSettings);
        qDebug() << "ModelShrimp: PB not set, will use self-training";
        pb->setImagelist(downsampledImages);
    }
    else {
        //now this part needs some work to be done using the loadPatternBank function which follows.
        //pb = new PatternBank(parser.value("pb"));
        pb->setImagelist(downsampledImages);
    }

    if (parser.isSet("savepb")) {
        qDebug() << "Pattern Bank will be saved at" << parser.value("savepb");
        //  pb->savePatternBank(parser.value("savepb"));
    }


}

void ModelShrimp::loadPatternBank()
{
    //two different cases for loading
    //1. json
    //2. filelist
}

bool ModelShrimp::writePatternBank()
{
    QFile saveFile("saved.json");
    if (!saveFile.open(QIODevice::WriteOnly)) {
        qWarning("Couldn't open save file.");
        return false;
    }

    QJsonObject mainObject;

    QJsonObject settingsObject;
    modelSettings.write(settingsObject);
    mainObject["settings"] = settingsObject;
    QJsonObject pbObject;
    pb->write(pbObject);
    mainObject["patternBank"] = pbObject;
    QJsonDocument saveDoc(mainObject);
    saveFile.write(saveDoc.toJson());
    return true;
}


void ModelShrimp::runMS()
{
    qDebug() << "----- Run ModelShrimp sept-----" ;
    modelSettings.printSettings();
    qDebug() << "Imagelist size: " << originalImages.size();
    pb->createPatternBank(); //at that place we generate the PB
    QVector<int> accepted_patt = pb->getPatternBank(); //and then we load the accepted patterns

    for (const auto& imgname: downsampledImages.keys()) {
        Mat img = downsampledImages.value(imgname);
        qDebug() << "Reconstructing " << imgname;
        Mat recImg(img.rows, img.cols, 0, Scalar(255));
        qDebug() << "reco" << recImg.rows << " " << recImg.cols;
        for (int y = 0; y < img.rows - 2; y++) {
            for (int x = 0; x < img.cols - 2; x++) {
                Mat p = img(Rect(x, y, 3, 3)).clone(); //create pattern
                if ( std::find(accepted_patt.begin(),
                               accepted_patt.end(),
                               Utils::getPatternAddress(p, 3, modelSettings.getThr())) != accepted_patt.end() ) {
                    qDebug() << "matched!" ;
                    Rect roi(x,y,3,3);
                    qDebug() << roi.area();
                    Mat bwroi = img(roi).clone();
                    qDebug() << bwroi.rows << bwroi.cols;
                    bwroi.copyTo(recImg(roi));
                }
                //else
                  //  qDebug() << "not matched" << Utils::getPatternAddress(p, 3, modelSettings.getThr());
            }
        }

        reconstructedImages[imgname] = recImg;
    }

    //Save the output
    for (const auto& img: originalImages.keys()) {
        if (saveResults) {
            QFileInfo imginfo = img;
            qDebug() << "Saving results for" << imginfo.baseName();
            //save the original image
            if (imginfo.absolutePath() != saveres) {
                imwrite(qPrintable(saveres + "/" + imginfo.baseName() +
                                   "_1original." + imginfo.completeSuffix()),
                        originalImages[img]);

            }
            //save the greyscale
            imwrite(qPrintable(saveres + "/" + imginfo.baseName() +
                               "_2greyscale." + imginfo.completeSuffix()),
                    greyscalelImages[img]);

            //save the downsampled
            QString thrs = "thr";
            for (int t: modelSettings.getThr())
                thrs += "_" + QString::number(t);
            imwrite(qPrintable(saveres + "/" + imginfo.baseName() +
                               "_3downsampled_" + thrs + "." +
                               imginfo.completeSuffix()),
                    downsampledImages[img]);

            //save the reconstructed
            imwrite(qPrintable(saveres + "/" + imginfo.baseName() +
                               "_4reconstructed_" + thrs +
                               "_npatt" + QString::number(modelSettings.getNPatt()) +
                               "_bwidth" + QString::number(modelSettings.getBwidth()).replace(".","o") +
                               "." + imginfo.completeSuffix()),
                    reconstructedImages[img]);
        }
    }

    writePatternBank();
}
