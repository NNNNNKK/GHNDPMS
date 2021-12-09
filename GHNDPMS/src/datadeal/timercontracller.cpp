#include"timercontracller.h"
#define R_TIMEROUTTIME (7200*1000)
#define O_TIMEROUTTIME (3600*1000)
Timercontracller::Timercontracller(QObject *parent):QObject(parent)
{
	r_timer = new QTimer(this);
	o_timer = new QTimer(this);
	timerUpdateCache2File = new QTimer(this);
	h_rquest = new HikdataRequest();
	connect(r_timer, SIGNAL(timeout()), this ,SLOT(dealRealData()));
	connect(o_timer, SIGNAL(timeout()), h_rquest, SLOT(writeOrganization()));
	connect(timerUpdateCache2File, SIGNAL(timeout()), this, SLOT(dealCache()));
	r_timer->setInterval(R_TIMEROUTTIME);
	o_timer->setInterval(O_TIMEROUTTIME);
	timerUpdateCache2File->setInterval(1000);
}
Timercontracller::~Timercontracller()
{
	if (o_timer->isActive())
	{
		o_timer->stop();
	}
	delete o_timer;
	delete h_rquest;
}

void Timercontracller::startO_timer()
{
	if (o_timer->isActive())
		return;
	else
		o_timer->start();
}


void Timercontracller::stopO_timer()
{
	if (o_timer->isActive())
		o_timer->stop();
	else
		return;
}

void Timercontracller::startR_timer()
{
	if (!timerUpdateCache2File->isActive()) {
		timerUpdateCache2File->start();
	}

	if (r_timer->isActive())
		return;
	else
		r_timer->start();
}


void Timercontracller::stopR_timer()
{
	if (r_timer->isActive())
		r_timer->stop();
	else
		return;
}

void Timercontracller::dealRealData()
{
	QFuture<void> f = QtConcurrent::run(HikdataRequest::realtimeDataDeal);
}

void Timercontracller::dealOrgList()
{
	
}

void Timercontracller::dealCache()
{
	QFuture<void> f = QtConcurrent::run(DataStore::updateCache2File);
}
