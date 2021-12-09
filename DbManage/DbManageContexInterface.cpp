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

void DbManageContexInterface::getReadDbData(GHND_ReadData * const pRd)
{
	return dbBase->readDataBase(pRd);
}
