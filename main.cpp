#include <QApplication>
#include <QDir>
#include <QStandardPaths>
#include <QDebug>
#include "LoginDialog.h"
#include "MainWindow.h"
#include "DatabaseManager.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    a.setApplicationName("学生管理系统");

    //获取数据目录
    QString dataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dir(dataDir);
    if (!dir.mkpath(dataDir)) {
        qCritical() << "无法创建数据目录:" << dataDir;
        return -1;
    }
    DatabaseManager::setDatabasePath(dataDir + "/school.db");

    //初始化数据库
    if (!DatabaseManager::getInstance().initDatabase()) {
        qCritical() << "数据库初始化失败";
        return -1;
    }

    LoginDialog login;
    if (login.exec() == QDialog::Accepted) {
        MainWindow w;
        w.show();
        return a.exec();
    }
    return 0;
}
