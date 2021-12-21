#pragma once
#include <qobject.h>
#include "ConnectionPool.h"
#include "DbOperationBase.h"
#include <vector>
#include <functional>
#include <QSqlDatabase>

struct GHND_RWData;

class HistoricalDataQuery :public DbOperationBase
{
	Q_OBJECT
public:
	explicit HistoricalDataQuery();
	~HistoricalDataQuery();
	virtual void OpDataBase(std::shared_ptr<GHND_RWData> sp) override;
	void queryOrgDetailedHistoricalData(std::shared_ptr<GHND_RWData> sp);
protected:
	//��ʼ����ѯ����
	void initSelectFunc();
private:
	using SelectFunction = std::function<void(std::shared_ptr<GHND_RWData>)>;//��ѯ����
	using SelectPair = std::map<int, SelectFunction>;//������ص�����ֵ��ִ����صĺ���
	SelectPair sSp;//��ѯ��
};

