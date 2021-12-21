/********************************************************************************
** Form generated from reading UI file 'DataRecovery.ui'
**
** Created by: Qt User Interface Compiler version 5.12.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DATARECOVERY_H
#define UI_DATARECOVERY_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_DataRecoveryClass
{
public:
    QWidget *centralWidget;
    QPushButton *btnDataRecovery;
    QPushButton *btnDataBk;

    void setupUi(QMainWindow *DataRecoveryClass)
    {
        if (DataRecoveryClass->objectName().isEmpty())
            DataRecoveryClass->setObjectName(QString::fromUtf8("DataRecoveryClass"));
        DataRecoveryClass->resize(412, 120);
        centralWidget = new QWidget(DataRecoveryClass);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        btnDataRecovery = new QPushButton(centralWidget);
        btnDataRecovery->setObjectName(QString::fromUtf8("btnDataRecovery"));
        btnDataRecovery->setGeometry(QRect(50, 40, 93, 28));
        btnDataBk = new QPushButton(centralWidget);
        btnDataBk->setObjectName(QString::fromUtf8("btnDataBk"));
        btnDataBk->setGeometry(QRect(250, 40, 93, 28));
        DataRecoveryClass->setCentralWidget(centralWidget);

        retranslateUi(DataRecoveryClass);

        QMetaObject::connectSlotsByName(DataRecoveryClass);
    } // setupUi

    void retranslateUi(QMainWindow *DataRecoveryClass)
    {
        DataRecoveryClass->setWindowTitle(QApplication::translate("DataRecoveryClass", "DataRecovery", nullptr));
        btnDataRecovery->setText(QApplication::translate("DataRecoveryClass", "\346\225\260\346\215\256\346\201\242\345\244\215", nullptr));
        btnDataBk->setText(QApplication::translate("DataRecoveryClass", "\346\225\260\346\215\256\345\244\207\344\273\275", nullptr));
    } // retranslateUi

};

namespace Ui {
    class DataRecoveryClass: public Ui_DataRecoveryClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DATARECOVERY_H
