#pragma once
#include <qlogging.h>

constexpr auto AUTO_RUN = "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run";
//�����Ƿ�������
void setAutoStart(bool bstart);
/**�洢������־*/
void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg);
//����hik�¼�����
bool subscribeHikevent();
//ȡ���¼�����
bool cancleHikevent();