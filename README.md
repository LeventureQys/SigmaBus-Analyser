这是一个非常基础的HID Message Analyser，主要的作用是用于解析SigmaBus的消息，用于调试和测试，所有的核心内容都封装在DataPackage.h 和 .cpp中，主要内容见接口

#pragma once


#define report 0x00
#define CC_ERROR_CODE 0x02 //error command
#define CC_PARAM_RAND_WR 0x03 // random write
#define CC_PARAM_RAND_RD 0x04 // random read
#define CC_PARAM_SEQU_WR 0x05 // continuity write(two bytes)
#define CC_PARAM_SEQU_RD 0x06 // continuity read（two bytes）
#define CC_PARAM_RAND_WR_EXT 0x07 // random write（four bytes）
#define CC_PARAM_RAND_RD_EXT 0x08 // random read （four bytes）
#define CC_PARAM_SEQU_WR_EXT 0x09 // continuity write（four bytes）
#define CC_PARAM_SEQU_RD_EXT 0x0A // continuity read（four bytes）
#define CC_DFU 0x0B //firmware upgrade command
#define CC_CTRL 0x0C //control command
#include "qbytearray.h"
#include "qvector.h"
#include "qobject.h"
#include "qqueue.h"

/// <summary>
/// DataPackage 此类只用于组装数据和解析数据用，所有的方法都是静态方法 
/// </summary>
class DataPackage : public QObject {
	Q_OBJECT
public:
	DataPackage(QObject* parent);
	~DataPackage();

	static unsigned short CRC(const unsigned char*, size_t);
	/// <summary>
	/// 随机写入
	/// </summary>
	/// <param name="sadd">地址位</param>
	/// <param name="sval">值</param>
	/// <returns></returns>
	QByteArray PackageRandomMessage(unsigned short sadd, short sval);
	/// <summary>
	/// 连续发送随机写入请求
	/// </summary>
	/// <param name="sadd">地址组</param>
	/// <param name="sval">值组</param>
	/// <returns></returns>
	QByteArray PackageRandomMessageQVector(QVector<unsigned short> sadd, QVector<short> sval);

	/// <summary>
	/// 打包随机读取请求
	/// </summary>
	/// <param name="sadd">读取地址</param>
	/// <returns></returns>
	QByteArray PackageRandomMessage(unsigned short sadd);

	/// <summary>
	/// 连续打包随机读取请求
	/// </summary>
	/// <param name="sadd">读取地址</param>
	/// <returns></returns>
	QByteArray PackageRandomMessageQVector(QVector<unsigned short> sadd);

	/// <summary>
	/// 连续写入请求
	/// </summary>
	/// <param name="sadd">连续写入请求起点</param>
	/// <param name="length">连续写入请求起点</param>
	/// <param name="sval">值</param>
	/// <returns></returns>
	QByteArray PackageContinuousMessage(unsigned short sadd, short length, QVector<short> sval);

	/// <summary>
	/// 连续读取请求
	/// </summary>
	/// <param name="sadd">连续读取请求起点</param>
	/// <param name="sval">连续读取请求的值</param>
	/// <returns></returns>
	QByteArray PackagereadContinuousMessage(unsigned short sadd, short sval);

	void AnalyseDatas(QByteArray datas);
	//这个是直接解析，不是根据输入来的
	void AnalyseDatas_Analyser(QByteArray datas);
signals:
	void Sig_RandomWrite(unsigned short sadd, short sval);
	void Sig_RandomRead(unsigned short sadd, short sval);
	void Sig_ContinuousRead(unsigned short sadd, const QVector<short>& vec_sval);
private:
	void AnalyseRandomWrite(unsigned char* data);
	void AnalyseRandomRead(unsigned char* data);
	void AnalyseContinuousRead(const QByteArray& data);
	void AnalyseContinuousWrite(const QByteArray& data);

	QQueue<QByteArray> SplitCoalescingPacket(const QByteArray& arr);		//处理粘包数据 
};
