#include <QDebug>
#include <QFileInfo>
#include <QDir>
#include <QDirIterator>
#include <QJsonObject>
#include <QJsonDocument>

#include <ctime>
#include "modelshrimp.h"
#include "utils.h"


ModelShrimp::ModelShrimp()
{

}

ModelShrimp::~ModelShrimp()
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

QStringList ModelShrimp::getLoadedImages()
{
    QStringList imagelist;
    for (auto img: originalImages.keys()) {
        imagelist << img;
    }
    return imagelist;
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
        saveResults = QFileInfo(parser.value("saveres")).canonicalFilePath();
        qDebug() << "ModelShrimp: Results will be saved at " << saveResults;
    }

    if (parser.isSet("savepb")) {
        savePB = QFileInfo(parser.value("savepb")).absoluteFilePath();
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

    qDebug() << "ModelShrimp: Setting the PB";
    if (!parser.isSet("pb")) {
        qDebug() << "ModelShrimp: PB not set, will use self-training";
        pb.setImagelist(downsampledImages);
        pb.createPatternBank(&modelSettings); //at that place we generate the PB

    }
    else {
        loadPatternBank(parser.value("pb"));
    }
}

bool ModelShrimp::loadPatternBank(const QString pblocation)
{
    QFile pbjson(pblocation);
    if (!pbjson.open(QIODevice::ReadOnly)) {
        qWarning("Couldn't open Pattern Bank.");
        return false;
    }
    QByteArray json = pbjson.readAll();
    QJsonDocument pbload(QJsonDocument::fromJson(json));
    QJsonObject mainObject = pbload.object();

    QJsonObject settingsObject = mainObject["settings"].toObject();
    modelSettings.setNpatt(settingsObject["npatt"].toInt());
    modelSettings.setBwidth(settingsObject["bwidth"].toDouble());
    modelSettings.setThr(settingsObject["thrs"].toArray());

    QJsonObject pbObject = mainObject["patternBank"].toObject();
    pb.setPatternBank(pbObject["accepted_patt"].toArray());

    return true;
}

bool ModelShrimp::writePatternBank(const QString pblocation)
{
    QFile saveFile(pblocation);
    if (!saveFile.open(QIODevice::WriteOnly)) {
        qWarning("Couldn't open Pattern Bank for saving.");
        return false;
    }

    QJsonObject mainObject;

    QJsonObject settingsObject;
    modelSettings.write(settingsObject);
    mainObject["settings"] = settingsObject;
    QJsonObject pbObject;
    pb.write(pbObject);
    mainObject["patternBank"] = pbObject;
    QJsonDocument saveDoc(mainObject);
    saveFile.write(saveDoc.toJson());
    return true;
}


void ModelShrimp::runMS()
{
    qDebug() << "----- Run ModelShrimp -----" ;
    //modelSettings.printSettings();
    QVector<int> accepted_patt = pb.getPatternBank(); //and then we load the accepted patterns
    for (const auto& imgname: downsampledImages.keys()) {
        Mat img = downsampledImages.value(imgname);
        qDebug() << "Reconstructing " << imgname;
        Mat recImg(img.rows, img.cols, 0, Scalar(255));
        for (int y = 0; y < img.rows - 2; y++) {
            for (int x = 0; x < img.cols - 2; x++) {
                Mat p = img(Rect(x, y, 3, 3)).clone(); //create pattern
                if ( std::find(accepted_patt.begin(),
                               accepted_patt.end(),
                               Utils::getPatternAddress(p, 3, modelSettings.getThr())) != accepted_patt.end() ) {
                    Rect roi(x,y,3,3);
                    Mat bwroi = img(roi).clone();
                    bwroi.copyTo(recImg(roi));
                }
            }
        }

        reconstructedImages[imgname] = recImg;
    }

    //Save the output
    for (const auto& img: originalImages.keys()) {
        if (saveResults != NULL) {
            QFileInfo imginfo = img;
            qDebug() << "Saving results for" << imginfo.baseName();
            //save the original image
            if (imginfo.absolutePath() != saveResults) {
                imwrite(qPrintable(saveResults + "/" + imginfo.baseName() +
                                   "_1original." + imginfo.completeSuffix()),
                        originalImages[img]);

            }
            //save the greyscale
            qDebug() << qPrintable(saveResults + "/" + imginfo.baseName() +
                                   "_2greyscale." + imginfo.completeSuffix());
            imwrite(qPrintable(saveResults + "/" + imginfo.baseName() +
                               "_2greyscale." + imginfo.completeSuffix()),
                    greyscalelImages[img]);

            //save the downsampled
            QString thrs = "thr";
            for (int t: modelSettings.getThr())
                thrs += "_" + QString::number(t);
            imwrite(qPrintable(saveResults + "/" + imginfo.baseName() +
                               "_3downsampled_" + thrs + "." +
                               imginfo.completeSuffix()),
                    downsampledImages[img]);

            //save the reconstructed
            imwrite(qPrintable(saveResults + "/" + imginfo.baseName() +
                               "_4reconstructed_" + thrs +
                               "_npatt" + QString::number(modelSettings.getNPatt()) +
                               "_bwidth" + QString::number(modelSettings.getBwidth()).replace(".","o") +
                               "." + imginfo.completeSuffix()),
                    reconstructedImages[img]);
        }
    }

    if (savePB != NULL) {
        writePatternBank(savePB);
    }
}
