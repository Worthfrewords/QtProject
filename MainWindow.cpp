#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "LoadDataWorker.h"
#include "StatisticsService.h"
#include "StudentService.h"
#include "SessionManager.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QTextStream>
#include <QtConcurrent/QtConcurrent>
#include <QFutureWatcher>
#include <QInputDialog>
#include <QDateTime>
#include <QtCharts/QChart>
#include <QtCharts/QPieSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QBarSeries>
#include <QStringConverter>
#include <QSqlDatabase>
#include <QBarCategoryAxis>
#include <QValueAxis>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_tableModel(new StudentTableModel(this))
{
    ui->setupUi(this);
    ui->tableView->setModel(m_tableModel);
    ui->tableView->setEditTriggers(QAbstractItemView::DoubleClicked);
    ui->lblUserInfo->setText(QString("当前用户: %1 (%2)").arg(SessionManager::getInstance().getUsername(),SessionManager::getInstance().isAdmin() ? "管理员" : "普通用户"));

    if (!SessionManager::getInstance().isAdmin()) {
        ui->btnAdd->setEnabled(false);
        ui->btnDelete->setEnabled(false);
    }

    chartView = ui->chartView;
    updateChart();
    m_currentCategory = StatisticsService::Class;
    m_isPie = true;
    updateButtonTexts();

    connect(ui->btnRefresh, &QPushButton::clicked,this, &MainWindow::loadDataAsync);
    connect(ui->btnAdd, &QPushButton::clicked,this, &MainWindow::onAddStudent);
    connect(ui->btnDelete, &QPushButton::clicked,this, &MainWindow::onDeleteStudent);
    connect(ui->btnExport, &QPushButton::clicked,this, &MainWindow::onExportCSV);
    connect(ui->btnCategory, &QPushButton::clicked,this, &MainWindow::onCategorySwitchClicked);
    connect(ui->btnChartType, &QPushButton::clicked,this, &MainWindow::onChartTypeSwitchClicked);

    QSettings settings("MyCompany", "StudentMS");
    ui->mainSplitter->restoreState(settings.value("splitterState").toByteArray());

    loadDataAsync();
}

void MainWindow::updateButtonTexts()
{
    QString catText;
    switch (m_currentCategory) {
    case StatisticsService::Gender: catText = "分类：性别"; break;
    case StatisticsService::Age:    catText = "分类：年龄"; break;
    case StatisticsService::Class:  catText = "分类：班级"; break;
    }
    ui->btnCategory->setText(catText);

    ui->btnChartType->setText(m_isPie ? "饼图" : "柱状图");
}

void MainWindow::onCategorySwitchClicked()
{
    switch (m_currentCategory) {
    case StatisticsService::Gender: m_currentCategory = StatisticsService::Age;    break;
    case StatisticsService::Age:    m_currentCategory = StatisticsService::Class;  break;
    case StatisticsService::Class:  m_currentCategory = StatisticsService::Gender; break;
    }
    updateButtonTexts();
    updateChart();
}
void MainWindow::onChartTypeSwitchClicked()
{
    m_isPie = !m_isPie;
    updateButtonTexts();
    updateChart();
}
void MainWindow::onCategoryChanged(int id) {
    Q_UNUSED(id)
    updateChart();
}
void MainWindow::onChartTypeChanged(int id) {
    Q_UNUSED(id)
    updateChart();
}

void MainWindow::updateChart()
{
    StatisticsService &service = StatisticsService::getInstance();
    QChart *chart = new QChart();
    chart->setTitle("学生统计");
    chart->setAnimationOptions(QChart::SeriesAnimations);

    if (m_isPie) {
        QPieSeries *series = service.getPieChartByCategory(m_currentCategory);
        chart->addSeries(series);
        series->setLabelsVisible(true);
    } else {
        QBarSeries *series = service.getBarChartByCategory(m_currentCategory);
        chart->addSeries(series);

        QBarCategoryAxis *axisX = new QBarCategoryAxis();
        QStringList categories = series->property("categories").toStringList();
        if (!categories.isEmpty()) {
            axisX->append(categories);
        }
        chart->addAxis(axisX, Qt::AlignBottom);
        series->attachAxis(axisX);

        QValueAxis *axisY = new QValueAxis();
        chart->addAxis(axisY, Qt::AlignLeft);
        series->attachAxis(axisY);
    }

    ui->chartView->setChart(chart);  // 旧图表自动释放
}

MainWindow::~MainWindow() {
    QSettings settings("MyCompany", "StudentMS");
    settings.setValue("splitterState", ui->mainSplitter->saveState());

    if (m_loadThread && m_loadThread->isRunning()) {
        m_loadThread->quit();
        if (!m_loadThread->wait(3000)) {
            qWarning() << "线程退出超时，强制终止";
            m_loadThread->terminate();
            m_loadThread->wait();
        }
    }
    delete ui;
}

void MainWindow::loadDataAsync() {
    if (m_isLoading) {
        qDebug() << "[MainWindow] 加载中，忽略本次刷新请求";
        return;
    }

    if (m_loadThread) {
        if (m_loadThread->isRunning()) {
            m_loadThread->quit();
            if (!m_loadThread->wait(2000)) {
                m_loadThread->terminate();
                m_loadThread->wait();
            }
        }
        m_loadThread = nullptr;
    }

    m_isLoading = true;
    m_loadThread = new QThread(this);
    LoadDataWorker *worker = new LoadDataWorker();
    worker->moveToThread(m_loadThread);

    connect(m_loadThread, &QThread::started, worker, &LoadDataWorker::doWork);
    connect(worker, &LoadDataWorker::dataReady, this, [this](const QList<StudentEntity>& data) {
        m_tableModel->setDataList(data);
        m_isLoading = false;
    });
    connect(worker, &LoadDataWorker::dataReady, m_loadThread, &QThread::quit);
    connect(m_loadThread, &QThread::finished, worker, &LoadDataWorker::deleteLater);
    connect(m_loadThread, &QThread::finished, this, [this]() {
        if (m_loadThread) {
            m_loadThread->deleteLater();
            m_loadThread = nullptr;
        }
    });

    m_loadThread->start();
}

void MainWindow::onAddStudent() {
    bool ok;
    QString no = QInputDialog::getText(this, "添加学生", "学号:", QLineEdit::Normal, "", &ok);
    if (!ok || no.isEmpty()) return;
    QString name = QInputDialog::getText(this, "添加学生", "姓名:", QLineEdit::Normal, "", &ok);
    if (!ok || name.isEmpty()) return;
    QString genderStr = QInputDialog::getItem(this, "添加学生", "性别:", QStringList() << "男" << "女", 0, false, &ok);
    if (!ok) return;
    QString birthdayStr = QInputDialog::getText(this, "添加学生", "出生日期 (yyyy-MM-dd):", QLineEdit::Normal, "2000-01-01", &ok);
    if (!ok || birthdayStr.isEmpty()) return;
    QDate birthday = QDate::fromString(birthdayStr, "yyyy-MM-dd");
    if (!birthday.isValid()) {
        QMessageBox::warning(this, "格式错误", "日期格式无效，请使用 yyyy-MM-dd");
        return;
    }
    QString cls = QInputDialog::getText(this, "添加学生", "班级:", QLineEdit::Normal, "", &ok);
    if (!ok) return;

    StudentEntity s;
    s.studentNo = no;
    s.name = name;
    s.gender = (genderStr == "男") ? 'M' : 'F';
    s.birthday = birthday;
    s.className = cls;
    s.createBy = SessionManager::getInstance().getUserId();

    if (StudentService::getInstance().addStudent(s)) {
        QMessageBox::information(this, "成功", "添加学生成功！");
        loadDataAsync();
    } else {
        QMessageBox::critical(this, "错误", "添加失败，请检查学号是否重复或数据库错误。");
    }
}

void MainWindow::onDeleteStudent() {
    QModelIndex index = ui->tableView->currentIndex();
    if (!index.isValid()) {
        QMessageBox::information(this, "提示", "请先选中要删除的行。");
        return;
    }
    int id = m_tableModel->data(index, Qt::UserRole).toInt();
    if (id <= 0) return;

    QMessageBox::StandardButton reply = QMessageBox::question(this, "确认删除","确定要删除该学生吗？此操作不可撤销！",QMessageBox::Yes | QMessageBox::No);
    if (reply != QMessageBox::Yes) return;

    if (StudentService::getInstance().deleteStudent(id)) {
        QMessageBox::information(this, "成功", "删除成功！");
        loadDataAsync();
    } else {
        QMessageBox::critical(this, "错误", "删除失败，可能由于权限不足或数据库错误。");
    }
}

void MainWindow::onExportCSV() {
    QString filePath = QFileDialog::getSaveFileName(this, "导出 CSV", "./students.csv", "CSV Files (*.csv)");
    if (filePath.isEmpty()) return;

    QList<StudentEntity> data = m_tableModel->getRawData();
    if (data.isEmpty()) {
        QMessageBox::information(this, "提示", "没有数据可导出。");
        return;
    }

    QFutureWatcher<bool> *watcher = new QFutureWatcher<bool>(this);
    QFuture<bool> future = QtConcurrent::run([filePath, data]() {
        QFile file(filePath);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) return false;
        QTextStream out(&file);
        out.setEncoding(QStringConverter::Utf8);
        out << "\xEF\xBB\xBF";
        out << "学号,姓名,性别,年龄,班级\n";
        int currentYear = QDate::currentDate().year();
        for (const auto& s : data) {
            int age = currentYear - s.birthday.year();
            out << s.studentNo << "," << s.name << ","
                << (s.gender == 'M' ? "男" : "女") << ","
                << age << "," << s.className << "\n";
        }
        return true;
    });

    watcher->setFuture(future);
    connect(watcher, &QFutureWatcher<bool>::finished, this, [this, watcher]() {
        bool success = watcher->result();
        QMessageBox::information(this, "导出结果", success ? "导出成功！" : "导出失败！");
        watcher->deleteLater();
    });
}
