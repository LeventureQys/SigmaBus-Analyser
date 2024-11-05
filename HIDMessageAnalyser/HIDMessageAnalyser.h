#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_HIDMessageAnalyser.h"
#include "DataPackage.h"
#include "qdatetime.h"
class HIDMessageAnalyser : public QMainWindow
{
    Q_OBJECT
        enum class ErrorType {
        AddressError,
        ShortConverError,         //数字转换失败
    };
public:
    HIDMessageAnalyser(QWidget *parent = nullptr);
    ~HIDMessageAnalyser();
private slots:

    //------data type ------
    void on_rdb_random_send_clicked();
    void on_rdb_random_read_clicked();
    void on_rad_conti_send_clicked();
    void on_rdb_conti_read_clicked();

    void on_btn_r_w_clicked();
    void on_btn_r_r_clicked();
	void on_btn_c_r_clicked();
	void on_btn_c_w_clicked();

    void on_btn_analyse_clicked();
    //地址位已包含0x，传入时不需要单独给0x
	void Add(const QString& address, const QVector<QString>& sval, const QString& total);
    void AddError(const ErrorType& type,const QString& code);

    bool JudgeAddressString(const QString& str);
    void Slot_RandomWrite(unsigned short sadd, short sval);
    void Slot_RandomRead(unsigned short sadd, short sval);
    void Slot_ContinuousRead(unsigned short sadd, const QVector<short>& vec_sval);
    void ResetTableWidgetTitle();
private:
    QSharedPointer<DataPackage> ptr_pack;
    void init();
    Ui::HIDMessageAnalyserClass ui;
};
