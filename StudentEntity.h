#ifndef STUDENTENTITY_H
#define STUDENTENTITY_H

#include <QString>
#include <QDate>

//维护学生数据
struct StudentEntity {
    int id = 0;
    QString studentNo;
    QString name;
    char gender = 'M'; // 'M' or 'F'
    QDate birthday;
    QString className;
    int createBy = 0; // 关联 users.id 0为普通用户
};

#endif // STUDENTENTITY_H
