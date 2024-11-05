#include "HIDMessageAnalyser.h"

HIDMessageAnalyser::HIDMessageAnalyser(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    this->setWindowTitle(QStringLiteral("HID工具箱 v1.0.0 by Leventure."));
    this->init();
}

HIDMessageAnalyser::~HIDMessageAnalyser()
{}

void HIDMessageAnalyser::on_rdb_random_send_clicked()
{
    this->ui.widgets->setCurrentIndex(0);
}

void HIDMessageAnalyser::on_rdb_random_read_clicked()
{
    this->ui.widgets->setCurrentIndex(1);
}

void HIDMessageAnalyser::on_rad_conti_send_clicked()
{
    this->ui.widgets->setCurrentIndex(2);
}

void HIDMessageAnalyser::on_rdb_conti_read_clicked()
{
    this->ui.widgets->setCurrentIndex(3);
}

void HIDMessageAnalyser::on_btn_r_w_clicked()
{
    QString str_input_addr = this->ui.line_r_w_addr->text();
    //判断一下这个是不是16进制的四位数字
    if (!this->JudgeAddressString(str_input_addr)) return;
    QString sval = this->ui.line_r_w_sval->text();
    bool* ret = new bool();
    short short_input_addr = str_input_addr.toShort(ret, 16);
    if (*ret == false) {
        this->AddError(ErrorType::ShortConverError, str_input_addr);
        return;
    }
    short short_sval = sval.toShort(ret, 16);
    if (*ret == false) {
        this->AddError(ErrorType::ShortConverError, str_input_addr);
        return;
    }

    QString str_total = this->ptr_pack->PackageRandomMessage(short_input_addr, short_sval).toHex(' ');
    this->Add(str_input_addr, QVector<QString>{sval}, str_total);
}

void HIDMessageAnalyser::on_btn_r_r_clicked()
{
	QString str_input_addr = this->ui.line_r_r_addr->text();
	//判断一下这个是不是16进制的四位数字
	if (!this->JudgeAddressString(str_input_addr)) return;
    bool* ret = new bool() ;
    short short_input_addr = str_input_addr.toShort(ret, 16);
    if (*ret == false) {
        this->AddError(ErrorType::ShortConverError, str_input_addr);
        return;
    }
	QString str_total = this->ptr_pack->PackageRandomMessage(short_input_addr).toHex(' ');
   
	this->Add(str_input_addr, QVector<QString>{}, str_total);
}

void HIDMessageAnalyser::on_btn_c_r_clicked()
{
    //从上层读取连续参数读取地址
	QString str_input_addr = this->ui.line_c_r_addr->text();
    if (!this->JudgeAddressString(str_input_addr)) return;
	//从上层读取连续参数读取值
	QString sval = this->ui.lline_c_r_sval->text();
	QString str_total = this->ptr_pack->PackagereadContinuousMessage(str_input_addr.toShort(new bool(), 16), sval.toShort(new bool(), 16)).toHex(' ');
	this->Add(str_input_addr, QVector<QString>{sval}, str_total);
}

void HIDMessageAnalyser::on_btn_c_w_clicked()
{
	//从上层读取连续参数写入地址
    QString str_input_addr = this->ui.line_c_w_addr->text();
    if (!this->JudgeAddressString(str_input_addr)) return;
    QStringList str_sval_total = this->ui.line_c_w_sval->text().split('|');
    QVector<QString> vec_output;
    QVector<short> vec_input;
    for (auto item : str_sval_total) {
        bool* ret = new bool();
       
        short short_ret = item.toShort(ret, 16);
        if (*ret == false) {
            this->AddError(ErrorType::ShortConverError, item);
            return;
        }
        vec_input.append(short_ret);
        vec_output.append(item);
    }

	QString str_total = this->ptr_pack->PackageContinuousMessage(str_input_addr.toShort(new bool(), 16),vec_input.size(), vec_input).toHex(' ');
	this->Add(str_input_addr, vec_output, str_total);
}
QString getSubstring(const QString& input) {
    // 检查字符串是否以"c4"开头
    if (!input.startsWith("c4")) {
        return QString(); // 返回空字符串
    }

    // 从右边开始查找第一个非零字符
    int rightIndex = input.length() - 1;
    while (rightIndex >= 0 && input[rightIndex] == '0') {
        rightIndex--;
    }

    // 如果没有找到非零字符，返回空字符串
    if (rightIndex < 0) {
        return QString();
    }

    // 截取从头到找到的非零字符的字符串
    return input.left(rightIndex + 1);
}
#include "qdebug.h"
QByteArray hexStringToByteArray(const QString& hexString) {
    QByteArray byteArray;
    for (int i = 0; i < hexString.length(); i += 2) {
        QString byteString = hexString.mid(i, 2);
        bool ok;
        byteArray.append(static_cast<char>(byteString.toUInt(&ok, 16)));
        if (!ok) {
            qWarning() << "Invalid hex string!";
            return QByteArray(); // 处理错误
        }
    }
    return byteArray;
}
void HIDMessageAnalyser::on_btn_analyse_clicked()
{
    QString str_input = this->ui.text_input->toPlainText();

    //尝试解析其中文字
    QString str_analysed = getSubstring(str_input);
    QByteArray arr = hexStringToByteArray(str_analysed);
    qDebug() << arr.toHex(' ');

    this->ptr_pack->AnalyseDatas(arr);

}

void HIDMessageAnalyser::Add(const QString& address, const QVector<QString>& sval, const QString& total)
{
    this->ui.text->clear();
    QString str_addr = QStringLiteral("地址位 : 0x") + address;
    QString str_sval = QStringLiteral("值 ：");
    for (auto item : sval) {
        str_sval.append(QString(" [%1] ").arg(item));
    }
    QString str_total = QStringLiteral("总消息 : ") + total;
	QString ret = QString("%1\n%2\n%3\n%4").arg(str_addr).arg(str_sval).arg(str_total).arg(QDateTime::currentDateTime().toString());
    this->ui.text->appendPlainText(ret);
}

void HIDMessageAnalyser::AddError(const ErrorType& type, const QString& code)
{
    this->ui.text->clear();
    QString input;
    if (type == ErrorType::AddressError) {
        input = QStringLiteral("传入地址错误");
    }
    else if (type == ErrorType::ShortConverError) {
		input = QStringLiteral("无法转换为短整型");
    }

	QString ret = QString(QStringLiteral("出现错误！\n错误类型:%1\n关键参数:%2")).arg(input).arg(code);
    this->ui.text->appendPlainText(ret);
}

bool HIDMessageAnalyser::JudgeAddressString(const QString& str)
{
    // 正则表达式：^ 表示开始，0x? 表示可选的 "0x"，[0-9A-Fa-f]{1,4} 表示1到4位的十六进制数字，$ 表示结束
    QRegExp hexPattern("^(0x)?[0-9A-Fa-f]{1,4}$");
    bool bln_access = hexPattern.exactMatch(str);
    if (!bln_access) {
        this->AddError(ErrorType::AddressError, str);
    }
    return bln_access;
}

void HIDMessageAnalyser::Slot_RandomWrite(unsigned short sadd, short sval)
{
    this->ResetTableWidgetTitle();
    this->ui.tablewidget_ret->setRowCount(1);
    // 为每一列创建 QTableWidgetItem 对象
    QTableWidgetItem* item1 = new QTableWidgetItem(QString("%1").arg(QString::number(sadd,16)));
    QTableWidgetItem* item2 = new QTableWidgetItem(QString("%1").arg(sval));

    this->ui.tablewidget_ret->setItem(0, 0, item1);
    this->ui.tablewidget_ret->setItem(0, 1, item2);
    //this->ui.tablewidget_ret->setItem(0,0,"")

    this->ui.line_ret_addr_type->setText(QStringLiteral("离散读取"));
    this->ui.line_ret_addr->setText(QString::number(sadd, 16));

    QString str_ret;
    str_ret = QString(QStringLiteral("解析结果:\n地址位 : %1 \n参数 : %2;").arg(QString::number(sadd,16)).arg(sval));
    this->ui.text_analyse->clear();
    this->ui.text_analyse->appendPlainText(str_ret);
}

void HIDMessageAnalyser::Slot_RandomRead(unsigned short sadd, short sval)
{  
    this->ResetTableWidgetTitle();
    this->ui.tablewidget_ret->setRowCount(1);
    // 为每一列创建 QTableWidgetItem 对象
    QTableWidgetItem* item1 = new QTableWidgetItem(QString("%1").arg(QString::number(sadd, 16)));
    QTableWidgetItem* item2 = new QTableWidgetItem(QString("%1").arg(sval));

    this->ui.tablewidget_ret->setItem(0, 0, item1);
    this->ui.tablewidget_ret->setItem(0, 1, item2);
    //this->ui.tablewidget_ret->setItem(0,0,"")

    this->ui.line_ret_addr_type->setText(QStringLiteral("离散写入"));
    this->ui.line_ret_addr->setText(QString::number(sadd, 16));

    QString str_ret;
    str_ret = QString(QStringLiteral("解析结果:\n地址位 : %1 \n参数 : %2;").arg(QString::number(sadd, 16)).arg(sval));
    this->ui.text_analyse->clear();
    this->ui.text_analyse->appendPlainText(str_ret);
}

void HIDMessageAnalyser::Slot_ContinuousRead(unsigned short sadd, const QVector<short>& vec_sval)
{
    this->ResetTableWidgetTitle();
    int length = vec_sval.size();
    this->ui.tablewidget_ret->setRowCount(length);
    QString str_ret;
    QString svals;
    for (int i = 0; i < length; ++i) {
        // 为每一列创建 QTableWidgetItem 对象
        QTableWidgetItem* item1 = new QTableWidgetItem(QString("%1").arg(QString::number(sadd + i, 16)));
        QTableWidgetItem* item2 = new QTableWidgetItem(QString("%1").arg(vec_sval[i]));
        this->ui.tablewidget_ret->setItem(i, 0, item1);
        this->ui.tablewidget_ret->setItem(i, 1, item2);
        svals.append(QString::number(sadd + i,16)  + ":" + QString::number(vec_sval[i]) + '\n');
    }
    str_ret = QString(QStringLiteral("解析结果:\n地址位 : %1 \n参数 : %2;").arg(QString::number(sadd, 16)).arg(svals));
    this->ui.line_ret_addr_type->setText(QStringLiteral("连续写入"));
    this->ui.line_ret_addr->setText(QString::number(sadd, 16));


    this->ui.text_analyse->appendPlainText(str_ret);
}

void HIDMessageAnalyser::ResetTableWidgetTitle()
{
    this->ui.tablewidget_ret->clear();
    this->ui.tablewidget_ret->setColumnCount(2);
    QStringList headerLabels;
    headerLabels << QStringLiteral("地址") << QStringLiteral("参数");
    this->ui.tablewidget_ret->setHorizontalHeaderLabels(headerLabels);
}



void HIDMessageAnalyser::init()
{
    this->ptr_pack = QSharedPointer<DataPackage>(new DataPackage(this));
    connect(this->ptr_pack.data(), &DataPackage::Sig_RandomRead, this, &HIDMessageAnalyser::Slot_RandomRead);
    connect(this->ptr_pack.data(), &DataPackage::Sig_RandomWrite, this, &HIDMessageAnalyser::Slot_RandomWrite);
    connect(this->ptr_pack.data(), &DataPackage::Sig_ContinuousRead, this, &HIDMessageAnalyser::Slot_ContinuousRead);


}
