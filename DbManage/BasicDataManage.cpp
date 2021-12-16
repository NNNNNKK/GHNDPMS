#include "BasicDataManage.h"
#include "ConnectionPool.h"
#include "StructureData.h"
#include <algorithm>
#include <map>

BasicDataManage::BasicDataManage()
{
	initSelectFunc();
}

//初始化查询函数map
void BasicDataManage::initSelectFunc()
{
	sSp[READ_ALLORG] = std::bind(&BasicDataManage::queryOrg, 
		this,std::placeholders::_1);
	sSp[READ_ALLORGEMPLOYEES] = std::bind(&BasicDataManage::queryAllEmployees,
		this, std::placeholders::_1); 
	sSp[READ_AllORGINDEXCODE] = std::bind(&BasicDataManage::queryAllOrgIndexCode,
		this, std::placeholders::_1);
}

//读取数据
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
		//1 查询所有的员工信息
		std::shared_ptr<GHND_ReadData> spCode(new GHND_ReadData(READ_ALLORGEMPLOYEES));
		queryAllEmployees(spCode);
		//查询状态判断及其处理
		if (!spCode->plDataPtr || READ_OK != spCode->nResult)
		{
			ConnectionPool::closeConnection(db);
			return;
		}
		auto allEmployees = spCode->plDataPtr->toJsonObject();//每个部门的所有的员工 
		//2 查询部门信息
		QJsonObject pObj;//父节点
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
		//3.结果集返回
		sp->plDataPtr = std::make_shared<QVariant>(pObj);
		sp->nResult = READ_OK;
		//4.资源释放
		ConnectionPool::closeConnection(db);
	}
	catch (...)
	{
		sp->nResult = FUNCTION_EXCEPTION;
		ConnectionPool::closeConnection(db);
	}
}

//查询指定部分的员工
void BasicDataManage::queryAllEmployees(std::shared_ptr<GHND_ReadData> sp)
{
	if (!sp|| READ_ALLORGEMPLOYEES != sp->nReadOpState)
	{
		return;
	}

	auto db = ConnectionPool::openConnection();//获取数据库连接
	try
	{
		QSqlQuery query(db);
		//步骤1 获取所有的部门标识符
		std::shared_ptr<GHND_ReadData> spCode(new GHND_ReadData(READ_AllORGINDEXCODE));
		queryAllOrgIndexCode(spCode);
		//查询出错，返回
		if (!spCode->plDataPtr||READ_OK != spCode->nResult)
		{
			ConnectionPool::closeConnection(db);
			return;
		}
		auto allOrgIndexCode = spCode->plDataPtr->toJsonArray();//获取所有的员工信息
		QJsonObject allOrgEms;//所有的员工数
		//步骤2 查询每个部门的员工信息
		for (auto it = allOrgIndexCode.constBegin();it!= allOrgIndexCode.constEnd();++it)
		{
			const QString str = (*it).toString();//解析部门编码
			
			QString sqlStr = tr(" select * from [GHNDDB].[dbo].[Employee] where orgIndexCode = '%1'").arg(str);
			query.prepare(sqlStr);
			if (query.exec())
			{
				QJsonArray singleOrgEm;//单个部门的员工列表
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
		//结果集返回
		sp->plDataPtr = std::make_shared<QVariant>(allOrgEms);
		sp->nResult = READ_OK;
		//释放资源
		ConnectionPool::closeConnection(db);
	}
	catch (...)
	{
		//出现异常释放资源
		ConnectionPool::closeConnection(db);
		sp->nResult = FUNCTION_EXCEPTION;
	}
}

//查询所有的部门编码
void  BasicDataManage::queryAllOrgIndexCode(std::shared_ptr<GHND_ReadData> & sp)
{
	if (!sp || READ_AllORGINDEXCODE != sp->nReadOpState)
	{
		return;
	}
	auto db = ConnectionPool::openConnection();//获取数据库连接
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
		//将结果集返回
		sp->plDataPtr = std::make_shared<QVariant>(allIndexCodes);
		sp->nResult = READ_OK;
		//释放资源
		ConnectionPool::closeConnection(db);
	}
	catch (...)
	{
		//异常资源释放
		ConnectionPool::closeConnection(db);
		sp->nResult = FUNCTION_EXCEPTION;
	}
}

