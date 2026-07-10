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

    // 1. 获取系统安全数据目录（如 Windows 的 AppData/Local/学生管理系统）
    QString dataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dir(dataDir);
    if (!dir.mkpath(dataDir)) {
        qCritical() << "无法创建数据目录:" << dataDir;
        return -1;
    }
    // 2. 设置数据库路径（必须放在 initDatabase 之前）
    DatabaseManager::setDatabasePath(dataDir + "/school.db");

    // 3. 初始化数据库（将在上述路径创建/打开文件）
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
