#include "ConnectionPool.h"

QMutex ConnectionPool::mutex;
QWaitCondition ConnectionPool::waitConnetion;
ConnectionPool *ConnectionPool::instance = nullptr;

ConnectionPool::ConnectionPool() 
{
	hostName = "localhost";
	dataBaseName = "GHNDDB";
	userName = "ma";
	password = "";
	databaseType = "QODBC";

	testOnBorrow = true;
	testOnBorrowSql = "SELECT 1";
	maxWaitTime = 1000;
	waitInterval = 200;
	maxConnectionCount = 1000;
	port = 1435;
}

ConnectionPool::~ConnectionPool()
{
	//�������ӳص�ʱ��ɾ�����е�����
	foreach (QString connectionName, usedConnectionNames)
	{
		QSqlDatabase::removeDatabase(connectionName);
	}

	foreach(QString connectionName, unusedConnectionNames)
	{
		QSqlDatabase::removeDatabase(connectionName);
	}
}

ConnectionPool& ConnectionPool::getInstance()
{
	if (nullptr == instance)
	{
		QMutexLocker locker(&mutex);
		if (nullptr == instance)
		{
			instance = new ConnectionPool();
		}
	}
	return *instance;
}

void ConnectionPool::release()
{
	QMutexLocker locker(&mutex);
	delete instance;
	instance = nullptr;
}

QSqlDatabase ConnectionPool::openConnection()
{
	ConnectionPool &pool = ConnectionPool::getInstance();
	QString connectionName;
	QMutexLocker locker(&mutex);
	//�Ѵ���������
	int connectionCount = pool.unusedConnectionNames.size() + pool.usedConnectionNames.size();
	//�������������,�ȴ�waitInterval���뿴���Ƿ��п�������,��ȴ�ʱ��maxWaitTime����
	for (int i = 0; i < pool.maxWaitTime&&pool.unusedConnectionNames.size() == 0 && 
		connectionCount == pool.maxConnectionCount;i += pool.waitInterval)
	{
		waitConnetion.wait(&mutex, pool.waitInterval);
		//���¼����Ѵ���������
		connectionCount = pool.unusedConnectionNames.size() + pool.usedConnectionNames.size();
	}
	if (pool.unusedConnectionNames.size() > 0)
	{
		//���ѻ��յģ���������
		connectionName = pool.unusedConnectionNames.dequeue();
	}
	else if (connectionCount < pool.maxConnectionCount)
	{
		//û�пɻ��յ����ӣ�����û�дﵽ���������Ŀ���򴴽��µ�����
		connectionName = QString("Connection-%1").arg(connectionCount + 1);
	}
	else
	{
		qDebug() << "Cannot create more connections.";
		return QSqlDatabase();
	}
	//��������
	QSqlDatabase db = pool.createConnection(connectionName);
	//��Ч�����Ӳŷ���usedConnectionNames
	if (db.isOpen())
	{
		pool.usedConnectionNames.enqueue(connectionName);
	}
	return db;
}

void ConnectionPool::closeConnection(QSqlDatabase connection)
{
	ConnectionPool &pool = ConnectionPool::getInstance();
	QString connectionName = connection.connectionName();
	//��������Ǵ��������,��used��ɾ��,����unused
	if (pool.usedConnectionNames.contains(connectionName))
	{
		QMutexLocker locker(&mutex);
		pool.usedConnectionNames.removeOne(connectionName);
		pool.unusedConnectionNames.enqueue(connectionName);
		waitConnetion.wakeOne();
	}
}

QSqlDatabase ConnectionPool::createConnection(const QString &connectionName)
{
	//�����Ѿ����ӹ���,���ã����������´���
	if (QSqlDatabase::contains(connectionName))
	{
		QSqlDatabase db1 = QSqlDatabase::database(connectionName);
		if (testOnBorrow)
		{
			//��������ǰ�������ݿ�,����Ͽ�����,���½�������
			qDebug() << "Test connection on borrow, execute :" << testOnBorrowSql << ",for" << connectionName;
			QSqlQuery query(testOnBorrowSql, db1);
			if (query.lastError().type() != QSqlError::NoError && !db1.open())
			{
				qDebug() << "Open database error:" << db1.lastError().text();
				return QSqlDatabase();
			}
		}
		return db1;
	}
	//����һ���µ�����
	QSqlDatabase db = QSqlDatabase::addDatabase(databaseType, connectionName);
	db.setHostName(hostName);
	db.setDatabaseName(dataBaseName);
	db.setUserName(userName);
	db.setPassword(password);
	db.setPort(port);

	if (!db.open())
	{
		qDebug() << "Open database error:" << db.lastError().text();
		return QSqlDatabase();
	}
	return db;
}