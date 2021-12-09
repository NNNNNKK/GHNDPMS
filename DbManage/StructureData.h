#pragma once
#include <QString>
#include <QJsonObject>
#include <list>
#include <QVariantList>
#include <memory>

const int READ_ALLORG = 1;//��ȡ���еĲ�����Ϣ
const int READ_AllORGINDEXCODE = 2;//��ȡ���еĲ���Ψһ��ʶ��

const int READ_ACCOUNT = 100;//��ȡ�˺�

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
struct GHND_ReadData
{
	long * plDataPtr = nullptr;						//����ָ��--���ݽṹ���ȡ
	long * plRequireDataPtr = nullptr;				//��������ָ��
	int nReadOpState;								//��ȡ�����궨��

	long nResult;									//���ؽ������
};

/*****************************�����붨��*********************************/
const int READ_OK = 3000;			//��ȡ�ɹ�
const int READ_DATA_PTR_NULL = 3001;//������ָ��Ϊ��