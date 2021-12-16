#include "DataStore.h"
#include <QFile>
#include <QJsonObject>
#include <QDate>
#include <QCoreApplication>
#include <QDebug>
#include <qjsonarray.h>
#include <QDir>
#include "DbManageContexInterface.h"
#include "StructureData.h"

// lock for file 
QReadWriteLock DataStore::rtimefilelocak;
QReadWriteLock DataStore::orgfilelocak;
QReadWriteLock DataStore::totalfilelock;
QReadWriteLock DataStore::doorsfilelock;
// lock for cache
QReadWriteLock DataStore::rtimefileCachelocak;
QReadWriteLock DataStore::orgfileCachelock;
QReadWriteLock DataStore::totalfileCachelock;
QReadWriteLock DataStore::doorsfileCachelock;

int  DataStore::realNumber;
QJsonObject DataStore::realtimeObjectCache;
int DataStore::totalObjectCache = -1;
QJsonObject DataStore::orgDataCache;
QJsonObject DataStore::doorCheckCache;

bool DataStore::addTotalToday(QString personId)
{
	
	QWriteLocker locker(&totalfilelock);
	qDebug()<<"enter:addTotalToday";
	QString path = QCoreApplication::applicationDirPath();
	QString filename = path + "/Data/RealTime/total.data";
	QString tfilename= path + "/Data/RealTime/todayPersonId.data";//���������Ա��id
	auto item = DataStore::openJsonDocument(filename);
	auto idObj = DataStore::openJsonDocument(tfilename);//�򿪴����id�ļ�
	auto idArray = idObj["id"].toArray();

	//�ж��Ƿ�Ϊ��
	if (item.isEmpty())
	{
		item["totalInToday"] = 1;
		idArray = QJsonArray();
		idArray.append(personId);
		idObj["id"] = idArray;
		item["updateDay"] = QDate::currentDate().toString("yyyy-MM-dd");
	}
	else 
	{
		bool containflag = idArray.contains(personId);
		//�µ�һ��
		if (QDate::currentDate().toString("yyyy-MM-dd") != item["updateDay"].toString()) 
		{
			item["totalInToday"] = 1;
			item["updateDay"] = QDate::currentDate().toString("yyyy-MM-dd");
			idArray = QJsonArray();
			idArray.append(personId);
			idObj["id"] = idArray;
		}
		else 
		{
			if (!containflag)
			{
				item["totalInToday"] = item["totalInToday"].toInt() + 1;
				idArray.append(personId);
				idObj["id"] = idArray;
			}
		}
	}
	setTotalTodayCache(item["totalInToday"].toInt());
	writeJsonDocument(tfilename, idObj);//дid
	writeJsonDocument(filename, item);//д����
	qDebug()<<"leave:addTotalToday";
	return true;
}
int DataStore::getTotalTodayCache() {
	qDebug()<<"enter:getTotalTodayCache";
	if (totalObjectCache < 0) {
		totalObjectCache = getTotalToday();
	}
	qDebug()<<"leave:getTotalTodayCache";
	return totalObjectCache;
	
}
void DataStore::setTotalTodayCache(int val)
{
	qDebug()<<"enter:setTotalTodayCache";
	totalObjectCache = val;
	qDebug()<<"leave:setTotalTodayCache";
}
int DataStore::getTotalToday()
{ 
	QReadLocker locker(&totalfilelock);
	qDebug()<<"enter:getTotalToday";
	QString path = QCoreApplication::applicationDirPath();
	QString filename = path + "/Data/RealTime/total.data";
	auto item = DataStore::openJsonDocument(filename);
	if (item.empty()) {
		return 0;
	}
	else {
		if (QDate::currentDate() != QDate::fromString(item["updateDay"].toString(),"yyyy-MM-dd")) {
			//item["totalInToday"] = 0;
			//item["updateDay"] = QDate::currentDate().toString("yyyy-MM-dd");
			//writeJsonDocument(filename, item);
			return 0;
		}else{
			qDebug()<<"leave:getTotalToday";
			return item["totalInToday"].toInt();
		}
	}
}

//��json�ĵ���������json����
QJsonObject DataStore::openJsonDocument(QString filename)
{
	qDebug()<<"enter:openJsonDocument";
	QFile loadfile(filename);
	if (!loadfile.open(QIODevice::ReadOnly| QIODevice::Text))
	{
		qDebug("could't open %s", qPrintable(filename));
		return QJsonObject();
	}
	QByteArray allData = loadfile.readAll();
	QJsonParseError json_error;
	QJsonDocument jsonDoc(QJsonDocument::fromJson(allData, &json_error));
	if (json_error.error != QJsonParseError::NoError)
	{
		qDebug("json error:%s", qPrintable(json_error.errorString()));
		return QJsonObject();//���ؿյ�json����
	}
	if (jsonDoc.isObject()) {
		qDebug()<<"leave:openJsonDocument";
        return jsonDoc.object();
	}
	else
		return QJsonObject();
}
//дjson�ĵ�
void DataStore::writeJsonDocument(QString fileName, QJsonObject &writeObject)
{
	qDebug()<<"enter:writeJsonDocument";
	QFile loadfile(fileName);
	if (!loadfile.open(QIODevice::WriteOnly))
	{
		qDebug("error:could't open %s ", qPrintable(fileName));
	}
	QJsonDocument document;
	document.setObject(writeObject);
	loadfile.write(document.toJson());
	loadfile.flush();
	loadfile.close();
	qDebug()<<"leave:writeJsonDocument";
}

bool DataStore::checkDoorByName(QString doorName, DoorDirection & InOut)
{
	qDebug()<<"enter:checkDoorByName";
	QReadLocker locker(&doorsfilelock); 
	QString path = QCoreApplication::applicationDirPath();
	QString filename = path + "/Data/Config/doors.data";
	qDebug() <<"Checking Door:" << doorName;
	doorCheckCache = DataStore::openJsonDocument(filename); 

	if (doorCheckCache.isEmpty()) {
		qDebug() << "Get Doors Config File Failed!";
		return false;
	}
	else { 
		if (doorCheckCache["doors"].toObject()["imports"].isArray()) {
			auto doors = doorCheckCache["doors"].toObject()["imports"].toArray();
		
			for (QJsonValue door : doors) {
				if (doorName.toStdString() == door["name"].toString().toStdString()) {
					InOut = DoorDirection::Import;
					return true;
				}
			} 
		}
		else {
			qDebug() << "Get Doors Config File Imports Array Failed!";
			return false;
		}

		if (doorCheckCache["doors"].toObject()["exports"].isArray()) {
			auto doors = doorCheckCache["doors"].toObject()["exports"].toArray();
			for (QJsonValue door : doors) {
				if (doorName == door["name"].toString()) {
					InOut = DoorDirection::Export;
					return true;
				}
			}
		}
		else {
			qDebug() << "Get Doors Config File Exports Array Failed!";
			return false;
		}

		InOut = DoorDirection::Unused;
		qDebug()<<"leave:checkDoorByName";
		return true;
	}

	qDebug() << "Get Doors Config File Access Fault State!";
	return false;
}

//读取部门数据缓存,由于可能为空需要写操作
QJsonObject DataStore::getOrgnizeDataCache() 
{
	QWriteLocker locker(&orgfilelocak);
	if (orgDataCache.isEmpty()) 
	{
		std::shared_ptr<GHND_ReadData> sp(new GHND_ReadData(READ_ALLORG));

		DbManageContexInterface(Basic).getReadDbData(sp);
		orgDataCache = sp->plDataPtr->toJsonObject();
	}
	return orgDataCache;
}

//更新部门数据缓存
void DataStore::setOrgnizeDataCache(QJsonObject data) 
{
	QWriteLocker lock(&orgfilelocak);
	//orgDataCache = std::move(DbManageContexInterface(Basic, READ_ALLORG).getReadDbData().toJsonObject());
}


void DataStore::setOrgnizeData(QJsonObject data)
{
	qDebug()<<"enter:setOrgnizeData";
	setOrgnizeDataCache(data);
	QWriteLocker locker(&orgfilelocak);
	QString appPath = QCoreApplication::applicationDirPath();
	QString  fileDir = appPath + "/Data/Config";
	if (checkDirExist(fileDir)) {
		QString fileName = fileDir + "/org.data";
		writeJsonDocument(fileName, data);
	}
	qDebug()<<"leave:setOrgnizeData";
}
QJsonObject DataStore::getRealTimeDataCache() {
	qDebug()<<"enter:getRealTimeDataCache";
	QReadLocker locker(&rtimefileCachelocak);
	if (DataStore::realtimeObjectCache.isEmpty()) {
		realtimeObjectCache = getRealTimeData();
	}
	qDebug()<<"leave:getRealTimeDataCache";
	return realtimeObjectCache;
}
void DataStore::setRealTimeDataCache(QJsonObject cacheObject)
{
	qDebug()<<"enter:setRealTimeDataCache";
	QWriteLocker locker(&rtimefileCachelocak);
	realtimeObjectCache = cacheObject;
	qDebug()<<"leave:setRealTimeDataCache";
}
QJsonObject DataStore::getRealTimeData()
{
	QReadLocker locker(&rtimefilelocak);
	qDebug()<<"enter:getRealTimeData";
	QString appPath = QCoreApplication::applicationDirPath();
	//�ж��ļ����Ƿ���ڣ����������򴴽���·���µ��ļ���
	QString  fileDir = appPath + "/Data/RealTime";
	if (checkDirExist(fileDir)) {
		QString fileName = fileDir + "/rtime.data";
		//�����ļ��Ƿ���ڶ�Ҫ��json�ĵ�
		QJsonObject   Orglist = openJsonDocument(fileName);
		qDebug()<<"leave:getRealTimeData";
		return Orglist;
	}
	else {
		return QJsonObject();
	} 
}

void DataStore::setRealTimeData(QJsonObject data)
{
	/*setRealTimeDataCache(data);*/
	QWriteLocker locker(&rtimefilelocak);
	qDebug()<<"enter:setRealTimeData";
	QString appPath = QCoreApplication::applicationDirPath();
	//�ж��ļ����Ƿ���ڣ����������򴴽���·���µ��ļ���
	QString  fileDir = appPath + "/Data/RealTime";
	if (checkDirExist(fileDir)) {
		QString fileName = fileDir + "/rtime.data";
		writeJsonDocument(fileName, data);
	}
	qDebug()<<"leave:setRealTimeData";
}

void DataStore::calcDepartPersonNum(QString orgIndex)
{ 
	qDebug()<<"enter:calcDepartPersonNum";
	QJsonObject OrgDoc = DataStore::getRealTimeDataCache();
	QJsonObject orginfo = OrgDoc[orgIndex].toObject();
	int realnum = 0;
	for (auto personIndexs : orginfo.keys())
	{
		if (orginfo[personIndexs].isObject()) {
			if (orginfo[personIndexs].toObject()["iostate"] == "in") {
				realnum++;
			}
		}
	}
	orginfo["realnumber"] = realnum;
	OrgDoc[orgIndex] = orginfo; 
	setRealTimeDataCache(OrgDoc); 
	qDebug()<<"leave:calcDepartPersonNum";
}

bool DataStore::checkDirExist(QString path)
{ 
	qDebug()<<"enter:checkDirExist";
	QDir dir;
	if (!dir.exists(path))
	{
		if (!dir.mkdir(path)) {
			qDebug("create %s path failed.", qPrintable(path));
			return false;
		} 
	}
	qDebug()<<"leave:checkDirExist";
	return true;
}


QString DataStore::JsonObjectToJson(QJsonObject object) {
	qDebug()<<"enter:JsonObjectToJson(QJsonObject object)";
	QJsonDocument doc;
	doc.setObject(object);
	qDebug()<<"leave:JsonObjectToJson(QJsonObject object)";
	return doc.toJson();
}
QString DataStore::JsonObjectToJson(QJsonArray object) {
	qDebug()<<"enter:JsonObjectToJson(QJsonArray object)";
	QJsonDocument doc;
	doc.setArray(object);
	qDebug()<<"leave:JsonObjectToJson(QJsonArray object)";
	return doc.toJson();
}

void DataStore::deleteoldData(QStringList &newOrgIndex)
{
	////���û�л�ȡ��ISC�����򷵻�
	//if(newOrgIndex.count()==0)
	//    return false;
	//else
	//{
		//����org.data
	qDebug()<<"enter:deleteoldData";
		QJsonObject orgList=DataStore::getOrgnizeDataCache();
		if(newOrgIndex.count()>0)
		     for (auto var : orgList.keys())
		    {
			    //δ�鵽�ò���
			   if (!newOrgIndex.contains(var))
			  {
				orgList.remove(var);
			  }
		    }
		DataStore::setOrgnizeDataCache(orgList);
		//����rtime.data
		QJsonObject rtimeObj = DataStore::getRealTimeDataCache();
		QStringList orgIndexCodes = orgList.keys();
		for (auto var1 : rtimeObj.keys())
		{
			//δ�鵽�ò���
			if (!orgIndexCodes.contains(var1))
			{
				rtimeObj.remove(var1);
			}
		}
		DataStore::setRealTimeDataCache(rtimeObj);
	//}
		qDebug()<<"leave:deleteoldData";
}

void DataStore::updateCache2File()
{ 
	setRealTimeData(getRealTimeDataCache());
}
