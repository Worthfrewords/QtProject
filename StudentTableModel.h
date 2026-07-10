#ifndef STUDENTTABLEMODEL_H
#define STUDENTTABLEMODEL_H

#include <QAbstractTableModel>
#include "StudentEntity.h"

class StudentTableModel : public QAbstractTableModel {
    Q_OBJECT

public:
    explicit StudentTableModel(QObject *parent = nullptr);

    void setDataList(const QList<StudentEntity>& data);
    QList<StudentEntity> getRawData() const { return m_data; }

    // QAbstractTableModel 接口
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;

private:
    QList<StudentEntity> m_data;
    QStringList m_headers{"学号", "姓名", "性别", "年龄", "班级"};
};

#endif // STUDENTTABLEMODEL_H
