/********************************************************************************
** Form generated from reading UI file 'qkdsim.ui'
**
** Created by: Qt User Interface Compiler version 6.4.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_QKDSIM_H
#define UI_QKDSIM_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_QKDSim
{
public:
    QAction *action_open_net;
    QAction *action_open_dem;
    QAction *action_save_net;
    QAction *action_save_dem;
    QAction *action_exit;
    QAction *action_qt;
    QWidget *centralwidget;
    QGridLayout *gridLayout;
    QTabWidget *tabWidget;
    QWidget *tab;
    QFormLayout *formLayout;
    QTableWidget *tableWidget_net;
    QWidget *tab_2;
    QFormLayout *formLayout_2;
    QTableWidget *tableWidget_dem;
    QMenuBar *menubar;
    QMenu *menu;
    QMenu *menu_2;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *QKDSim)
    {
        if (QKDSim->objectName().isEmpty())
            QKDSim->setObjectName("QKDSim");
        QKDSim->resize(884, 672);
        QFont font;
        font.setPointSize(15);
        QKDSim->setFont(font);
        action_open_net = new QAction(QKDSim);
        action_open_net->setObjectName("action_open_net");
        action_open_net->setFont(font);
        action_open_dem = new QAction(QKDSim);
        action_open_dem->setObjectName("action_open_dem");
        action_open_dem->setFont(font);
        action_save_net = new QAction(QKDSim);
        action_save_net->setObjectName("action_save_net");
        action_save_net->setFont(font);
        action_save_dem = new QAction(QKDSim);
        action_save_dem->setObjectName("action_save_dem");
        action_save_dem->setFont(font);
        action_exit = new QAction(QKDSim);
        action_exit->setObjectName("action_exit");
        action_exit->setFont(font);
        action_qt = new QAction(QKDSim);
        action_qt->setObjectName("action_qt");
        action_qt->setFont(font);
        centralwidget = new QWidget(QKDSim);
        centralwidget->setObjectName("centralwidget");
        gridLayout = new QGridLayout(centralwidget);
        gridLayout->setObjectName("gridLayout");
        tabWidget = new QTabWidget(centralwidget);
        tabWidget->setObjectName("tabWidget");
        tabWidget->setFont(font);
        tab = new QWidget();
        tab->setObjectName("tab");
        formLayout = new QFormLayout(tab);
        formLayout->setObjectName("formLayout");
        tableWidget_net = new QTableWidget(tab);
        tableWidget_net->setObjectName("tableWidget_net");
        tableWidget_net->verticalHeader()->setVisible(false);

        formLayout->setWidget(0, QFormLayout::SpanningRole, tableWidget_net);

        tabWidget->addTab(tab, QString());
        tab_2 = new QWidget();
        tab_2->setObjectName("tab_2");
        formLayout_2 = new QFormLayout(tab_2);
        formLayout_2->setObjectName("formLayout_2");
        tableWidget_dem = new QTableWidget(tab_2);
        tableWidget_dem->setObjectName("tableWidget_dem");
        tableWidget_dem->verticalHeader()->setVisible(false);

        formLayout_2->setWidget(0, QFormLayout::SpanningRole, tableWidget_dem);

        tabWidget->addTab(tab_2, QString());

        gridLayout->addWidget(tabWidget, 0, 0, 1, 1);

        QKDSim->setCentralWidget(centralwidget);
        menubar = new QMenuBar(QKDSim);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 884, 31));
        menu = new QMenu(menubar);
        menu->setObjectName("menu");
        menu_2 = new QMenu(menubar);
        menu_2->setObjectName("menu_2");
        QKDSim->setMenuBar(menubar);
        statusbar = new QStatusBar(QKDSim);
        statusbar->setObjectName("statusbar");
        QKDSim->setStatusBar(statusbar);

        menubar->addAction(menu->menuAction());
        menubar->addAction(menu_2->menuAction());
        menu->addAction(action_open_net);
        menu->addAction(action_open_dem);
        menu->addSeparator();
        menu->addAction(action_save_net);
        menu->addAction(action_save_dem);
        menu->addSeparator();
        menu->addAction(action_exit);
        menu_2->addAction(action_qt);

        retranslateUi(QKDSim);

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(QKDSim);
    } // setupUi

    void retranslateUi(QMainWindow *QKDSim)
    {
        QKDSim->setWindowTitle(QCoreApplication::translate("QKDSim", "QKDSim", nullptr));
        action_open_net->setText(QCoreApplication::translate("QKDSim", "\346\211\223\345\274\200\346\213\223\346\211\221", nullptr));
        action_open_dem->setText(QCoreApplication::translate("QKDSim", "\346\211\223\345\274\200\351\234\200\346\261\202", nullptr));
        action_save_net->setText(QCoreApplication::translate("QKDSim", "\346\213\223\346\211\221\345\217\246\345\255\230\344\270\272", nullptr));
        action_save_dem->setText(QCoreApplication::translate("QKDSim", "\351\234\200\346\261\202\345\217\246\345\255\230\344\270\272", nullptr));
        action_exit->setText(QCoreApplication::translate("QKDSim", "\351\200\200\345\207\272", nullptr));
        action_qt->setText(QCoreApplication::translate("QKDSim", "Qt", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab), QCoreApplication::translate("QKDSim", "\346\213\223\346\211\221\344\277\241\346\201\257", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_2), QCoreApplication::translate("QKDSim", "\351\234\200\346\261\202\344\277\241\346\201\257", nullptr));
        menu->setTitle(QCoreApplication::translate("QKDSim", "\346\226\207\344\273\266", nullptr));
        menu_2->setTitle(QCoreApplication::translate("QKDSim", "\345\270\256\345\212\251", nullptr));
    } // retranslateUi

};

namespace Ui {
    class QKDSim: public Ui_QKDSim {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_QKDSIM_H
