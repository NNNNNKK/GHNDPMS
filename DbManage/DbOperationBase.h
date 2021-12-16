
#pragma once
#include <QObject>
#include <memory>
struct GHND_ReadData;

class DbOperationBase:public QObject
{
	Q_OBJECT
public:
	DbOperationBase(){};
	virtual ~DbOperationBase() {};
	//�����ݿ����
	virtual void readDataBase(std::shared_ptr<GHND_ReadData> sp) = 0;
	//д���ݲ���
	virtual bool writeDataBase(const QVariant & var) = 0;
};