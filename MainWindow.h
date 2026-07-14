#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>
#include <QButtonGroup>
#include <QtCharts/QChartView>
#include "StudentTableModel.h"
#include "StatisticsService.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void loadDataAsync();
    void onAddStudent();
    void onDeleteStudent();
    void onExportCSV();
    //图表槽函数
    void onCategoryChanged(int id);
    void onChartTypeChanged(int id);
    void onCategorySwitchClicked();
    void onChartTypeSwitchClicked();
    void updateChart();

private:
    void updateButtonTexts();
    Ui::MainWindow *ui;
    QChartView *chartView;
    QButtonGroup *categoryGroup;
    QButtonGroup *chartTypeGroup;
    StudentTableModel *m_tableModel;
    bool m_isPie;
    StatisticsService::StatisticsCategory m_currentCategory;
    QThread* m_loadThread = nullptr;
    bool m_isLoading = false;
};

#endif // MAINWINDOW_H
