#pragma once

#include <QObject>
class DbOperationBase;			//���ݿ�����ӿ�
struct GHND_ReadData;			//��ȡ���ݽṹ��
enum GHND_DbDataOpType;			//�������������ͣ��������ݣ�ʵʱ���ݣ���ʷ����

class  DbManageContexInterface:public QObject
{
public:
	//typeΪ�������ݿ�����ͣ���ȡ�������ݣ�ʵʱ�����Լ���ʷ����
	DbManageContexInterface(GHND_DbDataOpType type);
	~DbManageContexInterface();

	//���ض�ȡ����������
	void getReadDbData(GHND_ReadData * const pRd);
private:
	DbOperationBase * dbBase;
};
