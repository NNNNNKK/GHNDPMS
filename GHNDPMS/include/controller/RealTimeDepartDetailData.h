#ifndef REALTIMEDEPARTDETAILDATA_H
#define REALTIMEDEPARTDETAILDATA_H
#include "httprequest.h"
#include "httpresponse.h"
#include "httprequesthandler.h"
#include "hikdatarequest.h"

#include <QObject>

class RealtimeDepartDetailData : public HttpRequestHandler
{
    Q_OBJECT
public:
    explicit RealtimeDepartDetailData(QObject *parent = nullptr);
    void service(HttpRequest& request, HttpResponse& response);
signals:

public slots:
};

#endif // RealtimeLocalDepartDetailData_H