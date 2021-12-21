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
#include <QSqlDatabase>
//��ȡ���ݽṹ��
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

	//д��������
	void writeOrgMsg(std::shared_ptr<GHND_RWData> sp);
	//дԱ������
	void writeEmployeeMsg(std::shared_ptr<GHND_RWData> sp);
	//��ѯ������Ϣ
	void queryOrg(std::shared_ptr<GHND_RWData> sp);
	//��ѯ���е�Ա����Ŀ
	void queryAllEmployees(std::shared_ptr<GHND_RWData> sp);
	//��ѯ���в��ű��
	void queryAllOrgIndexCode(std::shared_ptr<GHND_RWData> &sp);
	//��ѯ���ź�����
	void queryOrgBlack(std::shared_ptr<GHND_RWData> sp);
private:
	using SelectFunction = std::function<void(std::shared_ptr<GHND_RWData>)>;//��ѯ����
	using SelectPair=std::map<int, SelectFunction>;//������ص�����ֵ��ִ����صĺ���
	SelectPair sSp;//��ѯ��
protected:
	//��ʼ����ѯ��������
	void initSelectFunc();
};