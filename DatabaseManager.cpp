#include "DatabaseManager.h"
#include <QCryptographicHash>
#include <QCoreApplication>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QDir>
#include <QFileInfo>

QString DatabaseManager::m_dbPath;

void DatabaseManager::setDatabasePath(const QString& path) {
    m_dbPath = path;
}

QString DatabaseManager::databasePath() {
    return m_dbPath;
}

bool DatabaseManager::initDatabase() {
    if (m_dbPath.isEmpty()) {
        // 回退方案（一般不会走到这里）
        m_dbPath = QCoreApplication::applicationDirPath() + "/data/school.db";
    }

    // 确保目录存在
    QFileInfo fileInfo(m_dbPath);
    QDir dir = fileInfo.absoluteDir();
    if (!dir.exists()) {
        if (!dir.mkpath(".")) {
            qDebug() << "无法创建数据库目录:" << dir.absolutePath();
            return false;
        }
    }

    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(m_dbPath);

    if (!m_db.open()) {
        qDebug() << "数据库链接失败:" << m_db.lastError().text();
        return false;
    }
    return createTables();
}

bool DatabaseManager::createTables() {
    QSqlQuery query;
    if (!query.exec("CREATE TABLE IF NOT EXISTS users ("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                    "username TEXT UNIQUE NOT NULL, "
                    "password TEXT NOT NULL, "
                    "role TEXT DEFAULT 'user' CHECK(role IN ('admin','user')))")) {
        qDebug() << "建表失败:" << query.lastError();
        return false;
    }
    if (!query.exec("CREATE TABLE IF NOT EXISTS students ("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                    "student_no TEXT UNIQUE NOT NULL, "
                    "name TEXT NOT NULL, "
                    "gender TEXT CHECK(gender IN ('M','F')), "
                    "birthday TEXT NOT NULL, "
                    "class_name TEXT, "
                    "create_by INTEGER, "
                    "created_at TEXT DEFAULT CURRENT_TIMESTAMP, "
                    "FOREIGN KEY (create_by) REFERENCES users(id) ON DELETE SET NULL)")) {
        qDebug() << "建表失败:" << query.lastError();
        return false;
    }

    // 插入默认管理员
    query.prepare("INSERT OR IGNORE INTO users (username, password, role) VALUES (?, ?, ?)");
    QString hashedPwd = QCryptographicHash::hash(QString("admin123").toUtf8(), QCryptographicHash::Sha256).toHex();
    query.addBindValue("Admin");
    query.addBindValue(hashedPwd);
    query.addBindValue("admin");
    if (!query.exec()) {
        qDebug() << "插入默认管理员失败:" << query.lastError();
    }

    // 插入测试学生（仅当表为空时）
    query.exec("SELECT COUNT(*) FROM students");
    if (query.next() && query.value(0).toInt() == 0) {
        query.prepare("INSERT INTO students (student_no, name, gender, birthday, class_name, create_by) "
                      "VALUES (?, ?, ?, ?, ?, 1)");
        query.addBindValue("S001"); query.addBindValue("张三"); query.addBindValue("M"); query.addBindValue("2002-05-15"); query.addBindValue("软件工程1班"); query.exec();
        query.addBindValue("S002"); query.addBindValue("李四"); query.addBindValue("F"); query.addBindValue("2003-08-21"); query.addBindValue("软件工程1班"); query.exec();
        query.addBindValue("S003"); query.addBindValue("王五"); query.addBindValue("M"); query.addBindValue("2001-12-01"); query.addBindValue("软件工程2班"); query.exec();
    }
    return true;
}

DatabaseManager& DatabaseManager::getInstance() {
    static DatabaseManager instance;
    return instance;
}
