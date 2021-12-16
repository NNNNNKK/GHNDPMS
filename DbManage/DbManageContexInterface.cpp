#include "DbManageContexInterface.h"
#include "StructureData.h"
#include "BasicDataManage.h"
#include <QVariant>

DbManageContexInterface::DbManageContexInterface(GHND_DbDataOpType type) :
	dbBase(nullptr)
{
	switch (type)
	{
	case Basic:
		dbBase = new BasicDataManage();
		break;
	case RealTime:
		break;
	case Historical:
		break;
	default:
		break;
	}
}

DbManageContexInterface::~DbManageContexInterface()
{
	if (dbBase)
	{
		delete dbBase;
		dbBase = nullptr;
	}
}

//返回从数据库中读取的数据
void DbManageContexInterface::getReadDbData(std::shared_ptr<GHND_ReadData> sp)
{
	return dbBase->readDataBase(sp);
}

