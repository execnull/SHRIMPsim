/********************************************************************************
** Form generated from reading UI file 'qshrimp.ui'
**
** Created by: Qt User Interface Compiler version 5.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_QSHRIMP_H
#define UI_QSHRIMP_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_qshrimp
{
public:
    QAction *action_About;
    QAction *action_Quit;
    QWidget *centralWidget;
    QMenuBar *menuBar;
    QMenu *menuFile;
    QMenu *menuView;
    QMenu *menuPattern_Bank;
    QMenu *menuQuit;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *qshrimp)
    {
        if (qshrimp->objectName().isEmpty())
            qshrimp->setObjectName(QStringLiteral("qshrimp"));
        qshrimp->resize(944, 639);
        QFont font;
        font.setPointSize(8);
        qshrimp->setFont(font);
        action_About = new QAction(qshrimp);
        action_About->setObjectName(QStringLiteral("action_About"));
        action_Quit = new QAction(qshrimp);
        action_Quit->setObjectName(QStringLiteral("action_Quit"));
        centralWidget = new QWidget(qshrimp);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        qshrimp->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(qshrimp);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 944, 23));
        menuFile = new QMenu(menuBar);
        menuFile->setObjectName(QStringLiteral("menuFile"));
        menuView = new QMenu(menuBar);
        menuView->setObjectName(QStringLiteral("menuView"));
        menuPattern_Bank = new QMenu(menuBar);
        menuPattern_Bank->setObjectName(QStringLiteral("menuPattern_Bank"));
        menuQuit = new QMenu(menuBar);
        menuQuit->setObjectName(QStringLiteral("menuQuit"));
        qshrimp->setMenuBar(menuBar);
        mainToolBar = new QToolBar(qshrimp);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        qshrimp->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(qshrimp);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        qshrimp->setStatusBar(statusBar);

        menuBar->addAction(menuFile->menuAction());
        menuBar->addAction(menuView->menuAction());
        menuBar->addAction(menuPattern_Bank->menuAction());
        menuBar->addAction(menuQuit->menuAction());
        menuFile->addAction(action_Quit);
        menuQuit->addAction(action_About);

        retranslateUi(qshrimp);

        QMetaObject::connectSlotsByName(qshrimp);
    } // setupUi

    void retranslateUi(QMainWindow *qshrimp)
    {
        qshrimp->setWindowTitle(QApplication::translate("qshrimp", "qSHRIMP", Q_NULLPTR));
        action_About->setText(QApplication::translate("qshrimp", "&About", Q_NULLPTR));
        action_Quit->setText(QApplication::translate("qshrimp", "&Quit", Q_NULLPTR));
        menuFile->setTitle(QApplication::translate("qshrimp", "&File", Q_NULLPTR));
        menuView->setTitle(QApplication::translate("qshrimp", "&View", Q_NULLPTR));
        menuPattern_Bank->setTitle(QApplication::translate("qshrimp", "&Pattern Bank", Q_NULLPTR));
        menuQuit->setTitle(QApplication::translate("qshrimp", "&Help", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class qshrimp: public Ui_qshrimp {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_QSHRIMP_H
