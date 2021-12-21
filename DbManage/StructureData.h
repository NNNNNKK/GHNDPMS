#pragma once
#include <QString>
#include <QJsonObject>
#include <list>
#include <QVariant>
#include <memory>

const int OP_NOTHING = -1;				//����ȡ�κ����ݣ�Ĭ��ֵ
const int READ_ALLORG = 1;					//��ȡ���еĲ�����Ϣ
const int READ_AllORGINDEXCODE = 2;			//��ȡ���еĲ���Ψһ��ʶ��
const int READ_ALLORGEMPLOYEES = 3;			//��ȡ���в��ŵ�����Ա����Ϣ
const int READ_ORGBLACK = 4;				//��ȡ���������б�
const int READ_ACCOUNT = 100;				//��ȡ�˺�
const int READ_ORGDETAILHISTORIYDATA = 101; //��ȡ������ʷ��ϸ����

const int WRITE_ORGMSG = 200;				//д��������
const int WRITE_OK = 201;					//д�ɹ�
const int WRITE_EMPLOYEEMSG = 202;			//дԱ������

/*****************************�����붨��*********************************/
const int READ_OK = 3000;			//��ȡ�ɹ�
const int READ_DATA_PTR_NULL = 3001;//������ָ��Ϊ��
const int FUNCTION_EXCEPTION = 3002;//�����ڲ��쳣
const int DB_EXCEPTION = 3003;		//���ݿ��쳣

//�������������ͣ��������ݣ�ʵʱ���ݣ���ʷ����
enum GHND_DbDataOpType { Basic, RealTime, Historical };

using READTYPE = unsigned int;
//Ա��
struct Employee
{
	std::string			certificateNo = "";						//��֤���
	int					certificateType = 111;					//��֤����
	int					gender = 0;								//�Ա�
	std::string			jobNo = "";								//����
	std::string			orgIndexCode = "";						//����Ψһ��ʶ��
	std::string			orgName = "";							//��������
	std::string			personId = "";							//Ա��ΨһId
	std::string			personName = "";						//Ա������
	std::string			phoneNo = "";							//�ֻ���
	std::string			pinyin = "";							//����ƴ��
};

struct Organization
{
	std::string										orgIndexCode = "";						//����Ψһ��ʶ��
	std::string										orgName = "";							//��������
	std::string										orgNo = "";								//���ű��
	std::string										orgPath = "";							//����·��
	std::string										parentOrgName = "";						//����������
	bool											isModify = false;						//�Ƿ��޸Ĺ�
	bool											islocal = true;							//�Ƿ�Ϊ����λ��֯
	int												orgEmployeesNumber = 0;					//��������
	std::list<std::shared_ptr<Employee>>			employeelist;							//Ա���б� ָ������
};

//�����ݿ�ṹ��
struct GHND_RWData
{
	std::shared_ptr<QVariant> plDataPtr;				//����ָ��--���ݽṹ���ȡ
	std::shared_ptr<QVariant> plRequireData;				//��������ָ��
	int nReadOpState= OP_NOTHING;						//��ȡ�����궨��

	long nResult;										//���ؽ������

	GHND_RWData()
	{
	};
	GHND_RWData(int nReadOpState)
	{
		this->nReadOpState = nReadOpState;
	};
};
