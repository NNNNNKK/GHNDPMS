#pragma once

#include <QtWidgets/QMainWindow>
#include <QVariant>
#include "ui_DataRecovery.h"

class DataRecovery : public QMainWindow
{
    Q_OBJECT

public:
    DataRecovery(QWidget *parent = Q_NULLPTR);
	//��json�ĵ�
	QVariant  openJsonDoc(const QString &fileName);
	//������д�뵽���ݿ�
	void writeJsonToDb(const QString & fileName);
public slots:
	void on_btnDataRecovery_clicked();
private:
    Ui::DataRecoveryClass ui;
};
