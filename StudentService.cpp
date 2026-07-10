#include "StudentService.h"
#include "DatabaseManager.h"
#include "SessionManager.h"
#include <QSqlQuery>
#include <QVariant>
#include <QDebug>

QList<StudentEntity> StudentService::getAllStudents() {
    QList<StudentEntity> list;
    QSqlDatabase db = DatabaseManager::getInstance().getDatabase();
    if (!db.isOpen()) return list;

    QSqlQuery query(db);
    QString sql = "SELECT id, student_no, name, gender, birthday, class_name, create_by FROM students WHERE 1=1 ";
    if (!SessionManager::getInstance().isAdmin()) {
        sql += " AND create_by = :uid ";
    }
    query.prepare(sql);
    if (!SessionManager::getInstance().isAdmin()) {
        query.bindValue(":uid", SessionManager::getInstance().getUserId());
    }
    if (!query.exec()) {
        qDebug() << "查询学生失败:" << query.lastError().text();
        return list;
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
        list.append(s);
    }
    return list;
}

bool StudentService::deleteStudent(int studentId) {
    QSqlDatabase db = DatabaseManager::getInstance().getDatabase();
    if (!db.isOpen()) return false;

    QSqlQuery query(db);
    if (!SessionManager::getInstance().isAdmin()) {
        query.prepare("SELECT create_by FROM students WHERE id = ?");
        query.addBindValue(studentId);
        if (query.exec() && query.next()) {
            if (query.value(0).toInt() != SessionManager::getInstance().getUserId())
                return false;
        } else return false;
    }

    query.prepare("DELETE FROM students WHERE id = ?");
    query.addBindValue(studentId);
    if (!query.exec()) {
        qDebug() << "删除学生失败:" << query.lastError().text();
        return false;
    }
    return query.numRowsAffected() > 0;
}

bool StudentService::addStudent(const StudentEntity& student) {
    QSqlDatabase db = DatabaseManager::getInstance().getDatabase();
    if (!db.isOpen()) return false;

    QSqlQuery query(db);
    query.prepare("INSERT INTO students "
                  "(student_no, name, gender, birthday, class_name, create_by) "
                  "VALUES (:no, :name, :gender, :birthday, :class, :uid)");
    query.bindValue(":no", student.studentNo);
    query.bindValue(":name", student.name);
    query.bindValue(":gender", QString(student.gender));
    query.bindValue(":birthday", student.birthday.toString("yyyy-MM-dd"));
    query.bindValue(":class", student.className);
    query.bindValue(":uid", SessionManager::getInstance().getUserId());

    if (!query.exec()) {
        qDebug() << "添加学生失败:" << query.lastError().text();
        return false;
    }
    return true;
}

bool StudentService::updateStudent(const StudentEntity& student) {
    QSqlDatabase db = DatabaseManager::getInstance().getDatabase();
    if (!db.isOpen()) return false;

    QSqlQuery query(db);
    if (!SessionManager::getInstance().isAdmin()) {
        query.prepare("SELECT create_by FROM students WHERE id = ?");
        query.addBindValue(student.id);
        if (query.exec() && query.next()) {
            if (query.value(0).toInt() != SessionManager::getInstance().getUserId())
                return false;
        } else return false;
    }

    query.prepare("UPDATE students SET "
                  "student_no = :no, "
                  "name = :name, "
                  "gender = :gender, "
                  "birthday = :birthday, "
                  "class_name = :class "
                  "WHERE id = :id");
    query.bindValue(":no", student.studentNo);
    query.bindValue(":name", student.name);
    query.bindValue(":gender", QString(student.gender));
    query.bindValue(":birthday", student.birthday.toString("yyyy-MM-dd"));
    query.bindValue(":class", student.className);
    query.bindValue(":id", student.id);

    if (!query.exec()) {
        qDebug() << "更新学生失败:" << query.lastError().text();
        return false;
    }
    return query.numRowsAffected() > 0;
}

StudentService& StudentService::getInstance() {
    static StudentService instance;
    return instance;
}
