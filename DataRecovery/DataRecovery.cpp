#include "DataRecovery.h"
#include "ConnectionPool.h"
#include <QDebug>
#include <QFile>
#include <QJsonObject>
#include <QFileDialog>
#include <vector>
#include <iostream>
#include <QString>

DataRecovery::DataRecovery(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
}

QVariant DataRecovery::openJsonDoc(const QString &fileName)
{
	QFile file(fileName);
	if (!file.open(QIODevice::ReadOnly|QIODevice::Text))
	{
		qDebug("could not open %s",qPrintable(fileName));
		return QVariant();
	}
	QByteArray allData = file.readAll();
	QJsonParseError j_error;
	QJsonDocument jDoc(QJsonDocument::fromJson(allData, &j_error));
	if (j_error.error != QJsonParseError::NoError)
	{
		qDebug("json error: %s", qPrintable(j_error.errorString()));
		return QVariant();
	}
	if (jDoc.isArray())
	{
		return jDoc.array();
	}
	else if(jDoc.isObject())
	{
		return jDoc.object();
	}
	return QVariant();
}

void DataRecovery::writeJsonToDb(const QString & fileName)
{
	auto jObj = openJsonDoc(fileName).toJsonObject();
	QStringList objCodes = jObj.keys();//部门唯一标识码
	foreach (QString objCode , objCodes)
	{
		QJsonObject orgMsg = jObj[objCode].toObject();
		QString orgCode = orgMsg["orgIndexCode"].toString();
		QString orgName = orgMsg["orgName"].toString();
		QString orgNo = orgMsg["orgNo"].toString();
		QString orgPath = orgMsg["orgPath"].toString();
		int orgEmployessNumber = orgMsg["orgEmployeesNumber"].toInt();
		int isModify = orgMsg["isModify"].toInt();
		int islocal = orgMsg["islocal"].toInt();
		//将该条数据写入到数据库
		auto DB= ConnectionPool::openConnection();
		if (!DB.open())
		{
			//打印错误,关闭连接,函数返回
			qDebug("connect db error: %s",qPrintable(DB.lastError().text()));
			ConnectionPool::closeConnection(DB);
			return;
		}
		QSqlQuery query(DB);
		query.prepare("exec GHNDDB.dbo.GHND_SaveDepartment ?,?,?,?,?,?,?");
		query.bindValue(0, orgCode);
		query.bindValue(1, orgName);
		query.bindValue(2, orgEmployessNumber);
		query.bindValue(3, islocal);
		query.bindValue(4, orgNo);
		query.bindValue(5, orgPath);
		query.bindValue(6, "");

		if (!query.exec())
		{
			//打印错误,关闭连接,函数返回
			qDebug("connect db error: %s", qPrintable(query.lastError().text()));
			ConnectionPool::closeConnection(DB);
			return;
		}
		//获取部门的员工列表
		auto orgEmployees = orgMsg["employeelist"].toArray();
		for (auto cit = orgEmployees.constBegin(); cit != orgEmployees.constEnd(); ++cit)
		{
			QJsonObject employee = (*cit).toObject();
			QString certificateNo = employee["certificateNo"].toString();
			int certificateType = employee["certificateType"].toInt();
			QChar gender = employee["gender"].toInt();
			QString jobNo = employee["jobNo"].toString();
			QString orgIndexCode = employee["orgIndexCode"].toString();
			QString orgName = employee["orgName"].toString();
			QString personId = employee["personId"].toString();
			QString personName = employee["personName"].toString();
			QString phoneNo = employee["phoneNo"].toString();
			QString pinyin = employee["pinyin"].toString();

			query.prepare("exec GHNDDB.dbo.GHND_Employee ?,?,?,?,?,?,?,?,?,?,?");
			query.bindValue(0, personId);
			query.bindValue(1, personName);
			query.bindValue(2, pinyin);
			query.bindValue(3, gender);
			query.bindValue(4, phoneNo);
			query.bindValue(5, jobNo);
			query.bindValue(6, orgIndexCode);
			query.bindValue(7, orgName);
			query.bindValue(8, certificateNo);
			query.bindValue(9, certificateType);
			query.bindValue(10, "");
			if (!query.exec())
			{
				//打印错误,关闭连接,函数返回
				qDebug("connect db error: %s", qPrintable(query.lastError().text()));
				ConnectionPool::closeConnection(DB);
				return;
			}
		}
		//关闭连接,释放资源
		ConnectionPool::closeConnection(DB);
	}
}

void GetAllFileFolder(QString dirPath, std::vector<QString> &folder)
{
	QDir dir(dirPath);
	dir.setFilter(QDir::Dirs);
	foreach(QFileInfo fullDir, dir.entryInfoList())
	{
		if (fullDir.fileName() == "." || fullDir.fileName() == "..") continue;
		folder.push_back(fullDir.absoluteFilePath());
		GetAllFileFolder(fullDir.absoluteFilePath(), folder);
	}
}

void DataRecovery::recoveryHistory()
{
	QString dirPath = "D:/VsProjects/GHNDPMS/Debug/Data/HistoryData/";
	std::vector<QString> folder;
	GetAllFileFolder(dirPath, folder);
	for (int i=0;i<folder.size();i++)
	{
		//文件夹路径
		QString dirPath = folder.at(i);
		auto res= dirPath.split('/');
		//表时间
		QString yymm= res.at(res.size()-1);
		QDir dir(dirPath);

		QFileInfoList subFileList = dir.entryInfoList(QDir::Files | QDir::CaseSensitive);
		for (int i = 0; i < subFileList.size(); i++)
		{
			QString suffix = subFileList[i].suffix();//获取后缀名
			if (suffix.compare("data") == 0)//如果后缀为"txt"
			{
				QString fileName = dirPath + "/"+subFileList[i].baseName() + ".data";
				auto jObj = openJsonDoc(fileName).toJsonObject();
				auto keys=jObj.keys();
				//该部门的历史数据
				auto obj = jObj[keys.at(0)].toObject();
				QString orgIndexCode = obj["departId"].toString();
				QString orgName = obj["orgName"].toString();
				QString turnoverRate = obj["turnoverRate"].toString();
				QString actualWkT = obj["actualWkT"].toString();
				QString attendanceRatio = obj["attendanceRatio"].toString();
				int contractPsN = obj["contractPsN"].toInt();
				QString contractWrH = obj["contractWrH"].toString();
				int count = obj["count"].toInt();
				//将jsonArray转为QString
				QJsonDocument doc;
				doc.setArray(obj["data"].toArray());
				auto compressData = QString::fromLocal8Bit(doc.toJson(QJsonDocument::Compact).constData());
				//auto size=compressData.size();
				//QString data = UnCompress(compressData);// QString::fromUtf8(doc.toJson(QJsonDocument::Compact).constData());
				//auto ssize = data.size();
				auto db = ConnectionPool::openConnection();
				QSqlQuery query(db);
				if (!db.open())
				{
					//打印错误,关闭连接,函数返回
					qDebug("connect db error: %s", qPrintable(db.lastError().text()));
					ConnectionPool::closeConnection(db);
					return;
				}
				query.prepare("exec GHNDDB.dbo.GHND_SaveHistoryDataRecord ?,?,?,?,?,?,?,?,?,?");
				query.bindValue(0, orgIndexCode);
				query.bindValue(1, orgName);
				query.bindValue(2, turnoverRate);
				query.bindValue(3, actualWkT);
				query.bindValue(4, attendanceRatio);
				query.bindValue(5, contractPsN);
				query.bindValue(6, contractWrH);
				query.bindValue(7, count);
				query.bindValue(8, compressData);
				query.bindValue(9, yymm);
				if (!query.exec())
				{
					//打印错误,关闭连接,函数返回
					qDebug("connect db error: %s", qPrintable(query.lastError().text().toLocal8Bit()));
					ConnectionPool::closeConnection(db);
					return;
				}
				query.clear();
				ConnectionPool::closeConnection(db);
			}
		}
	}
}

QByteArray DataRecovery::Compress(QByteArray postBody)
{
	QByteArray outBuf;
	z_stream c_stream;
	int err = 0;
	int windowBits = 15;
	int GZIP_ENCODING = 16;
	if (!postBody.isEmpty())
	{
		c_stream.zalloc = (alloc_func)0;
		c_stream.zfree = (free_func)0;
		c_stream.opaque = (voidpf)0;
		c_stream.next_in = (Bytef *)postBody.data();
		c_stream.avail_in = postBody.size();
		if (deflateInit2(&c_stream, Z_DEFAULT_COMPRESSION, Z_DEFLATED,
			MAX_WBITS + GZIP_ENCODING, 8, Z_DEFAULT_STRATEGY) != Z_OK) return QByteArray();
		for (;;) {
			char destBuf[4096] = { 0 };
			c_stream.next_out = (Bytef *)destBuf;
			c_stream.avail_out = 4096;
			int err = deflate(&c_stream, Z_FINISH);
			outBuf.append(destBuf, 4096 - c_stream.avail_out);
			if (err == Z_STREAM_END || err != Z_OK)
			{
				break;
			}
		}
		auto total = c_stream.total_out;
		deflateEnd(&c_stream);
		total = c_stream.total_out;
	}
	return std::move(outBuf);
}

QByteArray DataRecovery::UnCompress(QByteArray src)
{
	QByteArray outBuffer;
	z_stream strm;
	strm.zalloc = NULL;
	strm.zfree = NULL;
	strm.opaque = NULL;

	strm.avail_in = src.size();
	strm.next_in = (Bytef *)src.data();

	int err = -1, ret = -1;
	err = inflateInit2(&strm, MAX_WBITS + 16);
	if (err == Z_OK) {
		while (true)
		{
			char buffer[4096] = { 0 };
			strm.avail_out = 4096;
			strm.next_out = (Bytef *)buffer;
			int code = inflate(&strm, Z_FINISH);
			outBuffer.append(buffer, 4096 - strm.avail_out);
			if (Z_STREAM_END == code || Z_OK != code)
			{
				break;
			}
		}
	}
	inflateEnd(&strm);
	return outBuffer;
}

void DataRecovery::on_btnDataRecovery_clicked()
{
	//QString fileName = QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("Data Recovery"));
	//writeJsonToDb(fileName);
	recoveryHistory();
}