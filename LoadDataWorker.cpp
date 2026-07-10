#include "LoadDataWorker.h"
#include "DatabaseManager.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QDate>
#include <QThread>
#include <QCoreApplication>
#include <QStandardPaths>

void LoadDataWorker::doWork() {
    QList<StudentEntity> data;

    // 使用唯一连接名，基于当前线程ID，避免残留连接冲突
    const QString connName = QString("worker_%1").arg((quintptr)QThread::currentThreadId());

    // 如果该连接名已存在（极罕见情况），先移除（在子线程内安全）
    if (QSqlDatabase::contains(connName)) {
        QSqlDatabase::removeDatabase(connName);
    }

    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", connName);
        QString dbPath = DatabaseManager::databasePath();
        db.setDatabaseName(dbPath);

        if (!db.open()) {
            qDebug() << "[Worker] 打开数据库失败：" << db.lastError().text();
            emit dataReady(data);
            return; // db 析构时会自动关闭，但连接可能残留，下次同线程ID会复用
        }

        {
            QSqlQuery query(db);
            if (!query.exec("SELECT id, student_no, name, gender, birthday, class_name, create_by FROM students")) {
                qDebug() << "[Worker] 查询失败：" << query.lastError().text();
                db.close();
                QSqlDatabase::removeDatabase(connName);
                emit dataReady(data);
                return;
            }
            while (query.next()) {
                StudentEntity s;
                s.id = query.value(0).toInt();
                s.studentNo = query.value(1).toString();
                s.name = query.value(2).toString();
                s.gender = query.value(3).toString().isEmpty() ? 'M' : query.value(3).toString().at(0).toLatin1();
                s.birthday = QDate::fromString(query.value(4).toString(), "yyyy-MM-dd");
                s.className = query.value(5).toString();
                s.createBy = query.value(6).toInt();
                data.append(s);
            }
        } // query 销毁

        db.close();
        QSqlDatabase::removeDatabase(connName); // 子线程内安全移除
    }

    qDebug() << "[Worker] 加载完成，共" << data.size() << "条记录";
    emit dataReady(data);
}
