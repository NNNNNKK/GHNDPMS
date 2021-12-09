#include <QCoreApplication>
#include <QDir>
#include <QCommandLineParser>
#include <QFuture>
#include <QtConcurrent>
#include "httplistener.h"
#include "templatecache.h"
#include "httpsessionstore.h"
#include "staticfilecontroller.h"
#include "filelogger.h"
#include "requestmapper.h"
#include <QProcess>
#include <QSettings>
#include <iostream>
#include <sstream> 
#include "systemconfig.h"
#include "qtimer.h"
#include "qpointer.h"
#include "timercontracller.h"
#include "commTools.h"

/** Cache for template files */
TemplateCache* templateCache;
/** Storage for session cookies */
HttpSessionStore *sessionStore;
/** Controller for static files */
StaticFileController* staticFileController;
/** Redirects log messages to a file */
FileLogger* logger;

//������
QReadWriteLock* rtimefilelocak;

int main(int argc, char *argv[])
{
	// ��װ��Ϣ�������  
	rtimefilelocak = new QReadWriteLock();
	QCoreApplication app(argc, argv);
	app.setApplicationName("HttpServer");
	app.setOrganizationName("Butterfly");
	QCommandLineParser parser;
	// A boolean option with a single name (-p)
	QCommandLineOption showProgressOption("l",QCoreApplication::translate("main", "Save log to file."));
	parser.addOption(showProgressOption);
	parser.process(app);
	if (parser.isSet(showProgressOption))
	{
		qInstallMessageHandler(myMessageOutput);
	}
	QString AppPath = QCoreApplication::applicationDirPath();
	QString configFilePath = AppPath + "/Profile/config.ini";//�����ļ�·��
	SystemConfig appconfig(configFilePath);
	//����IP��ַ���˿ںš�APPKey��APPSecret
	Unit_query_t query;
	query.appKey = appconfig.Get("AppConfig", "appkey").toString().toStdString();               // ������APPKey
	query.appSecret = appconfig.Get("AppConfig", "appSecret").toString().toStdString();         // ������APPSecret
	query.artemisIp = appconfig.Get("AppConfig", "artemisIp").toString().toStdString();         // ƽ̨IP��ַ
	query.artemisPort = appconfig.Get("AppConfig", "artemisPort").toInt();						// HTTP��HTTPSЭ��˿ڣ�
	//��Ҫ����ʵ��ʹ��HTTP����HTTPSЭ������䣬�˴�ʹ��HTTPSЭ��
	HikdataRequest::sethikQueryPointer(&query);
	HikdataRequest dataRequest(AppPath);//��ʼ����������������
	dataRequest.writeOrganization();//��ȡ����
	QFuture<void> f = QtConcurrent::run(HikdataRequest::realtimeDataDeal);//ʵʱ���ݴ���
	Timercontracller timercontracller;
	timercontracller.startO_timer();//��ʱ����������ʱˢ�²����б�������
	timercontracller.startR_timer();
	QString configFileName = AppPath + "/etc/systemset.ini";
	// Configure template loader and cache
	QSettings* templateSettings = new QSettings(configFileName, QSettings::IniFormat, &app);
	templateSettings->beginGroup("templates");
	templateCache = new TemplateCache(templateSettings, &app);
	// Configure session store
	QSettings* sessionSettings = new QSettings(configFileName, QSettings::IniFormat, &app);
	sessionSettings->beginGroup("sessions");
	sessionStore = new HttpSessionStore(sessionSettings, &app);
	// Configure logging into a file
	QSettings* logSettings = new QSettings(configFileName, QSettings::IniFormat, &app);
	logSettings->beginGroup("logging");
	FileLogger* logger = new FileLogger(logSettings, 10000, &app);
	//logger->installMsgHandler();
	// Configure static file controller
	QSettings* fileSettings = new QSettings(configFileName, QSettings::IniFormat, &app);
	fileSettings->beginGroup("docroot");
	staticFileController = new StaticFileController(fileSettings, &app);
	// Configure and start the TCP listener
	QSettings* listenerSettings = new QSettings(configFileName, QSettings::IniFormat, &app);
	listenerSettings->beginGroup("listener");
	new HttpListener(listenerSettings, new RequestMapper(&app), &app);

	cancleHikevent();
	//�����Ž��¼�
	if (!subscribeHikevent())
		qDebug("errr:subscribe hik event failed");
	qWarning("Application has started");
	return app.exec();
	qWarning("Application has stopped");
	logger->clear();
}
