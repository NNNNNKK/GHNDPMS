#include "CommonTools.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

CommonTools::CommonTools()
{

}

QVariant CommonTools::openJsonDoc(const QString & fileName,const QIODevice::OpenMode model)
{
	QFile loadfile(fileName);
	//指定文件读取模式
	if (!loadfile.open(model))
	{
		qDebug("could't open %s", qPrintable(fileName));
		return QVariant();
	}
	QByteArray allData = loadfile.readAll();
	QJsonParseError json_error;
	QJsonDocument jsonDoc(QJsonDocument::fromJson(allData, &json_error));
	if (json_error.error != QJsonParseError::NoError)
	{
		qDebug("json error:%s", qPrintable(json_error.errorString()));
		return QVariant();
	}
	if (jsonDoc.isArray())
	{
		return jsonDoc.array();
	}
	else if (jsonDoc.isObject())
	{
		return jsonDoc.object();
	}
	return QVariant();
}

QByteArray CommonTools::Compress(QByteArray postBody)
{
	QByteArray outBuf;
	z_stream c_stream;
	int err = 0;
	int windowBits = 15;
	int GZIP_ENCODING = 16;
	if (!postBody.isEmpty())
	{
		c_stream.zalloc = (alloc_func)0;
		c_stream.zfree = (free_func)0;
		c_stream.opaque = (voidpf)0;
		c_stream.next_in = (Bytef *)postBody.data();
		c_stream.avail_in = postBody.size();
		if (deflateInit2(&c_stream, Z_DEFAULT_COMPRESSION, Z_DEFLATED,
			MAX_WBITS + GZIP_ENCODING, 8, Z_DEFAULT_STRATEGY) != Z_OK) return QByteArray();
		for (;;) {
			char destBuf[4096] = { 0 };
			c_stream.next_out = (Bytef *)destBuf;
			c_stream.avail_out = 4096;
			int err = deflate(&c_stream, Z_FINISH);
			outBuf.append(destBuf, 4096 - c_stream.avail_out);
			if (err == Z_STREAM_END || err != Z_OK)
			{
				break;
			}
		}
		auto total = c_stream.total_out;
		deflateEnd(&c_stream);
		total = c_stream.total_out;
	}
	return std::move(outBuf);
}

QByteArray CommonTools::UnCompress(QByteArray src)
{
	QByteArray outBuffer;
	z_stream strm;
	strm.zalloc = NULL;
	strm.zfree = NULL;
	strm.opaque = NULL;

	strm.avail_in = src.size();
	strm.next_in = (Bytef *)src.data();

	int err = -1, ret = -1;
	err = inflateInit2(&strm, MAX_WBITS + 16);
	if (err == Z_OK) {
		while (true)
		{
			char buffer[4096] = { 0 };
			strm.avail_out = 4096;
			strm.next_out = (Bytef *)buffer;
			int code = inflate(&strm, Z_FINISH);
			outBuffer.append(buffer, 4096 - strm.avail_out);
			if (Z_STREAM_END == code || Z_OK != code)
			{
				break;
			}
		}
	}
	inflateEnd(&strm);
	return std::move(outBuffer);
}

