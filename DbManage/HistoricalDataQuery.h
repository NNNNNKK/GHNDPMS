#pragma once
#include <qobject.h>
#include "ConnectionPool.h"

class HistoricalDataQuery :public QObject
{
	Q_OBJECT
public:
	explicit HistoricalDataQuery();
	~HistoricalDataQuery() {};

};

