#ifndef QSHRIMP_H
#define QSHRIMP_H

#include <QMainWindow>
#include <QtWidgets>

#include "modelshrimp.h"

namespace Ui {
class qshrimp;
}

class qshrimp : public QMainWindow
{
    Q_OBJECT

public:
    explicit qshrimp(ModelShrimp *ms, QWidget *parent = 0);
    ~qshrimp();

private slots:
    void on_action_Quit_triggered();

    void on_actionOpen_File_triggered();

private:
    Ui::qshrimp *ui;
    ModelShrimp* modelshrimp;

    enum ImageType {
        Original = 0,
        Greyscale = 1,
        Downsampled = 2,
        Reconstructed = 3,
        ImageTypeCount = 4
    };

    inline const char* imageName(int v)
    {
        switch (v)
        {
          case 0:   return "Original";
          case 1:   return "Greyscale";
          case 2:   return "Downsampled";
          case 3:   return "Reconstructed";
          default:  return "[Unknown Image]";
        }
    }

    QVector <QLabel*> imageLabel; ///Vector containing the images shown in the different tabs
    QVector <QScrollArea*> scrollArea; ///scroll area containing the images
    double scaleFactor; ///scale Factor of the images
    QStringListModel* imageList; ///yeah, good question

    bool loadFile(const QStringList &fileName);

};

#endif // QSHRIMP_H
