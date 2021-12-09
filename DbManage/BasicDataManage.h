/*********************************************************************
* 文 件 名: BasicDataManage.h
* 创 建 者: 马彦军
* 创建日期: 2021.11.22
* 修    改:
* 功    能: 基本数据处理接口类
*********************************************************************/
#pragma once
#include <qobject.h>
#include <qjsonobject.h>
#include "DbOperationBase.h"
#include <string>
#include <memory>

//读取数据结构体
struct GHND_ReadData;
struct Employee;
struct Organization;

class BasicDataManage :public DbOperationBase
{
	Q_OBJECT
public:
	explicit BasicDataManage();
	~BasicDataManage() {};
	
	virtual void readDataBase(GHND_ReadData * const plReadDataPtr);
	virtual bool writeDataBase(const QVariant & var) override;

	//写部门数据
	bool writeOrganization();
	//查询部门信息
	std::map<std::string, Organization> * queryOrg();
	//查询所有的员工数目
	const std::map<std::string,std::list<std::shared_ptr<Employee>>>  queryAllEmployees();
	//查询所有部门编号
	const std::list<std::string>  queryAllOrgIndexCode();
};