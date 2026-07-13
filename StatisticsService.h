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
    // 统计分类枚举
    enum StatisticsCategory {
        Gender,
        Age,
        Class //默认
    };

    static StatisticsService& getInstance();
    QPieSeries* getPieChartByCategory(StatisticsCategory category);
    QBarSeries* getBarChartByCategory(StatisticsCategory category);
    //设置/获取当前分类
    void setCurrentCategory(StatisticsCategory category);
    StatisticsCategory currentCategory() const;

private:
    StatisticsService() : m_currentCategory(Class) {}  // 默认班级
    StatisticsCategory m_currentCategory;
    QStringList m_lastCategories;    // 缓存最近一次查询的类别顺序
    // 各分类的具体实现
    QBarSeries* getGenderBar();
    QBarSeries* getAgeBar();
    QBarSeries* getClassBar();
    QPieSeries* getGenderPie();
    QPieSeries* getAgePie();
    QPieSeries* getClassPie();
};

#endif // STATISTICSSERVICE_H
