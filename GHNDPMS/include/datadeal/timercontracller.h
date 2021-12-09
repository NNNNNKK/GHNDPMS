#ifndef TIMERCONTRACLLER_H
#define TIMERCONTRACLLER_H
#include"qobject.h"
#include"qtimer.h"
#include"datadeal/hikdatarequest.h"
#include"qfuture.h"
#include"qtconcurrent"
class Timercontracller:public QObject
{
	Q_OBJECT
public:
	Timercontracller(QObject* parent = nullptr);
	~Timercontracller();
	void startO_timer();//������ʱ��
	void stopO_timer();//�رն�ʱ��
	void startR_timer();//������ʱ��
	void stopR_timer();//�رն�ʱ��
private slots:
	void dealRealData();
	void dealOrgList();
	void dealCache();
private:
	QTimer *o_timer;//���Ŷ�ʱ��
	QTimer *r_timer;//ʵʱ���ݶ�ʱ��
	QTimer *timerUpdateCache2File;//cache file sync
	HikdataRequest *h_rquest;
};

#endif