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
	//��ֻ��ģʽ��json�ĵ�
	static QVariant openJsonDoc(const QString & fileName,const QIODevice::OpenMode model);
};

