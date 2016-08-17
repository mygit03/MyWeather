#include "widget.h"
#include "ui_widget.h"

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QTableWidgetItem>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>

QNetworkRequest network_request;

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    init();

    manage = new QNetworkAccessManager(this);
    QString cityName = "朝阳";
    QString cityId = "101010300";

    //设置网络请求
    setNetworkRequest(network_request, cityName, cityId);

    connect(manage,SIGNAL(finished(QNetworkReply *)),this,SLOT(replyFinished(QNetworkReply*)));
    /*发送get网络请求*/
    manage->get(network_request);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::init()
{
    QString dateTime = QDateTime::currentDateTime().toString("yyyy.MM.dd ddd");
    ui->label_dateTime->setText(dateTime);

    ui->tableWidget->setColumnWidth(0, 109);
    ui->tableWidget->setColumnWidth(2, 80);
    ui->tableWidget->setColumnWidth(3, 80);
    ui->tableWidget->setColumnWidth(4, 80);
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);

    ui->tableWidget_history->setColumnWidth(0, 109);
    ui->tableWidget_history->setColumnWidth(2, 80);
    ui->tableWidget_history->setColumnWidth(3, 80);
    ui->tableWidget_history->setColumnWidth(4, 80);
    ui->tableWidget_history->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget_history->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget_history->setColumnHidden(2, true);

    ui->tableWidget_forecast->setColumnWidth(0, 109);
    ui->tableWidget_forecast->setColumnWidth(2, 80);
    ui->tableWidget_forecast->setColumnWidth(3, 80);
    ui->tableWidget_forecast->setColumnWidth(4, 80);
    ui->tableWidget_forecast->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget_forecast->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget_forecast->setColumnHidden(2, true);

    setTableHorizontalHeader();         //设置表头

    //设置选中行的背景色
    QString styleSheet = "QTableView::item:selected{color:white;background:rgb(34, 175, 75);}";
    ui->tableWidget->setStyleSheet(styleSheet);

//    operateSql();     //暂时获取数据库
}

void Widget::setTableHorizontalHeader()
{
    QStringList hList;
    hList << "日期" << "天气" << "当前温度" << "最高温度" << "最低温度" << "风力" << "风向"/* << "空气质量指数"*/;
    ui->tableWidget->setHorizontalHeaderLabels(hList);
    ui->tableWidget_history->setHorizontalHeaderLabels(hList);
    ui->tableWidget_forecast->setHorizontalHeaderLabels(hList);
}

void Widget::operateSql()
{
    provinceList.clear();
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    QString dbPath = QApplication::applicationDirPath();
    dbPath.append("/db_weather.db");
    qDebug() << "path:" << dbPath;
    db.setDatabaseName(dbPath);
    if (!db.open()){
        qDebug() << "database open fail!";
    }else{
        qDebug() << "database open success!";
        QSqlQuery q;
        //查询数据库中的有哪些表
        q.exec("SELECT name FROM sqlite_master WHERE type='table' ORDER BY name;");
//        q.exec("select * from provinces");

        int count = q.size();
        qDebug() << "size:" << count;
        while (q.next()){
            for(int i = 0; i < count; i++){
                provinceList.append(q.value(i).toString());
                qDebug() << "value:" << i << q.value(i);
            }
        }
    }
    db.close();
    qDebug()<<"database closed!";
}

void Widget::setNetworkRequest(QNetworkRequest &request, QString cityName, QString cityId)
{
    /*设置发送数据*/
#if 0
    request.setUrl(QUrl(QString("http://api.map.baidu.com/location/ip?ak=%1&coor=bd09ll").arg("54GzQbyspseUfRUbvDdVMKQW")));
#else
    request.setUrl(QUrl(QString("http://apis.baidu.com/apistore/weatherservice/recentweathers?cityname=%1&cityid=%2")
                                .arg(cityName).arg(cityId)));
    request.setRawHeader("apikey", "b446bb51d329b1098b008568231a772b");
#endif
}

void Widget::getHistoryWeatherInfo(QJsonObject data)
{
    int cols = ui->tableWidget_history->columnCount();
    QJsonArray historyArray = data.value("retData").toObject().value("history").toArray();
    int size = historyArray.size();
    qDebug() << "size;" << size;
    for(int i=0; i < size; i++)
    {
        QJsonObject history = historyArray.at(i).toObject();
        WeatherInfo historyInfo;
        historyInfo.date = history.value("date").toString();
        historyInfo.week = history.value("week").toString();
        historyInfo.type = history.value("type").toString();
        historyInfo.curTemp = history.value("curTemp").toString();
        historyInfo.hightemp = history.value("hightemp").toString();
        historyInfo.lowtemp = history.value("lowtemp").toString();
        historyInfo.fengli = history.value("fengli").toString();
        historyInfo.fengxiang = history.value("fengxiang").toString();
        historyInfo.aqi = history.value("aqi").toString();
        qDebug() << "ddd" << historyInfo.date + historyInfo.week << historyInfo.type << historyInfo.curTemp
                 << historyInfo.hightemp << historyInfo.lowtemp << historyInfo.fengli << historyInfo.fengxiang
                 << historyInfo.aqi;

        QStringList historyInfoList;
        historyInfoList << historyInfo.date + historyInfo.week << historyInfo.type << historyInfo.curTemp
                        << historyInfo.hightemp << historyInfo.lowtemp << historyInfo.fengli << historyInfo.fengxiang
                        << historyInfo.aqi;

        int rows = ui->tableWidget_history->rowCount();
        ui->tableWidget_history->setRowCount(rows + 1);
        for(int i = 0; i < cols; i++){
            QTableWidgetItem *item = new QTableWidgetItem(historyInfoList.at(i));
            item->setTextAlignment(Qt::AlignCenter);
            ui->tableWidget_history->setItem(rows, i, item);
        }
    }
}

void Widget::getTodayWeatherInfo(QJsonObject data)
{
    int cols = ui->tableWidget->columnCount();
    QJsonObject today = data.value("retData").toObject().value("today").toObject();
    WeatherInfo todayInfo;
    todayInfo.date = today.value("date").toString();
    todayInfo.week = today.value("week").toString();
    todayInfo.type = today.value("type").toString();
    todayInfo.curTemp = today.value("curTemp").toString();
    todayInfo.hightemp = today.value("hightemp").toString();
    todayInfo.lowtemp = today.value("lowtemp").toString();
    todayInfo.fengli = today.value("fengli").toString();
    todayInfo.fengxiang = today.value("fengxiang").toString();
    todayInfo.aqi = today.value("aqi").toString();
    qDebug() << "ddd" << todayInfo.date + todayInfo.week << todayInfo.type << todayInfo.curTemp
             << todayInfo.hightemp << todayInfo.lowtemp << todayInfo.fengli << todayInfo.fengxiang
             << todayInfo.aqi;

    QStringList todayInfoList;
    todayInfoList << todayInfo.date + todayInfo.week << todayInfo.type << todayInfo.curTemp
                  << todayInfo.hightemp << todayInfo.lowtemp << todayInfo.fengli << todayInfo.fengxiang
                  << todayInfo.aqi;

    int rows = ui->tableWidget->rowCount();
    ui->tableWidget->setRowCount(rows + 1);

    ui->tableWidget->selectRow(rows);
    for(int i = 0; i < cols; i++){
        QTableWidgetItem *item = new QTableWidgetItem(todayInfoList.at(i));
        item->setTextAlignment(Qt::AlignCenter);
//        item->setBackgroundColor(QColor(34, 175, 75));
        ui->tableWidget->setItem(rows, i, item);
    }
}

void Widget::getForecastWeatherInfo(QJsonObject data)
{
    int cols = ui->tableWidget_forecast->columnCount();
    QJsonArray forecastArray = data.value("retData").toObject().value("forecast").toArray();
    int size = forecastArray.size();
    qDebug() << "size;" << size;
    for(int i=0; i < size; i++)
    {
        QJsonObject forecast = forecastArray.at(i).toObject();
        WeatherInfo forecastInfo;
        forecastInfo.date = forecast.value("date").toString();
        forecastInfo.week = forecast.value("week").toString();
        forecastInfo.type = forecast.value("type").toString();
        forecastInfo.curTemp = forecast.value("curTemp").toString();
        forecastInfo.hightemp = forecast.value("hightemp").toString();
        forecastInfo.lowtemp = forecast.value("lowtemp").toString();
        forecastInfo.fengli = forecast.value("fengli").toString();
        forecastInfo.fengxiang = forecast.value("fengxiang").toString();
        forecastInfo.aqi = forecast.value("aqi").toString();
        qDebug() << "ddd" << forecastInfo.date + forecastInfo.week << forecastInfo.type << forecastInfo.curTemp
                 << forecastInfo.hightemp << forecastInfo.lowtemp << forecastInfo.fengli << forecastInfo.fengxiang
                 << forecastInfo.aqi;

        QStringList forecastInfoList;
        forecastInfoList << forecastInfo.date + forecastInfo.week << forecastInfo.type << forecastInfo.curTemp
                         << forecastInfo.hightemp << forecastInfo.lowtemp << forecastInfo.fengli << forecastInfo.fengxiang
                         << forecastInfo.aqi;

        int rows = ui->tableWidget_forecast->rowCount();
        ui->tableWidget_forecast->setRowCount(rows + 1);
        for(int i = 0; i < cols; i++){
            QTableWidgetItem *item = new QTableWidgetItem(forecastInfoList.at(i));
            item->setTextAlignment(Qt::AlignCenter);
            ui->tableWidget_forecast->setItem(rows, i, item);
        }
    }
}

void Widget::getOtherInfo(QJsonObject data)
{
    QJsonArray otherArray = data.value("retData").toObject().value("today").toObject()
            .value("index").toArray();

    QString index, details;
    QJsonObject otherInfo = otherArray.at(0).toObject();
    index = otherInfo.value("index").toString();
    details = otherInfo.value("details").toString();
    ui->label1_1->setText(index);
    ui->label1_2->setText(details);
    qDebug() << "details:" << details;

    otherInfo = otherArray.at(1).toObject();
    index = otherInfo.value("index").toString();
    details = otherInfo.value("details").toString();
    ui->label2_1->setText(index);
    ui->label2_2->setText(details);
    qDebug() << "details:" << details;

    otherInfo = otherArray.at(2).toObject();
    index = otherInfo.value("index").toString();
    details = otherInfo.value("details").toString();
    ui->label3_1->setText(index);
    ui->label3_2->setText(details);
    qDebug() << "details:" << details;

    otherInfo = otherArray.at(3).toObject();
    index = otherInfo.value("index").toString();
    details = otherInfo.value("details").toString();
    ui->label4_1->setText(index);
    ui->label4_2->setText(details);
    qDebug() << "details:" << details;

    otherInfo = otherArray.at(4).toObject();
    index = otherInfo.value("index").toString();
    details = otherInfo.value("details").toString();
    ui->label5_1->setText(index);
    ui->label5_2->setText(details);
    qDebug() << "details:" << details;

    otherInfo = otherArray.at(5).toObject();
    index = otherInfo.value("index").toString();
    details = otherInfo.value("details").toString();
    ui->label6_1->setText(index);
    ui->label6_2->setText(details);
    qDebug() << "details:" << details;
}

void Widget::replyFinished(QNetworkReply *reply)
{
    ui->tableWidget->clear();
    setTableHorizontalHeader();     //设置表头
    QJsonObject data = QJsonDocument::fromJson(reply->readAll()).object();

    //获取历史天气信息
    getHistoryWeatherInfo(data);

    //获取当前天气信息
    getTodayWeatherInfo(data);

    //获取未来天气信息
    getForecastWeatherInfo(data);

    //获取其他天气信息
    getOtherInfo(data);
}
