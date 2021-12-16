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
#include <vector>
#include <functional>

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
	
	virtual void readDataBase(std::shared_ptr<GHND_ReadData> sp) override;
	virtual bool writeDataBase(const QVariant & var) override;
	//д��������
	bool writeOrganization();
	//��ѯ������Ϣ
	void queryOrg(std::shared_ptr<GHND_ReadData> sp);
	//��ѯ���е�Ա����Ŀ
	void queryAllEmployees(std::shared_ptr<GHND_ReadData> sp);
	//��ѯ���в��ű��
	void queryAllOrgIndexCode(std::shared_ptr<GHND_ReadData> &sp);
private:
	using SelectFunction = std::function<void(std::shared_ptr<GHND_ReadData>)>;//��ѯ����
	using SelectPair=std::map<int, SelectFunction>;//������ص�����ֵ��ִ����صĺ���
	SelectPair sSp;//��ѯ��
protected:
	//��ʼ����ѯ��������
	void initSelectFunc();
};