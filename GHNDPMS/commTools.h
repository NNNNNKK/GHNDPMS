#pragma once
#include <qlogging.h>

constexpr auto AUTO_RUN = "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run";
//设置是否自启动
void setAutoStart(bool bstart);
/**存储调试日志*/
void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg);
//订阅hik事件服务
bool subscribeHikevent();
//取消事件订阅
bool cancleHikevent();