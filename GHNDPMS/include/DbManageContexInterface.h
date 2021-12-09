#pragma once

#include <QObject>
class DbOperationBase;			//数据库操作接口
struct GHND_ReadData;			//读取数据结构体
enum GHND_DbDataOpType;			//操作的数据类型，基本数据，实时数据，历史数据

class  DbManageContexInterface:public QObject
{
public:
	//type为操作数据库的类型，读取基本数据，实时数据以及历史数据
	DbManageContexInterface(GHND_DbDataOpType type);
	~DbManageContexInterface();

	//返回读取的数据类型
	void getReadDbData(GHND_ReadData * const pRd);
private:
	DbOperationBase * dbBase;
};
