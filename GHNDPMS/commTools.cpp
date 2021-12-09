#include "commTools.h"
#include "hikdatarequest.h"

#include <QCoreApplication>
#include <QSettings>
#include <QMutex>
#include <QDateTime>
#include <QFile>
#include <QTextStream>
#include <QJsonObject>
#include <QJsonArray>

void setAutoStart(bool bstart)
{
	QString application_name = QCoreApplication::applicationName();
	QSettings *settings = new QSettings(AUTO_RUN, QSettings::NativeFormat);
	if (bstart)
	{
		QString application_path = QCoreApplication::applicationFilePath();
		settings->setValue(application_name, application_path.replace("/", "\\"));
	}
	else
	{
		settings->remove(application_name);
	}
	delete settings;
}

/**�洢������־*/
void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
	// ����
	static QMutex mutex;
	mutex.lock();
	QByteArray localMsg = msg.toLocal8Bit();
	QString strMsg("");
	switch (type)
	{
	case QtDebugMsg:
		strMsg = QString("Debug");
		break;
	case QtWarningMsg:
		strMsg = QString("Warning");
		break;
	case QtCriticalMsg:
		strMsg = QString("Critical");
		break;
	case QtFatalMsg:
		strMsg = QString("Fatal");
		break;
	}
	// ���������Ϣ��ʽ
	QString strDateTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
	QString strMessage = QString("Time:%1 Type:%2  Messge:%3  File:%4  Line:%5")
		.arg(strDateTime).arg(strMsg).arg(localMsg.constData()).arg(context.file).arg(context.line);
	// �����Ϣ���ļ��У���д��׷����ʽ��
	QFile file(QString("./logs/log-%1.txt").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd")));
	file.open(QIODevice::ReadWrite | QIODevice::Append);
	QTextStream stream(&file);
	stream << strMessage.toLocal8Bit() << "\r\n";
	//std::cout << strMessage.tolo << std::endl;
	file.flush();
	file.close();
	// ����
	mutex.unlock();
}

//����hik�¼�����
bool subscribeHikevent()
{
	QJsonObject requestObject;//����body
	QJsonArray eventTypes;//�¼�����
	std::string apiAddress = "/artemis/api/eventService/v1/eventSubscriptionByEventTypes";//�ӿ�api
	eventTypes.append(196893);
	eventTypes.append(198914);
	requestObject["eventTypes"] = eventTypes;
	QString eventDest = "http://" + HikdataRequest::getIpv4() + ":9080" + "/CallBack";
	requestObject["eventDest"] = eventDest;
	requestObject["subType"] = 0;
	QJsonObject backMsg = HikdataRequest::httpRequest(HikdataRequest::gethikQuery(), apiAddress, requestObject);
	if (backMsg.contains("code") && backMsg["code"].toInt() == 0)
	{
		return true;
	}
	return false;
}
//ȡ���¼�����
bool cancleHikevent()
{
	QJsonObject requestObject;//����body
	QJsonArray eventTypes;//�¼�����
	std::string apiAddress = "/artemis/api/eventService/v1/eventUnSubscriptionByEventTypes";//�ӿ�api
	eventTypes.append(198914);
	eventTypes.append(196893);
	requestObject["eventTypes"] = eventTypes;
	QJsonObject backMsg = HikdataRequest::httpRequest(HikdataRequest::gethikQuery(), apiAddress, requestObject);
	if (backMsg.contains("code") && backMsg["code"].toInt() != 0)
	{
		return false;
	}
	return true;
}