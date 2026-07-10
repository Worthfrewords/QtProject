#ifndef STUDENTSERVICE_H
#define STUDENTSERVICE_H

// StudentService.h
#include <QList>
#include "StudentEntity.h"

class StudentService {
public:
    static StudentService& getInstance();
    QList<StudentEntity> getAllStudents(); // 管理员查所有，普通用户仅查自己添加的
    bool addStudent(const StudentEntity& student);
    bool updateStudent(const StudentEntity& student);
    bool deleteStudent(int studentId);
private:
    StudentService() = default;
    QString getCurrentUserFilter();
    //查找学生对象的输出过滤器,返回 "AND create_by = ?" 或 ""
};


#endif // STUDENTSERVICE_H
