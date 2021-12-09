#include "BasicDataManage.h"
#include "ConnectionPool.h"
#include "StructureData.h"
#include <algorithm>
#include <map>
using namespace std;
BasicDataManage::BasicDataManage()
{

}

void BasicDataManage::readDataBase(GHND_ReadData *  const plReadDataPtr)
{
	if (plReadDataPtr)
	{
		switch (plReadDataPtr->nReadOpState)
		{
			case READ_ALLORG:
			{
				if (nullptr == plReadDataPtr->plDataPtr)
				{
					plReadDataPtr->nResult = READ_DATA_PTR_NULL;
					return;
				}
				plReadDataPtr->plDataPtr = reinterpret_cast<long*>(queryOrg());
				plReadDataPtr->nResult = READ_OK;
				break;
			}
			case READ_AllORGINDEXCODE://��ȡ���в��ŵ�Ψһ��ʶ��
			{
				if (nullptr == plReadDataPtr->plDataPtr)
				{
					plReadDataPtr->nResult = READ_DATA_PTR_NULL;
					return;
				}
				auto allOrgCode = queryAllOrgIndexCode();
				auto temp = reinterpret_cast<std::list<std::string> *>(plReadDataPtr->plDataPtr);
				temp->resize(allOrgCode.size());//�������Ĵ�С����Ϊ���غ�����ݵĴ�С
				std::copy(allOrgCode.cbegin(), allOrgCode.cend(), temp->begin());//���ݿ���
				break;
			}
			default:
				break;
		}
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

std::map<std::string, Organization> * BasicDataManage::queryOrg()
{
	auto db = ConnectionPool::openConnection();//��ȡ���ݿ�����
	std::map<std::string, Organization> *allRobj =new std::map<std::string, Organization>();//���صĲ���json����
	try
	{
		QSqlQuery query(db);
		auto allEmployees = queryAllEmployees();//��ѯÿ�����ŵ����е�Ա�� 
		query.prepare(QString::fromLocal8Bit("select * FROM [GHNDDB].[dbo].[Department]"));
		if (query.exec())
		{
			qDebug("queryOrg success!");
			while (query.next())
			{
				std::string orgIndex = query.value("orgIndexCode").toString().toStdString();
				Organization orgObj;
				orgObj.isModify = query.value("isModify").toBool();
				orgObj.islocal = query.value("islocal").toBool();
				orgObj.islocal = query.value("islocal").toBool();
				orgObj.orgEmployeesNumber = query.value("orgEmployeesNumber").toInt();
				orgObj.orgName = query.value("orgName").toString().toStdString();
				orgObj.employeelist = allEmployees["allEmployees"];//Ա������

				allRobj->emplace(orgIndex, std::move(orgObj));
			}
		}
		ConnectionPool::closeConnection(db);
		return std::move(allRobj);
	}
	catch (...)
	{
		ConnectionPool::closeConnection(db);
		return std::move(allRobj);
	}
}

//��ѯָ�����ֵ�Ա��
const std::map<std::string, std::list<std::shared_ptr<Employee>>>  BasicDataManage::queryAllEmployees()
{
	auto db = ConnectionPool::openConnection();//��ȡ���ݿ�����
	std::map<std::string, std::list<std::shared_ptr<Employee>>> employees;//���е�Ա����Ŀ
	try
	{
		QSqlQuery query(db);
		// step 1. get all orgIndexCode
		auto allOrgIndexCode = queryAllOrgIndexCode();
		//step 2. query every org employee
		for (auto it = allOrgIndexCode.cbegin();it!= allOrgIndexCode.cend();++it)
		{
			const QString str = QString::fromStdString(*it);//�������ű���

			QString sqlStr = tr(" select * from [GHNDDB].[dbo].[Employee] where orgIndexCode = '%1'").arg(str);
			query.prepare(sqlStr);
			if (query.exec())
			{
				std::list<std::shared_ptr<Employee>> curOrgEmployee;//��ǰ���ŵ���Ա�б�
				while (query.next())
				{
					qDebug("queryAllEmployees success!");
					std::shared_ptr<Employee> employee(new Employee);
					std::string personId = query.value("personId").toString().toStdString();
					employee->certificateNo = query.value("certificateNo").toString().toStdString();
					employee->certificateType = query.value("certificateType").toInt();
					employee->gender = query.value("gender").toInt();
					employee->jobNo = query.value("jobNo").toString().toStdString();
					employee->orgIndexCode = query.value("orgIndexCode").toString().toStdString();
					employee->orgName = query.value("orgName").toString().toStdString();
					employee->personName = query.value("personName").toString().toStdString();
					employee->phoneNo = query.value("phoneNo").toString().toStdString();
					employee->pinyin = query.value("pinyin").toString().toStdString();
					employee->personId = personId;

					curOrgEmployee.emplace_back(employee);
				}
				employees.emplace(*it, curOrgEmployee);
			}
		};
		ConnectionPool::closeConnection(db);//�ر�����
		return std::move(employees);
	}
	catch (...)
	{
		//�����쳣�ͷ���Դ
		ConnectionPool::closeConnection(db);//�ر�����
		return std::move(employees);
	}
}

//��ѯ���еĲ��ű���
const std::list<std::string>  BasicDataManage::queryAllOrgIndexCode()
{
	auto db = ConnectionPool::openConnection();//��ȡ���ݿ�����
	std::list<std::string> allOrgIndexCode;
	try
	{
		QSqlQuery query(db);
		query.prepare("SELECT orgIndexCode from [GHNDDB].[dbo].[Employee] group by orgIndexCode");
		if (query.exec())
		{
			while (query.next())
			{
				allOrgIndexCode.emplace_back(query.value("orgIndexCode").toString().toStdString());
			}
		}
		//�ͷ���Դ
		ConnectionPool::closeConnection(db);
		return std::move(allOrgIndexCode);
	}
	catch (...)
	{
		ConnectionPool::closeConnection(db);
		return std::move(allOrgIndexCode);
	}
}
