#include "DataPackage.h"
#include "qdebug.h"
void DataPackage::AnalyseRandomWrite(unsigned char* buffer)
{
    int value_length = (buffer[1] - 7) / 4;
    for (int i = 0; i < value_length; i++) {
        unsigned short sadd = buffer[5 + i * 4] | buffer[6 + i * 4] << 8;
        short sval = buffer[7 + i * 4] | buffer[8 + i * 4] << 8;
        
        emit this->Sig_RandomWrite(sadd, sval);
		//DEBUG_LOG(__FUNCTION__ << "sadd:" << sadd << "sval:" << sval);
		//qDebug() << __FUNCTION__ << "sadd:" << sadd << "sval:" << sval;
    }
}

void DataPackage::AnalyseRandomRead(unsigned char* buffer)
{
    unsigned short sadd = buffer[5] | buffer[6] << 8;
    short sval = buffer[7] | buffer[8] << 8;
	emit this->Sig_RandomRead(sadd, sval);
	//qDebug() << __FUNCTION__ << "sadd:" << sadd << "sval:" << sval;
}

void DataPackage::AnalyseContinuousRead(const QByteArray& data)
{
	// 检查最小长度，至少需要14个字节（包含CRC）
	if (data.size() < 14) {
		// 数据长度不足，处理错误
		return;
	}
	unsigned short sadd = 0x00;
	QVector<short> sval;

	const unsigned char* buffer = reinterpret_cast<const unsigned char*>(data.constData());

	sadd = buffer[5] | buffer[6] << 8;
	// 从buffer中提取基本信息
	short length = buffer[7] | buffer[8] << 8;
	for (int i = 0; i < length; i++) {
		short val_temp = buffer[9 + (i * 2)] | buffer[10 + (i * 2)] << 8;
		sval.append(val_temp);
	}
	// 解析成功，发送信号
	emit Sig_ContinuousRead(sadd, sval);
}
void DataPackage::AnalyseContinuousWrite(const QByteArray& data)
{
	// 检查最小长度，至少需要14个字节（包含CRC）
	if (data.size() < 15) {
		// 数据长度不足，处理错误
		return;
	}
	unsigned short sadd = 0x00;
	QVector<short> sval;

	const unsigned char* buffer = reinterpret_cast<const unsigned char*>(data.constData());



}
QQueue<QByteArray> DataPackage::SplitCoalescingPacket(const QByteArray& arr)
{
	QQueue<QByteArray> packetQueue;
	int i = 0;

	while (i < arr.size()) {
		// 检查是否有足够的字节来读取包头和长度字段
		if (i + 3 > arr.size()) {
			// 剩余的字节不足以读取包头和长度
			break;
		}

		// 读取包头和长度
		unsigned char fixedByte = static_cast<unsigned char>(arr[i]);
		unsigned char lengthLow = static_cast<unsigned char>(arr[i + 1]);
		unsigned char lengthHigh = static_cast<unsigned char>(arr[i + 2]);

		// 确认包头是0xC4
		if (fixedByte != 0xC4) {
			// 如果不是预期的包头，跳过该字节
			i++;
			continue;
		}

		// 计算数据包的总长度（包含包头和 CRC）
		unsigned short length = static_cast<unsigned short>(lengthLow) | (static_cast<unsigned short>(lengthHigh) << 8);

		// 检查是否有足够的字节读取整个数据包
		if (i + length > arr.size()) {
			// 剩余的字节不足以读取完整的数据包
			break;
		}

		// 提取完整的数据包
		QByteArray packet = arr.mid(i, length);
		//qDebug() << packet.toHex();
		// 提取接收到的 CRC（最后两个字节）
		unsigned short receivedCrc = static_cast<unsigned short>(static_cast<unsigned char>(packet[length - 2])) |
			(static_cast<unsigned short>(static_cast<unsigned char>(packet[length - 1])) << 8);

		// 计算 CRC，范围是除了最后两个 CRC 字节之外的所有字节
		unsigned short calculatedCrc = this->CRC(reinterpret_cast<const unsigned char*>(packet.constData()), length - 2);

		if (receivedCrc == calculatedCrc) {
			// CRC 校验通过，将数据包加入队列
			packetQueue.enqueue(packet);
		}
		else {
			qDebug() << __FUNCTION__ << "Message Analyse SplitFailed" << packet.toHex(' ');
		}

		// 移动到下一个数据包的位置
		i += length;
	}

	return packetQueue;
}



DataPackage::DataPackage(QObject* parent) : QObject(parent)
{

}

DataPackage::~DataPackage()
{

}

unsigned short DataPackage::CRC(const unsigned char* data, size_t length)
{
	if (data == nullptr || length < 1) return 0;

	unsigned short checkValue = 0;
	for (int i = 0; i < length; i++)
	{
		checkValue += data[i];
	}
	checkValue = ~checkValue + 1;
	return checkValue;
}

QByteArray DataPackage::PackageRandomMessage(unsigned short sadd, short sval)
{
	unsigned char buffer[65];
	memset(buffer, 0, 65);
	buffer[0] = report;
	buffer[1] = 0xc4;
	buffer[2] = 0x0b;
	buffer[3] = 0x00;
	buffer[4] = 0x00;
	buffer[5] = CC_PARAM_RAND_WR;

	buffer[6] = sadd & 0xff;
	buffer[7] = (sadd >> 8) & 0xff;

	buffer[8] = sval & 0xff;
	buffer[9] = (sval >> 8) & 0xff;

	unsigned short valueCheck = DataPackage::CRC(buffer, 10);
	buffer[10] = valueCheck & 0xff;
	buffer[11] = (valueCheck >> 8) & 0xff;

	QByteArray byteArray(reinterpret_cast<const char*>(buffer), 65);
	return byteArray;
}

QByteArray DataPackage::PackageRandomMessageQVector(QVector<unsigned short> sadd, QVector<short> sval)
{
	unsigned char buffer[65];
	memset(buffer, 0, 65);
	buffer[0] = report;
	buffer[1] = 0xc4;
	buffer[2] = 7 + sval.count() * 4;
	buffer[3] = 0x00;
	buffer[4] = 0x00;
	buffer[5] = CC_PARAM_RAND_WR;

	for (int i = 0; i < sadd.count(); i++) {

		buffer[6 + i * 4] = sadd.at(i) & 0xff;
		buffer[7 + i * 4] = (sadd.at(i) >> 8) & 0xff;
		buffer[8 + i * 4] = sval.at(i) & 0xff;
		buffer[9 + i * 4] = (sval.at(i) >> 8) & 0xff;
	}

	unsigned short valueCheck = DataPackage::CRC(buffer, 6 + sval.count() * 4);
	buffer[10 + (sadd.count() - 1) * 4] = valueCheck & 0xff;
	buffer[11 + (sadd.count() - 1) * 4] = (valueCheck >> 8) & 0xff;

	QByteArray byteArray(reinterpret_cast<const char*>(buffer), sizeof(buffer));

	return byteArray;
}

QByteArray DataPackage::PackageRandomMessage(unsigned short sadd)
{
	unsigned char buffer[65];
	memset(buffer, 0, 65);
	int num = 0;
	buffer[num++] = report;
	buffer[num++] = 0xc4;
	buffer[num++] = 0x09;
	buffer[num++] = 0x00;
	buffer[num++] = 0x00;
	buffer[num++] = CC_PARAM_RAND_RD;

	buffer[num++] = sadd & 0xff;
	buffer[num++] = (sadd >> 8) & 0xff;

	unsigned short valueCheck = DataPackage::CRC(buffer, num);
	buffer[num++] = valueCheck & 0xff;
	buffer[num++] = (valueCheck >> 8) & 0xff;

	QByteArray byteArray(reinterpret_cast<const char*>(buffer), sizeof(buffer));

	return byteArray;
}

QByteArray DataPackage::PackageRandomMessageQVector(QVector<unsigned short> sadd)
{
	if (sadd.count() == 0)return QByteArray();

	unsigned char buffer[65];
	memset(buffer, 0, 65);
	buffer[0] = report;
	buffer[1] = 0xc4;
	buffer[2] = 7 + sadd.count() * 2;
	buffer[3] = 0x00;
	buffer[4] = 0x00;
	buffer[5] = CC_PARAM_RAND_RD;

	for (int i = 0; i < sadd.count(); i++) {

		buffer[6 + i * 2] = sadd.at(i) & 0xff;
		buffer[7 + i * 2] = (sadd.at(i) >> 8) & 0xff;
	}

	unsigned short valueCheck = DataPackage::CRC(buffer, 6 + sadd.count() * 2);
	buffer[8 + (sadd.count() - 1) * 2] = valueCheck & 0xff;
	buffer[9 + (sadd.count() - 1) * 2] = (valueCheck >> 8) & 0xff;

	QByteArray byteArray(reinterpret_cast<const char*>(buffer), sizeof(buffer));

	return byteArray;
}

QByteArray DataPackage::PackageContinuousMessage(unsigned short sadd, short length, QVector<short> sval)
{
	if (sval.count() == 0)
		return QByteArray();

	unsigned char buffer[65];
	memset(buffer, 0, 65);
	buffer[0] = report;
	buffer[1] = 0xc4;
	buffer[2] = 11 + sval.count() * 2;
	buffer[3] = 0x00;
	buffer[4] = 0x00;
	buffer[5] = CC_PARAM_SEQU_WR;

	buffer[6] = sadd & 0xff;
	buffer[7] = (sadd >> 8) & 0xff;

	buffer[8] = length & 0xff;
	buffer[9] = (length >> 8) & 0xff;

	for (int i = 0; i < sval.count(); i++) {

		buffer[10 + i * 2] = sval.at(i) & 0xff;
		buffer[11 + i * 2] = (sval.at(i) >> 8) & 0xff;
	}

	unsigned short valueCheck = DataPackage::CRC(buffer, 10 + sval.count() * 2);
	buffer[12 + (sval.count() - 1) * 2] = valueCheck & 0xff;
	buffer[13 + (sval.count() - 1) * 2] = (valueCheck >> 8) & 0xff;

	QByteArray byteArray(reinterpret_cast<const char*>(buffer), sizeof(buffer));

	return byteArray;
}

QByteArray DataPackage::PackagereadContinuousMessage(unsigned short sadd, short sval)
{
	unsigned char buffer[65];
	memset(buffer, 0, 65);
	buffer[0] = report;
	buffer[1] = 0xc4;
	buffer[2] = 0x0b;
	buffer[3] = 0x00;
	buffer[4] = 0x00;
	buffer[5] = CC_PARAM_SEQU_RD;

	buffer[6] = sadd & 0xff;
	buffer[7] = (sadd >> 8) & 0xff;

	buffer[8] = sval & 0xff;
	buffer[9] = (sval >> 8) & 0xff;

	unsigned short valueCheck = DataPackage::CRC(buffer, 10);
	buffer[10] = valueCheck & 0xff;
	buffer[11] = (valueCheck >> 8) & 0xff;

	QByteArray byteArray(reinterpret_cast<const char*>(buffer), sizeof(buffer));

	return byteArray;
}

void DataPackage::AnalyseDatas(QByteArray datas)
{
	//输入的参数可能粘包，所以需要做一些解析工作
	auto rets = this->SplitCoalescingPacket(datas);
	for (const QByteArray& arr : rets) {
		unsigned char buffer[64];
		memcpy(buffer, arr.data(), qMin(arr.size(), 64));
		if (buffer[4] == CC_PARAM_RAND_WR) {
			this->AnalyseRandomWrite(buffer);
		}
		else if (buffer[4] == CC_PARAM_RAND_RD) {
			this->AnalyseRandomRead(buffer);
		}
		else if (buffer[4] == CC_PARAM_SEQU_RD) {
			this->AnalyseContinuousRead(datas);
		}
		else if (buffer[4] == CC_PARAM_SEQU_WR) {
			this->AnalyseContinuousRead(datas);
		}
		//delete[] buffer;
	}
	
}

void DataPackage::AnalyseDatas_Analyser(QByteArray datas)
{
	//输入的参数可能粘包，所以需要做一些解析工作
	auto rets = this->SplitCoalescingPacket(datas);
	for (const QByteArray& arr : rets) {
		unsigned char buffer[64];
		memcpy(buffer, arr.data(), qMin(arr.size(), 64));
		if (buffer[4] == CC_PARAM_RAND_WR) {
			this->AnalyseRandomWrite(buffer);
		}
		else if (buffer[4] == CC_PARAM_RAND_RD) {
			this->AnalyseRandomRead(buffer);
		}
		else if (buffer[4] == CC_PARAM_SEQU_RD) {
			this->AnalyseContinuousRead(datas);
		}
		else if (buffer[5] == CC_PARAM_SEQU_WR) {

		}
	}
}

