#pragma once
#include <QObject>


class DbOperationBase;			//���ݿ�����ӿ�
struct GHND_RWData;			//��ȡ���ݽṹ��
enum GHND_DbDataOpType;			//�������������ͣ��������ݣ�ʵʱ���ݣ���ʷ����

class DbManageContexInterface :public QObject
{
	Q_OBJECT
public:
	//typeΪ�������ݿ�����ͣ���ȡ�������ݣ�ʵʱ�����Լ���ʷ����
	DbManageContexInterface(GHND_DbDataOpType type);
	~DbManageContexInterface();

	//���ض�ȡ����������
	void OpDbData(std::shared_ptr<GHND_RWData> sp);
private:
	DbOperationBase * dbBase;
};
