#include "qshrimp.h"
#include "ui_qshrimp.h"

qshrimp::qshrimp(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::qshrimp)
{
    ui->setupUi(this);
}

qshrimp::~qshrimp()
{
    delete ui;
}

/******** ACTIONS *******/
void qshrimp::on_action_Quit_triggered()
{
    QApplication::quit();
}
