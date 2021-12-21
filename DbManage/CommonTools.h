#pragma once
#include <qobject.h>
#include <QVariant>
#include <QIODevice>
#include <QtZlib/zlib.h>
#include <QByteArray>

class CommonTools :public QObject
{
	Q_OBJECT
public:
	explicit CommonTools();
	~CommonTools() {};
	//以只读模式打开json文档
	static QVariant openJsonDoc(const QString & fileName,const QIODevice::OpenMode model);
	//数据压缩
	QByteArray Compress(QByteArray postBody);
	//数据解压
	QByteArray UnCompress(QByteArray src);

};

