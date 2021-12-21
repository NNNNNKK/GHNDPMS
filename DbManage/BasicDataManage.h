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
#include <vector>
#include <functional>
#include <QSqlDatabase>
//读取数据结构体
struct GHND_RWData;
struct Employee;
struct Organization;

class BasicDataManage :public DbOperationBase
{
	Q_OBJECT
public:
	explicit BasicDataManage();
	~BasicDataManage();
	
	virtual void OpDataBase(std::shared_ptr<GHND_RWData> sp) override;

	//写部门数据
	void writeOrgMsg(std::shared_ptr<GHND_RWData> sp);
	//写员工数据
	void writeEmployeeMsg(std::shared_ptr<GHND_RWData> sp);
	//查询部门信息
	void queryOrg(std::shared_ptr<GHND_RWData> sp);
	//查询所有的员工数目
	void queryAllEmployees(std::shared_ptr<GHND_RWData> sp);
	//查询所有部门编号
	void queryAllOrgIndexCode(std::shared_ptr<GHND_RWData> &sp);
	//查询部门黑名单
	void queryOrgBlack(std::shared_ptr<GHND_RWData> sp);
private:
	using SelectFunction = std::function<void(std::shared_ptr<GHND_RWData>)>;//查询函数
	using SelectPair=std::map<int, SelectFunction>;//根据相关的整数值，执行相关的函数
	SelectPair sSp;//查询对
protected:
	//初始化查询函数容器
	void initSelectFunc();
};