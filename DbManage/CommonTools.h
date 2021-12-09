#pragma once
#include <qobject.h>
#include <QVariant>
#include <QIODevice>

class CommonTools :public QObject
{
	Q_OBJECT
public:
	explicit CommonTools();
	~CommonTools() {};
	//以只读模式打开json文档
	static QVariant openJsonDoc(const QString & fileName,const QIODevice::OpenMode model);
};

