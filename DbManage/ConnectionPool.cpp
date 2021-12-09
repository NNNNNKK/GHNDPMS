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
	//销毁连接池的时候删除所有的连接
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
	//已创建连接数
	int connectionCount = pool.unusedConnectionNames.size() + pool.usedConnectionNames.size();
	//如果连接已用完,等待waitInterval毫秒看看是否有可用连接,最长等待时间maxWaitTime毫秒
	for (int i = 0; i < pool.maxWaitTime&&pool.unusedConnectionNames.size() == 0 && 
		connectionCount == pool.maxConnectionCount;i += pool.waitInterval)
	{
		waitConnetion.wait(&mutex, pool.waitInterval);
		//重新计算已创建连接数
		connectionCount = pool.unusedConnectionNames.size() + pool.usedConnectionNames.size();
	}
	if (pool.unusedConnectionNames.size() > 0)
	{
		//有已回收的，复用他们
		connectionName = pool.unusedConnectionNames.dequeue();
	}
	else if (connectionCount < pool.maxConnectionCount)
	{
		//没有可回收的连接，但是没有达到最大连接数目，则创建新的连接
		connectionName = QString("Connection-%1").arg(connectionCount + 1);
	}
	else
	{
		qDebug() << "Cannot create more connections.";
		return QSqlDatabase();
	}
	//创建连接
	QSqlDatabase db = pool.createConnection(connectionName);
	//有效的连接才放入usedConnectionNames
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
	//如果是我们创造的连接,从used里删除,放入unused
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
	//连接已经连接过了,复用，而不是重新创建
	if (QSqlDatabase::contains(connectionName))
	{
		QSqlDatabase db1 = QSqlDatabase::database(connectionName);
		if (testOnBorrow)
		{
			//返回连接前访问数据库,如果断开连接,重新建立连接
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
	//创建一个新的连接
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