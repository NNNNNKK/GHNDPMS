/**
  @file
  @author Stefan Frings
*/
#include "requestmapper.h"
#include "filelogger.h"
#include "staticfilecontroller.h"
#include "RealtimePrsNumberController.h"
#include "HistoryDepartDetailData.h"
#include "setcontroller.h"
#include "getdepartcontroller.h"
#include "RealTimeDepartDetailData.h"
#include "HistoryDepartData.h"
#include "httpcallBackController.h"
#include "RealtimeDepartData.h"
#include <QCoreApplication>
#include <QProcess>
#include <QCoreApplication>
/** Redirects log messages to a file */
extern FileLogger* logger;

/** Controller for static files */
extern StaticFileController* staticFileController;
RequestMapper::RequestMapper(QObject* parent)
    :HttpRequestHandler(parent)
{
    qDebug("RequestMapper: created");
}
RequestMapper::~RequestMapper()
{
    qDebug("RequestMapper: deleted");
}
void RequestMapper::service(HttpRequest& request, HttpResponse& response)
{
    QByteArray path=request.getPath();
    qDebug("RequestMapper: path=%s",path.data());
    //实时人数
    if(path.startsWith("/RealTimeNumber"))
    {
        RealtimePrsNumberController().service(request,response);
    }
    //各个部门总体实时信息
    else if (path.startsWith("/RealTimeDepartData"))
    {
       RealtimeDepartData().service(request,response);
    }
	//部门实时数据详情
	else if (path.startsWith("/RealTimeDepartDetailData"))
	{
		RealtimeDepartDetailData().service(request, response);
	}
	//本单位各个部门总体历史信息
	else if(path.startsWith("/HistoryDepartData"))
	{
		HistoryDepartData().service(request, response);
	}
    //历史信息详情
    else if(path.startsWith("/HistoryDepartDetailData"))
    {
		HistoryDepartDetailData().service(request,response);
    }
    //获取合同部门的Id和名字
    else if(path.startsWith("/GetDepart"))
    {
       GetDepartController().service(request,response);
    }
    //设置
    else if(path.startsWith("/Set"))
    {
       SetController().service(request,response);
    }
	//回调接口
	else if(path.startsWith("/CallBack"))
	{
		httpcallBackController().service(request, response);
	}
    // All other pathes are mapped to the static file controller.
    // In this case, a single instance is used for multiple requests.
    else
    {
        staticFileController->service(request, response);
    }
    qDebug("RequestMapper: finished request");
    // Clear the log buffer
    if(logger)
    {
       logger->clear();
    }
}

