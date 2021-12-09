#pragma once
#include <qjsondocument.h>
#include <QReadWriteLock> 
#include <QTimer>
#include <QCoreApplication>

enum class DoorDirection
{
	Import,Export,Unused
};

const QString appPath = QCoreApplication::applicationDirPath();

class DataStore
{
public:
	static bool addTotalToday(QString personId);
	static int getTotalToday();
	static int getTotalTodayCache();
	static void setTotalTodayCache(int val);

	static QJsonObject openJsonDocument(QString filename);
	static void writeJsonDocument(QString fileName, QJsonObject & writeObject);
	static bool checkDoorByName(QString doorName, DoorDirection & InOut);
	//static QJsonObject& GetGlobalRealTimeObject() {
	//	return DataStore::GlobalRealTimeObject;
	//}
    static QJsonObject getOrgnizeDataCache();
	static void setOrgnizeData(QJsonObject data);
	static void setOrgnizeDataCache(QJsonObject data);

	static QJsonObject getRealTimeDataCache();
	static  void setRealTimeDataCache(QJsonObject cacheObject);
	static QJsonObject getRealTimeData();
	static void setRealTimeData(QJsonObject data);

	static void calcDepartPersonNum(QString orgIndex);

	static bool checkDirExist(QString path);
	static QString JsonObjectToJson(QJsonObject object);
	static QString JsonObjectToJson(QJsonArray object);
	/**delete old org data and rtime data*/
	static void deleteoldData(QStringList &newOrgIndex);

	static void updateCache2File();
private:
	static QJsonObject GlobalRealTimeObject;
	//lock for file
	static QReadWriteLock rtimefilelocak;
	static QReadWriteLock orgfilelocak;
	static QReadWriteLock totalfilelock;
	static QReadWriteLock doorsfilelock;
	//lock for cache
	static QReadWriteLock rtimefileCachelocak;
	static QReadWriteLock orgfileCachelock;
	static QReadWriteLock totalfileCachelock;
	static QReadWriteLock doorsfileCachelock;

	static int realNumber;

	static QJsonObject realtimeObjectCache;
	static int totalObjectCache;
	static QJsonObject orgDataCache;
	static QJsonObject doorCheckCache;
};

