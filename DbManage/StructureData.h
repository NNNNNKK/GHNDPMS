#pragma once
#include <QString>
#include <QJsonObject>
#include <list>
#include <QVariant>
#include <memory>

const int OP_NOTHING = -1;				//不读取任何数据，默认值
const int READ_ALLORG = 1;					//读取所有的部门信息
const int READ_AllORGINDEXCODE = 2;			//读取所有的部门唯一标识码
const int READ_ALLORGEMPLOYEES = 3;			//读取所有部门的所有员工信息
const int READ_ORGBLACK = 4;				//读取部门名单列表
const int READ_ACCOUNT = 100;				//读取账号
const int READ_ORGDETAILHISTORIYDATA = 101; //读取部门历史详细数据

const int WRITE_ORGMSG = 200;				//写部门数据
const int WRITE_OK = 201;					//写成功
const int WRITE_EMPLOYEEMSG = 202;			//写员工数据

/*****************************错误码定义*********************************/
const int READ_OK = 3000;			//读取成功
const int READ_DATA_PTR_NULL = 3001;//读数据指针为空
const int FUNCTION_EXCEPTION = 3002;//函数内部异常
const int DB_EXCEPTION = 3003;		//数据库异常

//操作的数据类型，基本数据，实时数据，历史数据
enum GHND_DbDataOpType { Basic, RealTime, Historical };

using READTYPE = unsigned int;
//员工
struct Employee
{
	std::string			certificateNo = "";						//验证编号
	int					certificateType = 111;					//验证类型
	int					gender = 0;								//性别
	std::string			jobNo = "";								//工号
	std::string			orgIndexCode = "";						//部门唯一标识码
	std::string			orgName = "";							//部门名称
	std::string			personId = "";							//员工唯一Id
	std::string			personName = "";						//员工姓名
	std::string			phoneNo = "";							//手机号
	std::string			pinyin = "";							//姓名拼音
};

struct Organization
{
	std::string										orgIndexCode = "";						//部门唯一标识号
	std::string										orgName = "";							//部门名称
	std::string										orgNo = "";								//部门编号
	std::string										orgPath = "";							//部门路径
	std::string										parentOrgName = "";						//父部门名称
	bool											isModify = false;						//是否修改过
	bool											islocal = true;							//是否为本单位组织
	int												orgEmployeesNumber = 0;					//部门人数
	std::list<std::shared_ptr<Employee>>			employeelist;							//员工列表 指针类型
};

//读数据库结构体
struct GHND_RWData
{
	std::shared_ptr<QVariant> plDataPtr;				//数据指针--数据结构体读取
	std::shared_ptr<QVariant> plRequireData;				//请求条件指针
	int nReadOpState= OP_NOTHING;						//读取操作宏定义

	long nResult;										//返回结果定义

	GHND_RWData()
	{
	};
	GHND_RWData(int nReadOpState)
	{
		this->nReadOpState = nReadOpState;
	};
};
