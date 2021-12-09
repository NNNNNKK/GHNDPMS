/*********************************************************************
* �� �� ��: BasicDataManage.h
* �� �� ��: �����
* ��������: 2021.11.22
* ��    ��:
* ��    ��: �������ݴ���ӿ���
*********************************************************************/
#pragma once
#include <qobject.h>
#include <qjsonobject.h>
#include "DbOperationBase.h"
#include <string>
#include <memory>

//��ȡ���ݽṹ��
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

	//д��������
	bool writeOrganization();
	//��ѯ������Ϣ
	std::map<std::string, Organization> * queryOrg();
	//��ѯ���е�Ա����Ŀ
	const std::map<std::string,std::list<std::shared_ptr<Employee>>>  queryAllEmployees();
	//��ѯ���в��ű��
	const std::list<std::string>  queryAllOrgIndexCode();
};