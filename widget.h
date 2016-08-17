#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

class QNetworkAccessManager;
class QNetworkRequest;
class QNetworkReply;

struct WeatherInfo{
    QString date;
    QString week;
    QString type;
    QString curTemp;
    QString hightemp;
    QString lowtemp;
    QString fengli;
    QString fengxiang;
    QString aqi;
};

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

    void init();
    void setTableHorizontalHeader();
    void operateSql();
    void setNetworkRequest(QNetworkRequest &request, QString cityName, QString cityId);//设置网络请求
    void getHistoryWeatherInfo(QJsonObject data);                   //获取历史天气信息
    void getTodayWeatherInfo(QJsonObject data);                     //获取当前天气信息
    void getForecastWeatherInfo(QJsonObject data);                  //获取未来天气信息
    void getOtherInfo(QJsonObject data);                            //获取其他天气信息

private slots:
    void replyFinished(QNetworkReply *reply);

private:
    Ui::Widget *ui;

    QStringList provinceList;
//    QStringList cityList;

    QNetworkAccessManager *manage;
};

#endif // WIDGET_H
