
#pragma once
#include <QObject>
#include <memory>
#include <QSqlDatabase>

struct GHND_RWData;

class DbOperationBase:public QObject
{
	Q_OBJECT
public:
	DbOperationBase(){};
	virtual ~DbOperationBase() {};
	//���ݿ����
	virtual void OpDataBase(std::shared_ptr<GHND_RWData> sp) = 0;
protected:
	QSqlDatabase db;//���ݿ����ָ��
};