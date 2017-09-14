#ifndef QSHRIMP_H
#define QSHRIMP_H

#include <QMainWindow>

namespace Ui {
class qshrimp;
}

class qshrimp : public QMainWindow
{
    Q_OBJECT

public:
    explicit qshrimp(QWidget *parent = 0);
    ~qshrimp();

private slots:
    void on_action_Quit_triggered();

private:
    Ui::qshrimp *ui;
};

#endif // QSHRIMP_H
