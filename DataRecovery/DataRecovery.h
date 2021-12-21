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
	//��json�ĵ�
	QVariant  openJsonDoc(const QString &fileName);
	//������д�뵽���ݿ�
	void writeJsonToDb(const QString & fileName);
	//�ָ���ʷ��ѯ����
	void recoveryHistory();
	//����ѹ��
	QByteArray Compress(QByteArray postBody);
	//���ݽ�ѹ
	QByteArray UnCompress(QByteArray src);

public slots:
	void on_btnDataRecovery_clicked();
private:
    Ui::DataRecoveryClass ui;
};
