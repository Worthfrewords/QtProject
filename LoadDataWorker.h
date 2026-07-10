#ifndef LOADDATAWORKER_H
#define LOADDATAWORKER_H

// LoadDataWorker.h
#include <QObject>
#include <QList>
#include <QThread>
#include "StudentEntity.h"

class LoadDataWorker : public QObject {
    Q_OBJECT
public:
    explicit LoadDataWorker(QObject *parent = nullptr) : QObject(parent) {}
public slots:
    void doWork();
signals:
    void dataReady(const QList<StudentEntity>& data);
};


#endif // LOADDASTAWORKEWR_H
