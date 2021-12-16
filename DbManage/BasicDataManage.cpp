#include "BasicDataManage.h"
#include "ConnectionPool.h"
#include "StructureData.h"
#include <algorithm>
#include <map>

BasicDataManage::BasicDataManage()
{
	initSelectFunc();
}

//��ʼ����ѯ����map
void BasicDataManage::initSelectFunc()
{
	sSp[READ_ALLORG] = std::bind(&BasicDataManage::queryOrg, 
		this,std::placeholders::_1);
	sSp[READ_ALLORGEMPLOYEES] = std::bind(&BasicDataManage::queryAllEmployees,
		this, std::placeholders::_1); 
	sSp[READ_AllORGINDEXCODE] = std::bind(&BasicDataManage::queryAllOrgIndexCode,
		this, std::placeholders::_1);
}

//��ȡ����
void BasicDataManage::readDataBase(std::shared_ptr<GHND_ReadData> sp)
{
	if (!sp || READ_NOTHING==sp->nReadOpState)
	{
		return;
	}
	auto res = sSp.find(sp->nReadOpState);
	if (res != sSp.end())
	{
		SelectFunction f = res->second;
		f(sp);
	}
}

bool BasicDataManage::writeDataBase(const QVariant & var)
{
	return false;
}


bool BasicDataManage::writeOrganization()
{

	return false;
}

void BasicDataManage::queryOrg(std::shared_ptr<GHND_ReadData> sp)
{
	if (!sp || READ_ALLORG != sp->nReadOpState)
	{
		return;
	}

	auto db = ConnectionPool::openConnection();
	try
	{
		//1 ��ѯ���е�Ա����Ϣ
		std::shared_ptr<GHND_ReadData> spCode(new GHND_ReadData(READ_ALLORGEMPLOYEES));
		queryAllEmployees(spCode);
		//��ѯ״̬�жϼ��䴦��
		if (!spCode->plDataPtr || READ_OK != spCode->nResult)
		{
			ConnectionPool::closeConnection(db);
			return;
		}
		auto allEmployees = spCode->plDataPtr->toJsonObject();//ÿ�����ŵ����е�Ա�� 
		//2 ��ѯ������Ϣ
		QJsonObject pObj;//���ڵ�
		QSqlQuery query(db);
		query.prepare(QString::fromLocal8Bit("select * FROM [GHNDDB].[dbo].[Department]"));
		if (query.exec())
		{
			qDebug("queryOrg success!");
			while (query.next())
			{
				QString orgIndex = query.value("orgIndexCode").toString();
				QJsonObject subObj
				{
					{"employeelist",allEmployees[orgIndex]},
					{"isModify",query.value("isModify").toBool()},
					{"islocal",query.value("islocal").toBool()},
					{"orgEmployeesNumber",query.value("orgEmployeesNumber").toInt()},
					{"orgName",query.value("orgName").toString()},
				};
				pObj[orgIndex] = subObj;
			}
		}
		//3.���������
		sp->plDataPtr = std::make_shared<QVariant>(pObj);
		sp->nResult = READ_OK;
		//4.��Դ�ͷ�
		ConnectionPool::closeConnection(db);
	}
	catch (...)
	{
		sp->nResult = FUNCTION_EXCEPTION;
		ConnectionPool::closeConnection(db);
	}
}

//��ѯָ�����ֵ�Ա��
void BasicDataManage::queryAllEmployees(std::shared_ptr<GHND_ReadData> sp)
{
	if (!sp|| READ_ALLORGEMPLOYEES != sp->nReadOpState)
	{
		return;
	}

	auto db = ConnectionPool::openConnection();//��ȡ���ݿ�����
	try
	{
		QSqlQuery query(db);
		//����1 ��ȡ���еĲ��ű�ʶ��
		std::shared_ptr<GHND_ReadData> spCode(new GHND_ReadData(READ_AllORGINDEXCODE));
		queryAllOrgIndexCode(spCode);
		//��ѯ��������
		if (!spCode->plDataPtr||READ_OK != spCode->nResult)
		{
			ConnectionPool::closeConnection(db);
			return;
		}
		auto allOrgIndexCode = spCode->plDataPtr->toJsonArray();//��ȡ���е�Ա����Ϣ
		QJsonObject allOrgEms;//���е�Ա����
		//����2 ��ѯÿ�����ŵ�Ա����Ϣ
		for (auto it = allOrgIndexCode.constBegin();it!= allOrgIndexCode.constEnd();++it)
		{
			const QString str = (*it).toString();//�������ű���
			
			QString sqlStr = tr(" select * from [GHNDDB].[dbo].[Employee] where orgIndexCode = '%1'").arg(str);
			query.prepare(sqlStr);
			if (query.exec())
			{
				QJsonArray singleOrgEm;//�������ŵ�Ա���б�
				while (query.next())
				{
					qDebug("queryAllEmployees success!");
					QJsonObject em
					{
						{"personId" ,query.value("personId").toString()},
						{"certificateNo",query.value("certificateNo").toString()},
						{"certificateType", query.value("certificateType").toInt()},
						{"gender", query.value("gender").toInt()},
						{"jobNo",  query.value("jobNo").toString()},
						{"orgIndexCode",  query.value("orgIndexCode").toString()},
						{"orgName",query.value("orgName").toString()},
						{"personName", query.value("personName").toString()},
						{"phoneNo", query.value("phoneNo").toString()},
						{"pinyin",  query.value("pinyin").toString()}
					};
					singleOrgEm.push_back(em);
				}
				allOrgEms[str] = singleOrgEm;
			}
		};
		//���������
		sp->plDataPtr = std::make_shared<QVariant>(allOrgEms);
		sp->nResult = READ_OK;
		//�ͷ���Դ
		ConnectionPool::closeConnection(db);
	}
	catch (...)
	{
		//�����쳣�ͷ���Դ
		ConnectionPool::closeConnection(db);
		sp->nResult = FUNCTION_EXCEPTION;
	}
}

//��ѯ���еĲ��ű���
void  BasicDataManage::queryAllOrgIndexCode(std::shared_ptr<GHND_ReadData> & sp)
{
	if (!sp || READ_AllORGINDEXCODE != sp->nReadOpState)
	{
		return;
	}
	auto db = ConnectionPool::openConnection();//��ȡ���ݿ�����
	try
	{
		QSqlQuery query(db);
		QJsonArray allIndexCodes;
		query.prepare("SELECT orgIndexCode from [GHNDDB].[dbo].[Employee] group by orgIndexCode");
		if (query.exec())
		{
			while (query.next())
			{
				allIndexCodes.push_back(query.value("orgIndexCode").toString());
			}
		}
		//�����������
		sp->plDataPtr = std::make_shared<QVariant>(allIndexCodes);
		sp->nResult = READ_OK;
		//�ͷ���Դ
		ConnectionPool::closeConnection(db);
	}
	catch (...)
	{
		//�쳣��Դ�ͷ�
		ConnectionPool::closeConnection(db);
		sp->nResult = FUNCTION_EXCEPTION;
	}
}

