#include "HistoricalDataQuery.h"
#include "StructureData.h"
#include "ConnectionPool.h"

HistoricalDataQuery::HistoricalDataQuery()
{
	db = ConnectionPool::openConnection();
}

HistoricalDataQuery::~HistoricalDataQuery()
{
	ConnectionPool::closeConnection(db);
}

void HistoricalDataQuery::initSelectFunc()
{
	sSp[READ_ALLORG] = std::bind(&HistoricalDataQuery::queryOrgDetailedHistoricalData,
		this, std::placeholders::_1);
}

void HistoricalDataQuery::OpDataBase(std::shared_ptr<GHND_RWData> sp)
{
	if (!sp || OP_NOTHING == sp->nReadOpState)
	{
		return;
	}
	//��ʼ����ѯmap
	if (0 == sSp.size())
	{
		sSp.clear();
		initSelectFunc();
	}

	auto res = sSp.find(sp->nReadOpState);
	if (res != sSp.end())
	{
		SelectFunction f = res->second;
		if (f)
		{
			f(sp);
		}
	}
}

//����Id���Ҳ�����ϸ��ʷ����
void HistoricalDataQuery::queryOrgDetailedHistoricalData(std::shared_ptr<GHND_RWData> sp)
{
	if (!sp || !sp->plDataPtr || READ_ORGDETAILHISTORIYDATA != sp->nReadOpState
		||!sp->plRequireData)
	{
		return;
	}
	
	try
	{
		//1.��ȡ��������
		QJsonObject request = sp->plRequireData->toJsonObject();
		QString deptId = request["deptId"].toString();
		QString date = request["date"].toString();
		//2.���ݲ�ѯ
		QString tableName = tr("HistoryDataRecord") + date;
		QSqlQuery query(db);
		QString sqlStr = tr("Select * from [GHNDDB].[dbo].[%1] where id = '%2'")
			.arg(tableName).arg(deptId);
		if (query.exec())
		{
			if (query.first())
			{
				QJsonObject backObj
				{
					{"orgIndexCode",query.value("orgIndexCode").toString()},
					{"orgName",query.value("orgName").toString()},
					{"turnoverRate",query.value("turnoverRate").toString()},
					{"actualWkt",query.value("actualWkt").toString()},
					{"attendanceRatio",query.value("attendanceRatio").toString()},
					{"contractWrH",query.value("contractWrH").toString()},
					{"contractPsN",query.value("contractPsN").toInt()},
					{"count",query.value("count").toInt()},
					{"data",(QJsonDocument::fromJson(query.value("data").toString().toUtf8())).array()}
				};
				//�����������
				sp->plDataPtr = std::make_shared<QVariant>(backObj);
				sp->nResult = READ_OK;
			}
		}
	}
	catch (...)
	{
		sp->nResult = FUNCTION_EXCEPTION;
	}
}
