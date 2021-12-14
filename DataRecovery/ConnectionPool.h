#pragma once
#include <QtSql>
#include <QQueue>
#include <QString>
#include <QMutex>
#include <QMutexLocker>
#include <QDebug>
#include <QSettings>

class ConnectionPool
{
public:
	static void release();//�ر��������ݿ�����
	static QSqlDatabase openConnection();//��ȡ���ݿ�����
	static void closeConnection(QSqlDatabase connection);//�ͷ����ݿ����ӻ����ӳ�
	~ConnectionPool();
private:
	static ConnectionPool& getInstance();
	ConnectionPool();
	ConnectionPool(const ConnectionPool&other);
	ConnectionPool & operator=(const ConnectionPool&other);
	QSqlDatabase createConnection(const QString &connectionName);//�������ݿ�����

	QQueue<QString> usedConnectionNames;//��ʹ�õ����ݿ�������
	QQueue<QString> unusedConnectionNames;//δʹ�õ����ݿ�������

	//���ݿ���Ϣ
	QString hostName;
	QString dataBaseName;
	QString userName;
	QString password;
	QString databaseType;
	int port;

	bool testOnBorrow;//ȡ�����ӵ�ʱ����֤�Ƿ���Ч
	QString testOnBorrowSql;//���Է������ݿ�� SQL

	int maxWaitTime;//��ȡ���ӵ����ȴ�ʱ��
	int waitInterval;//���Ի�ȡ����ʱ�ȴ����ʱ��
	int maxConnectionCount;//���������

	static QMutex mutex;
	static QWaitCondition waitConnetion;
	static ConnectionPool *instance;
};

