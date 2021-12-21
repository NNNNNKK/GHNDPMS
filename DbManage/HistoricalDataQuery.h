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
	//初始化查询函数
	void initSelectFunc();
private:
	using SelectFunction = std::function<void(std::shared_ptr<GHND_RWData>)>;//查询函数
	using SelectPair = std::map<int, SelectFunction>;//根据相关的整数值，执行相关的函数
	SelectPair sSp;//查询对
};

