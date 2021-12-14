#pragma once

#include <QtWidgets/QMainWindow>
#include <QVariant>
#include "ui_DataRecovery.h"

class DataRecovery : public QMainWindow
{
    Q_OBJECT

public:
    DataRecovery(QWidget *parent = Q_NULLPTR);
	//打开json文档
	QVariant  openJsonDoc(const QString &fileName);
	//将数据写入到数据库
	void writeJsonToDb(const QString & fileName);
public slots:
	void on_btnDataRecovery_clicked();
private:
    Ui::DataRecoveryClass ui;
};
