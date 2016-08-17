#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QScriptEngine>
#include <QScriptValue>

class QNetworkReply;

struct WeatherInfo{
    QString date;
    QString week;
    QString type;
    QString curTemp;
    QString hightemp;
    QString lowtemp;
    QString aqi;
    QString fengxiang;
    QString fengli;
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

private slots:
    void replyFinished(QNetworkReply *reply);

private:
    Ui::Widget *ui;

    QStringList provinceList;
    QStringList cityList;
//    QScriptEngine engine;
//    QScriptValue weatherValue;
};

#endif // WIDGET_H
