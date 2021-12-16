
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
	//读数据库操作
	virtual void readDataBase(std::shared_ptr<GHND_ReadData> sp) = 0;
	//写数据操作
	virtual bool writeDataBase(const QVariant & var) = 0;
};