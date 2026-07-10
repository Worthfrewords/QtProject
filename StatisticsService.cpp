#include "StatisticsService.h"
#include "DatabaseManager.h"
#include <QDate>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QtCharts/QPieSeries>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>

StatisticsService& StatisticsService::getInstance() {
    static StatisticsService instance;
    return instance;
}

QPieSeries* StatisticsService::getGenderPie() {
    QSqlDatabase db = DatabaseManager::getInstance().getDatabase();
    if (!db.isOpen()) return new QPieSeries();

    QSqlQuery query(db);
    query.prepare("SELECT gender, COUNT(*) FROM students GROUP BY gender");
    if (!query.exec()) {
        qDebug() << "性别统计失败:" << query.lastError().text();
        return new QPieSeries();
    }

    QPieSeries *series = new QPieSeries();
    while (query.next()) {
        QString gender = query.value(0).toString();
        int count = query.value(1).toInt();
        QString label = (gender == "M") ? "男生" : "女生";
        series->append(label, count);
    }
    if (series->isEmpty()) {
        series->append("无数据", 1);
    }
    return series;
}

QBarSeries* StatisticsService::getAgeBarChart() {
    QSqlDatabase db = DatabaseManager::getInstance().getDatabase();
    if (!db.isOpen()) return new QBarSeries();

    QSqlQuery query(db);
    query.prepare("SELECT birthday FROM students");
    if (!query.exec()) {
        qDebug() << "年龄统计失败:" << query.lastError().text();
        return new QBarSeries();
    }

    QMap<int, int> ageMap;
    int currentYear = QDate::currentDate().year();
    while (query.next()) {
        QDate birth = QDate::fromString(query.value(0).toString(), "yyyy-MM-dd");
        if (birth.isValid()) {
            int age = currentYear - birth.year();
            ageMap[age] = ageMap.value(age, 0) + 1;
        }
    }

    QBarSeries *series = new QBarSeries();
    if (ageMap.isEmpty()) {
        QBarSet *set = new QBarSet("年龄分布");
        *set << 0;
        series->append(set);
        return series;
    }

    QBarSet *set = new QBarSet("人数");
    for (auto it = ageMap.begin(); it != ageMap.end(); ++it) {
        *set << it.value();
    }
    series->append(set);
    return series;
}
