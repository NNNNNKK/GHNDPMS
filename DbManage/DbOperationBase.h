//���ݿ��������
#pragma once
#include <QObject>

struct GHND_ReadData;

class DbOperationBase:public QObject
{
	Q_OBJECT
public:
	DbOperationBase(){};
	virtual ~DbOperationBase() {};
	//�����ݿ����
	virtual void readDataBase(GHND_ReadData * const plReadDataPtr) = 0;
	//д���ݲ���
	virtual bool writeDataBase(const QVariant & var) = 0;
};