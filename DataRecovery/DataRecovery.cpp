#include "DataRecovery.h"
#include "ConnectionPool.h"
#include <QDebug>
#include <QFile>
#include <QJsonObject>
#include <QFileDialog>

DataRecovery::DataRecovery(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
}

QVariant DataRecovery::openJsonDoc(const QString &fileName)
{
	QFile file(fileName);
	if (!file.open(QIODevice::ReadOnly|QIODevice::Text))
	{
		qDebug("could not open %s",qPrintable(fileName));
		return QVariant();
	}
	QByteArray allData = file.readAll();
	QJsonParseError j_error;
	QJsonDocument jDoc(QJsonDocument::fromJson(allData, &j_error));
	if (j_error.error != QJsonParseError::NoError)
	{
		qDebug("json error: %s", qPrintable(j_error.errorString()));
		return QVariant();
	}
	if (jDoc.isArray())
	{
		return jDoc.array();
	}
	else if(jDoc.isObject())
	{
		return jDoc.object();
	}
	return QVariant();
}

void DataRecovery::writeJsonToDb(const QString & fileName)
{
	auto jObj = openJsonDoc(fileName).toJsonObject();
	QStringList objCodes = jObj.keys();//����Ψһ��ʶ��
	foreach (QString objCode , objCodes)
	{
		QJsonObject orgMsg = jObj[objCode].toObject();
		QString orgCode = orgMsg["orgIndexCode"].toString();
		QString orgName = orgMsg["orgName"].toString();
		QString orgNo = orgMsg["orgNo"].toString();
		QString orgPath = orgMsg["orgPath"].toString();
		int orgEmployessNumber = orgMsg["orgEmployessNumber"].toInt();
		int isModify = orgMsg["isModify"].toInt();
		int islocal = orgMsg["islocal"].toInt();
		//����������д�뵽���ݿ�
		auto DB= ConnectionPool::openConnection();
		if (!DB.open())
		{
			//��ӡ����,�ر�����,��������
			qDebug("connect db error: %s",qPrintable(DB.lastError().text()));
			ConnectionPool::closeConnection(DB);
			return;
		}
		QSqlQuery query(DB);
		query.prepare("exec GHNDDB.dbo.GHND_SaveDepartment ?,?,?,?,?,?,?");
		query.bindValue(0, orgCode);
		query.bindValue(1, orgName);
		query.bindValue(2, orgEmployessNumber);
		query.bindValue(3, islocal);
		query.bindValue(4, orgNo);
		query.bindValue(5, orgPath);
		query.bindValue(6, "");

		if (!query.exec())
		{
			//��ӡ����,�ر�����,��������
			qDebug("connect db error: %s", qPrintable(query.lastError().text()));
			ConnectionPool::closeConnection(DB);
			return;
		}
		//��ȡ���ŵ�Ա���б�
		auto orgEmployees = orgMsg["employeelist"].toArray();
		for (auto cit = orgEmployees.constBegin(); cit != orgEmployees.constEnd(); ++cit)
		{
			QJsonObject employee = (*cit).toObject();
			QString certificateNo = employee["certificateNo"].toString();
			int certificateType = employee["certificateType"].toInt();
			QChar gender = employee["gender"].toInt();
			QString jobNo = employee["jobNo"].toString();
			QString orgIndexCode = employee["orgIndexCode"].toString();
			QString orgName = employee["orgName"].toString();
			QString personId = employee["personId"].toString();
			QString personName = employee["personName"].toString();
			QString phoneNo = employee["phoneNo"].toString();
			QString pinyin = employee["pinyin"].toString();

			query.prepare("exec GHNDDB.dbo.GHND_Employee ?,?,?,?,?,?,?,?,?,?,?");
			query.bindValue(0, personId);
			query.bindValue(1, personName);
			query.bindValue(2, pinyin);
			query.bindValue(3, gender);
			query.bindValue(4, phoneNo);
			query.bindValue(5, jobNo);
			query.bindValue(6, orgIndexCode);
			query.bindValue(7, orgName);
			query.bindValue(8, certificateNo);
			query.bindValue(9, certificateType);
			query.bindValue(10, "");
			if (!query.exec())
			{
				//��ӡ����,�ر�����,��������
				qDebug("connect db error: %s", qPrintable(query.lastError().text()));
				ConnectionPool::closeConnection(DB);
				return;
			}
		}
		//�ر�����,�ͷ���Դ
		ConnectionPool::closeConnection(DB);
	}
}

void DataRecovery::on_btnDataRecovery_clicked()
{
	QString fileName = QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("Data Recovery"));
	writeJsonToDb(fileName);
}