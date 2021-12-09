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

/**存储调试日志*/
void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
	// 加锁
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
	// 设置输出信息格式
	QString strDateTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
	QString strMessage = QString("Time:%1 Type:%2  Messge:%3  File:%4  Line:%5")
		.arg(strDateTime).arg(strMsg).arg(localMsg.constData()).arg(context.file).arg(context.line);
	// 输出信息至文件中（读写、追加形式）
	QFile file(QString("./logs/log-%1.txt").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd")));
	file.open(QIODevice::ReadWrite | QIODevice::Append);
	QTextStream stream(&file);
	stream << strMessage.toLocal8Bit() << "\r\n";
	//std::cout << strMessage.tolo << std::endl;
	file.flush();
	file.close();
	// 解锁
	mutex.unlock();
}

//订阅hik事件服务
bool subscribeHikevent()
{
	QJsonObject requestObject;//请求body
	QJsonArray eventTypes;//事件类型
	std::string apiAddress = "/artemis/api/eventService/v1/eventSubscriptionByEventTypes";//接口api
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
//取消事件订阅
bool cancleHikevent()
{
	QJsonObject requestObject;//请求body
	QJsonArray eventTypes;//事件类型
	std::string apiAddress = "/artemis/api/eventService/v1/eventUnSubscriptionByEventTypes";//接口api
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