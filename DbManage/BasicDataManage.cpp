#include "BasicDataManage.h"
#include "ConnectionPool.h"
#include "StructureData.h"
#include <algorithm>
#include <map>

BasicDataManage::BasicDataManage()
{
	db = ConnectionPool::openConnection();
}

BasicDataManage::~BasicDataManage()
{
	ConnectionPool::closeConnection(db);
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
	sSp[READ_ORGBLACK] = std::bind(&BasicDataManage::queryOrgBlack,
		this, std::placeholders::_1);
	sSp[WRITE_ORGMSG] = std::bind(&BasicDataManage::writeOrgMsg,
		this, std::placeholders::_1);
	sSp[WRITE_EMPLOYEEMSG] = std::bind(&BasicDataManage::writeEmployeeMsg,
		this, std::placeholders::_1);
}

//读取数据
void BasicDataManage::OpDataBase(std::shared_ptr<GHND_RWData> sp)
{
	if (!sp || OP_NOTHING == sp->nReadOpState)
	{
		return;
	}
	//初始化查询map
	if (0 == sSp.size())
	{
		sSp.clear();
		initSelectFunc();
	}
	
	auto res = sSp.find(sp->nReadOpState);
	if (res != sSp.end())
	{
		SelectFunction f = res->second;
		if (f)
		{
			f(sp);
		}
	}
}


void BasicDataManage::writeOrgMsg(std::shared_ptr<GHND_RWData> sp)
{
	if (!sp||!sp->plDataPtr||WRITE_ORGMSG!=sp->nReadOpState||!db.isOpen())
	{
		return;
	}
	try
	{
		auto orgArray = sp->plDataPtr->toJsonArray();
		for (auto it=orgArray.constBegin();it!=orgArray.constEnd();++it)
		{
			QJsonObject orgMsg = (*it).toObject();
			QString orgCode = orgMsg["orgIndexCode"].toString();
			QString orgName = orgMsg["orgName"].toString();
			QString orgNo = orgMsg["orgNo"].toString();
			QString orgPath = orgMsg["orgPath"].toString();
			int orgEmployessNumber = orgMsg["orgEmployeesNumber"].toInt();
			int isModify = orgMsg["isModify"].toInt();
			int islocal = orgMsg["islocal"].toInt();

			QSqlQuery query(db);
			query.prepare("exec GHNDDB.dbo.GHND_SaveDepartment ?,?,?,?,?,?,?");
			query.bindValue(0, orgCode);
			query.bindValue(1, orgName);
			query.bindValue(2, orgEmployessNumber);
			query.bindValue(3, islocal);
			query.bindValue(4, orgNo);
			query.bindValue(5, orgPath);
			query.bindValue(6, "");
			query.bindValue(7, 0, QSql::Out);
			if (query.exec())
			{
				if (1 != query.boundValue(7).toInt())
				{
					sp->nResult = DB_EXCEPTION;
					return;
				}
			}
			else
			{
				sp->nResult = DB_EXCEPTION;
				qDebug("Write Database error:%s",qPrintable(query.lastError().text()));
				return;
			}
		}
		sp->nResult = WRITE_OK;
	}
	catch (...)
	{
		sp->nResult= FUNCTION_EXCEPTION;
	}
}

void BasicDataManage::writeEmployeeMsg(std::shared_ptr<GHND_RWData> sp)
{
	if (!sp||WRITE_EMPLOYEEMSG!=sp->nReadOpState||!sp->plDataPtr|| !db.isOpen())
	{
		return;
	}

	try
	{
		QJsonArray employeeArray = sp->plDataPtr->toJsonArray();
		for (auto it=employeeArray.constBegin();it!=employeeArray.constEnd();++it)
		{
			QJsonObject singleStaff = (*it).toObject();
			QString certificateNo = singleStaff["certificateNo"].toString();
			int certificateType = singleStaff["certificateType"].toInt();
			QChar gender = singleStaff["gender"].toInt();
			QString jobNo = singleStaff["jobNo"].toString();
			QString orgIndexCode = singleStaff["orgIndexCode"].toString();
			QString orgName = singleStaff["orgName"].toString();
			QString personId = singleStaff["personId"].toString();
			QString personName = singleStaff["personName"].toString();
			QString phoneNo = singleStaff["phoneNo"].toString();
			QString pinyin = singleStaff["pinyin"].toString();

			QSqlQuery query(db);
			query.prepare("exec GHNDDB.dbo.GHND_Employee ?,?,?,?,?,?,?,?,?,?,?");
			query.bindValue(0, personId);
			query.bindValue(1, personName);
			query.bindValue(2, pinyin);
			query.bindValue(3, gender);
			query.bindValue(4, phoneNo);
			query.bindValue(5, jobNo);
			query.bindValue(6, orgIndexCode);
			query.bindValue(7, orgName);
			query.bindValue(8, certificateNo);
			query.bindValue(9, certificateType);
			query.bindValue(10, "");

			if (!query.exec())
			{
				sp->nResult = DB_EXCEPTION;
				return;
			}
		}
		sp->nResult = WRITE_OK;
	}
	catch (...)
	{
		sp->nResult = FUNCTION_EXCEPTION;
	}
}

void BasicDataManage::queryOrg(std::shared_ptr<GHND_RWData> sp)
{
	if (!sp || READ_ALLORG != sp->nReadOpState|| !db.isOpen())
	{
		return;
	}

	try
	{
		//1 查询所有的员工信息
		std::shared_ptr<GHND_RWData> spCode(new GHND_RWData(READ_ALLORGEMPLOYEES));
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
					{"orgIndexCode",query.value("orgIndexCode").toString()}
				};
				pObj[orgIndex] = subObj;
			}
		}
		//3.结果集返回
		sp->plDataPtr = std::make_shared<QVariant>(pObj);
		sp->nResult = READ_OK;
	}
	catch (...)
	{
		sp->nResult = FUNCTION_EXCEPTION;
	}
}

//查询指定部分的员工
void BasicDataManage::queryAllEmployees(std::shared_ptr<GHND_RWData> sp)
{
	if (!sp|| READ_ALLORGEMPLOYEES != sp->nReadOpState)
	{
		return;
	}

	try
	{
		QSqlQuery query(db);
		//步骤1 获取所有的部门标识符
		std::shared_ptr<GHND_RWData> spCode(new GHND_RWData(READ_AllORGINDEXCODE));
		queryAllOrgIndexCode(spCode);
		//查询出错，返回
		if (!spCode->plDataPtr||READ_OK != spCode->nResult)
		{
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
	}
	catch (...)
	{
		sp->nResult = FUNCTION_EXCEPTION;
	}
}

//查询所有的部门编码
void  BasicDataManage::queryAllOrgIndexCode(std::shared_ptr<GHND_RWData> & sp)
{
	if (!sp || READ_AllORGINDEXCODE != sp->nReadOpState|| !db.isOpen())
	{
		return;
	}

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
	}
	catch (...)
	{
		sp->nResult = FUNCTION_EXCEPTION;
	}
}

void BasicDataManage::queryOrgBlack(std::shared_ptr<GHND_RWData> sp)
{
	if (!sp || READ_ORGBLACK!=sp->nReadOpState|| !db.isOpen())
	{
		return;
	}
	try
	{
		QSqlQuery query(db);
		QJsonArray orgBlackList;
		query.prepare("SELECT orgIndexCode from [GHNDDB].[dbo].[OrgBlack]");
		if (query.exec())
		{
			while (query.next())
			{
				orgBlackList.push_back(query.value("name").toString());
			}
		}
		//将结果集返回
		sp->plDataPtr = std::make_shared<QVariant>(orgBlackList);
		sp->nResult = READ_OK;
	}
	catch (...)
	{
		sp->nResult = FUNCTION_EXCEPTION;
	}
}

