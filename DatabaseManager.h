#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

// DatabaseManager.h
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

class DatabaseManager {
public:
    static void setDatabasePath(const QString& path);
    static DatabaseManager& getInstance();
    bool initDatabase();
    static QString databasePath();
    QSqlDatabase getDatabase() const { return m_db; }
private:
    DatabaseManager() = default;
    QSqlDatabase m_db;
    bool createTables();
    static QString m_dbPath;
};


#endif // DATABASEMANAGER_H
