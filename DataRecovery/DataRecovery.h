#pragma once

#include <QtWidgets/QMainWindow>
#include <QVariant>
#include "ui_DataRecovery.h"
#include <QtZlib/zlib.h>
#include <QByteArray>

class DataRecovery : public QMainWindow
{
    Q_OBJECT

public:
    DataRecovery(QWidget *parent = Q_NULLPTR);
	//打开json文档
	QVariant  openJsonDoc(const QString &fileName);
	//将数据写入到数据库
	void writeJsonToDb(const QString & fileName);
	//恢复历史查询数据
	void recoveryHistory();
	//数据压缩
	QByteArray Compress(QByteArray postBody);
	//数据解压
	QByteArray UnCompress(QByteArray src);

public slots:
	void on_btnDataRecovery_clicked();
private:
    Ui::DataRecoveryClass ui;
};
