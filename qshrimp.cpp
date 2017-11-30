#include "qshrimp.h"
#include "ui_qshrimp.h"

#include <QtWidgets>

qshrimp::qshrimp(ModelShrimp *ms, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::qshrimp)
{
    modelshrimp = ms;
    ui->setupUi(this);
    scaleFactor = 1.0;

    //Create the different tabs to be shown.
    for (size_t im = Original; im != ImageTypeCount; ++im) {
        QLabel* label = new QLabel();
        label->setBackgroundRole(QPalette::Base);
        label->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
        label->setScaledContents(true);
        label->adjustSize();
        imageLabel.append(label);

        QScrollArea* scroll = new QScrollArea();
        scroll->setBackgroundRole(QPalette::Base);
        scroll->setWidget(label);
        scroll->setVisible(false);
        scrollArea.append(scroll);

        //ui->imagesTab->insertTab(-1, scroll, imageName(im));
        //ui->imagesTab->setTabEnabled(im, false);
    }



}

qshrimp::~qshrimp()
{
    delete ui;
}

/******** INTERNAL FUNCTIONS *******/
static void initializeImageFileDialog(QFileDialog &dialog, QFileDialog::AcceptMode acceptMode)
{
    //Create a mimetype list.
    QStringList mimeTypeFilters;
    const QByteArrayList supportedMimeTypes = acceptMode == QFileDialog::AcceptOpen
            ? QImageReader::supportedMimeTypes() : QImageWriter::supportedMimeTypes();
    foreach (const QByteArray &mimeTypeName, supportedMimeTypes) {
        mimeTypeFilters.append(mimeTypeName);
    }
    mimeTypeFilters.append("image/tif");
    mimeTypeFilters.append("image/tiff");
    mimeTypeFilters.sort();

    //Set the mimetype in the dialog
    dialog.setMimeTypeFilters(mimeTypeFilters);
    dialog.selectMimeTypeFilter("image/jpeg");

    //Accept more than one files for input
    dialog.setFileMode(QFileDialog::ExistingFiles);

    //For saving mode
    if (acceptMode == QFileDialog::AcceptSave)
        dialog.setDefaultSuffix("jpg");
}


//Function to load the image files
bool qshrimp::loadFile(const QStringList &fileName)
{
    QImageReader reader(fileName.first());
    reader.setAutoTransform(true);
    const QImage newImage = reader.read();

    //    modelshrimp->reset();
    for (int i = 0; i < fileName.size(); ++i) {
        QFileInfo f = fileName.at(i);
        qDebug() << "added image " << fileName.at(i).toUtf8().constData();
        modelshrimp->addImage(f);
    }

    qDebug() << "loadFile setting image list";
    imageList->setStringList(modelshrimp->getLoadedImages());

//    imageList->setStringList(fileName);
//    QModelIndex index = imageList->index(0,0);
//    ui->imagelistView->setCurrentIndex(index);

//    setWindowFilePath(fileName.first());

//    const QString message = tr("Loaded \%1\ Images, %2x%3, Depth: %4")
//            .arg(fileName.size()).arg(newImage.width()).arg(newImage.height()).arg(newImage.depth());
//    statusBar()->showMessage(message);

//    ui->imagesTab->setCurrentIndex(0);

    return true;
}

/******** ACTIONS *******/
void qshrimp::on_action_Quit_triggered()
{
    QApplication::quit();
}



void qshrimp::on_actionOpen_File_triggered()
{
    QFileDialog dialog(this, tr("Open Files"));
    initializeImageFileDialog(dialog, QFileDialog::AcceptOpen);

    while (dialog.exec() == QDialog::Accepted && !loadFile(dialog.selectedFiles())) {}
}


