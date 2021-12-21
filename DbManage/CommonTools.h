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
	//��ֻ��ģʽ��json�ĵ�
	static QVariant openJsonDoc(const QString & fileName,const QIODevice::OpenMode model);
	//����ѹ��
	QByteArray Compress(QByteArray postBody);
	//���ݽ�ѹ
	QByteArray UnCompress(QByteArray src);

};

