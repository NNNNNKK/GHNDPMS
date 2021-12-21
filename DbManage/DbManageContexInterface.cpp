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

//���ش����ݿ��ж�ȡ������
void DbManageContexInterface::OpDbData(std::shared_ptr<GHND_RWData> sp)
{
	return dbBase->OpDataBase(sp);
}

