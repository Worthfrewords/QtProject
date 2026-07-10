#ifndef STATISTICSSERVICE_H
#define STATISTICSSERVICE_H

#include <QObject>
#include <QtCharts/QPieSeries>
#include <QtCharts/QBarSeries>
#include <QStringList>

class StatisticsService : public QObject
{
    Q_OBJECT
public:
    static StatisticsService& getInstance();

    QPieSeries* getGenderPie();
    QBarSeries* getAgeBarChart();

private:
    StatisticsService() = default;
    QStringList m_lastCategories;    // 缓存最近一次查询的类别顺序
};

#endif // STATISTICSSERVICE_H
