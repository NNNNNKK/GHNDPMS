#include "CommonTools.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

CommonTools::CommonTools()
{

}

QVariant CommonTools::openJsonDoc(const QString & fileName,const QIODevice::OpenMode model)
{
	QFile loadfile(fileName);
	//指定文件读取模式
	if (!loadfile.open(model))
	{
		qDebug("could't open %s", qPrintable(fileName));
		return QVariant();
	}
	QByteArray allData = loadfile.readAll();
	QJsonParseError json_error;
	QJsonDocument jsonDoc(QJsonDocument::fromJson(allData, &json_error));
	if (json_error.error != QJsonParseError::NoError)
	{
		qDebug("json error:%s", qPrintable(json_error.errorString()));
		return QVariant();
	}
	if (jsonDoc.isArray())
	{
		return jsonDoc.array();
	}
	else if (jsonDoc.isObject())
	{
		return jsonDoc.object();
	}
	return QVariant();
}
