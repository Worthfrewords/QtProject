#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>
#include "StudentTableModel.h"

// 引入编译生成的 UI 类（由 .ui 文件自动生成）
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


private:
    QThread* m_loadThread = nullptr;
    bool m_isLoading = false;
    Ui::MainWindow *ui;          // 唯一界面指针，替代所有手写 new 的控件
    StudentTableModel *m_tableModel;
    bool m_isDarkTheme = false;
};

#endif // MAINWINDOW_H
