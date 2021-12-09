//数据库操作基类
#pragma once
#include <QObject>

struct GHND_ReadData;

class DbOperationBase:public QObject
{
	Q_OBJECT
public:
	DbOperationBase(){};
	virtual ~DbOperationBase() {};
	//读数据库操作
	virtual void readDataBase(GHND_ReadData * const plReadDataPtr) = 0;
	//写数据操作
	virtual bool writeDataBase(const QVariant & var) = 0;
};