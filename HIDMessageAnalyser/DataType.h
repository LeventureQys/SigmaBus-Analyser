#pragma once
#include "qobject.h"
#include "QJsonObject"
#include <QJsonDocument>
#include "qjsonarray.h"
#include "qdebug"
#include <QColor>
#include <QMap>
#include <QList>
#include "qsharedpointer.h"
#include "qdatetime.h"
enum class ScenseType {
    rawsound,
    game,
    dubbing,
    live,
    recording
};

QString toString(const ScenseType& type);
QString toString_Translator(const ScenseType& type);
enum class ReduceNoisyLevel {
    low = 0,
    mid = 1,
    high = 2,
    custom = 3
};

enum class LightEffect {
    Always = 0,
    Hooxi = 2,  //呼吸灯
    Moving = 1
};

enum class MixtureLevel {
    light = 0,
    middle = 1,
    deep = 2
};

struct STPoint {
    short point_id = 0;

    double Gain = 0;
    double Freq = 0;
    double Q = 0;

    bool blnBypass = false;
    qint8 bandType;
    QString toJson() const;
    bool fromJson(const QString& jsonStr);
};

struct ScenseSetting {
    ScenseSetting();
    QList<STPoint> list_points;
    bool blnMixture = false;
    MixtureLevel mixture_level = MixtureLevel::light;
    QString strConfigName;
    
    QString toJson();
    bool fromJson(const QString& jsonStr);
};

struct LightSetting {
    QColor lightColor;
    LightEffect effect;
    bool blnOpen = true;
    //亮度
    int light_level = 0;
	short index_color_fixed = 0;
    //这里实际上并不是纯粹的自定义灯光，而是所有灯光都存放在这里，其中0-7为默认灯光，8-12为自定义灯光
    QMap<int, QColor> map_edit_colors;
    QList<QColor> list_edit_colors;
    QString toJson() const;
    bool fromJson(const QString& jsonStr);
};

/// <summary>
/// 还有个AI变音模式
/// </summary>
struct AISoundTransfer {
    qint32 index = 0;
    QString strName = "";
    QString imagePath = "";
    QDateTime limitTime;
    //TODO : 这里需要做一个超时判断，目前这个时间都是永久，暂时不做可配置 
    bool blnEnable = true;
    bool blnTimeout = false;
    QString toJson();
    void FromJson(const QString& strJson);
};

struct ProductSetting {
    ProductSetting();
    int MicGain = 36;
    bool blnMicMute = false;
    int HeadPhoneVolume = 36;
    bool blnNoisyReduce = false;
    int NoisyReduceCoeffi = 0;
    bool blnSpy = false;
    QString SNCode = "";
    ReduceNoisyLevel noisy_reduce_level = ReduceNoisyLevel::low;
    //QSharedPointer<ScenseSetting> default_sence;
    QMap<QString, QSharedPointer<ScenseSetting>> map_configs;
    LightSetting Settings;
	QMap<qint8,QSharedPointer<AISoundTransfer>> map_ai_setting;
    QString toJson() const;
    bool fromJson(const QString& jsonStr);
};
const unsigned short addr_ScenseIndex = 0x2022;


const unsigned short addr_MicLevelSig = 0x2080;
const unsigned short addr_MicLevelSigMute = 0x2081;
const unsigned short addr_MicGain = 0x207E;
const unsigned short addr_MicMute = 0x207D;

const unsigned short addr_HPVolume = 0x207F;
const unsigned short addr_NoisyRecudeToogle = 0x2084;
const unsigned short addr_NoisyReduceLevel = 0x2085;
const unsigned short addr_NoisyReduceCoeffi = 0x2086;
const unsigned short addr_MixtureCoeffi = 0x2083;
const unsigned short addr_MixtureToogle = 0x2082;
const unsigned short addr_LightShowToogle = 0x2089;
const unsigned short addr_LightLevel = 0x208A;
const unsigned short addr_LightRGBMode = 0x208B;
const unsigned short addr_LightColorCurrentFixed = 0x208C;
const unsigned short addr_LightColorCurrentBreathe = 0x208D;
const unsigned short addr_LightColorCount = 0x208E;

const unsigned short addr_FirstLightCustomFirst = 0x208f;

const unsigned short addr_Recovery = 0x20AE;

const unsigned short addr_firmware_rev = 0x000B;
const unsigned short addr_txrx_pair = 0x2087;
const unsigned short addr_txrx_repairing = 0x20AD;
const unsigned short addr_SNCode = 0x001E;
//const unsigned short addr_levelsignal_mute = 0x2081;
const QString str_headphone_volume_what = "";


