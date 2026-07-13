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

//设置/获取当前分类
void StatisticsService::setCurrentCategory(StatisticsCategory category) {
    m_currentCategory = category;
}
StatisticsService::StatisticsCategory StatisticsService::currentCategory() const {
    return m_currentCategory;
}
QBarSeries* StatisticsService::getBarChartByCategory(StatisticsCategory category) {
    switch (category) {
    case Gender: return getGenderBar();
    case Age:    return getAgeBar();
    case Class:  return getClassBar();
    default:     return new QBarSeries();
    }
}
QPieSeries* StatisticsService::getPieChartByCategory(StatisticsCategory category) {
    switch (category) {
    case Gender: return getGenderPie();
    case Age:    return getAgePie();
    case Class:  return getClassPie();
    default:     return new QPieSeries();
    }
}


QBarSeries* StatisticsService::getGenderBar() {
    QSqlDatabase db = DatabaseManager::getInstance().getDatabase();
    if (!db.isOpen()) return new QBarSeries();
    QSqlQuery query(db);
    query.prepare("SELECT gender, COUNT(*) FROM students GROUP BY gender");
    if (!query.exec()) {
        qDebug() << "性别统计失败:" << query.lastError().text();
        return new QBarSeries();
    }
    QBarSeries *series = new QBarSeries();
    QBarSet *maleSet = new QBarSet("男生");
    QBarSet *femaleSet = new QBarSet("女生");
    *maleSet << 0;
    *femaleSet << 0;
    while (query.next()) {
        QString gender = query.value(0).toString();
        int count = query.value(1).toInt();
        if (gender == "M") {
            maleSet->replace(0, count);
        } else {
            femaleSet->replace(0, count);
        }
    }
    series->append(maleSet);
    series->append(femaleSet);
    return series;
}

QBarSeries* StatisticsService::getAgeBar() {
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
        // 空数据时也可设置一个占位标签
        series->setProperty("categories", QStringList{"无数据"});
        return series;
    }
    QBarSet *set = new QBarSet("人数");
    QStringList categories;          // 用于保存横轴标签
    // 遍历 QMap 时同时填充数据与标签（QMap 默认按年龄升序）
    for (auto it = ageMap.constBegin(); it != ageMap.constEnd(); ++it) {
        *set << it.value();                       // 对应年龄的人数
        categories << QString("%1岁").arg(it.key()); // 横轴显示18岁,19岁等
    }
    series->append(set);
    series->setProperty("categories", categories); // 将标签传递给 UI
    return series;
}

QBarSeries* StatisticsService::getClassBar() {
    QSqlDatabase db = DatabaseManager::getInstance().getDatabase();
    if (!db.isOpen()) return new QBarSeries();
    QSqlQuery query(db);
    query.prepare("SELECT class_name, COUNT(*) FROM students GROUP BY class_name ORDER BY class_name");
    if (!query.exec()) {
        qDebug() << "班级统计失败:" << query.lastError().text();
        return new QBarSeries();
    }
    QBarSeries *series = new QBarSeries();
    QBarSet *set = new QBarSet("人数");
    QStringList categories;
    while (query.next()) {
        categories << query.value(0).toString();
        *set << query.value(1).toInt();
    }
    if (categories.isEmpty()) {
        *set << 0;
    }
    series->append(set);
    // 类别标签可通过属性或全局变量传递，这里简单处理：将分类名称存入 series 的 meta 数据
    series->setProperty("categories", categories);
    return series;
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

QPieSeries* StatisticsService::getAgePie() {
    QSqlDatabase db = DatabaseManager::getInstance().getDatabase();
    if (!db.isOpen()) return new QPieSeries();
    QSqlQuery query(db);
    query.prepare("SELECT birthday FROM students");
    if (!query.exec()) {
        qDebug() << "年龄统计失败:" << query.lastError().text();
        return new QPieSeries();
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
    QPieSeries *series = new QPieSeries();
    if (ageMap.isEmpty()) {
        series->append("无数据", 1);
        return series;
    }
    // 按年龄升序遍历，保证扇区顺序
    for (auto it = ageMap.constBegin(); it != ageMap.constEnd(); ++it) {
        series->append(QString("%1岁").arg(it.key()), it.value());
    }
    return series;
}

QPieSeries* StatisticsService::getClassPie() {
    QSqlDatabase db = DatabaseManager::getInstance().getDatabase();
    if (!db.isOpen()) return new QPieSeries();
    QSqlQuery query(db);
    query.prepare("SELECT class_name, COUNT(*) FROM students GROUP BY class_name ORDER BY class_name");
    if (!query.exec()) {
        qDebug() << "班级统计失败:" << query.lastError().text();
        return new QPieSeries();
    }
    QPieSeries *series = new QPieSeries();
    while (query.next()) {
        QString className = query.value(0).toString();
        int count = query.value(1).toInt();
        series->append(className, count);
    }
    if (series->isEmpty()) {
        series->append("无数据", 1);
    }
    return series;
}
